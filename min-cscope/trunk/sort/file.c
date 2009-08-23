/*******************************************************************************
 * Copyright (C) 2009 Elad Lahav
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "internal.h"

#define MIN_FILE_SIZE (16 << PAGE_SIZE_ORDER)
#define FILE_COOKIE 0x40da892f

u32 system_page_size = 0;
#if defined(USE_SYSTEM_PAGE_SIZE)
u32 system_page_size_order = 0;
#endif // defined(USE_SYSTEM_PAGE_SIZE)

typedef struct {
    file_t*     file;
    page_hdr_t* page;
    page_id_t   pageid;
    rec_id_t    recid;
} file_itr_t;

int get_system_page_size()
{
#if defined(USE_SYSTEM_PAGE_SIZE)
    u32 temp;
#endif // defined(USE_SYSTEM_PAGE_SIZE)
        
    system_page_size = os_get_page_size();
    
    if ((system_page_size & (system_page_size - 1)) != 0) {
        fprintf(stderr, "System page size (0x%x) is not a power of 2\n",
                system_page_size);
        return -1;
    }
        
    if (system_page_size < (1 << 12) || system_page_size > (1 << 26)) {
        fprintf(stderr, "System page size (0x%x) is not supported\n",
                system_page_size);
        return -1;
    }
    
#if !defined(USE_SYSTEM_PAGE_SIZE)
    if (PAGE_SIZE < system_page_size) {
        fprintf(stderr, "The pre-defined page size (0x%x) is not a multiple of"
                " the system page size (0x%x)\nThe application needs to be"
                " recompiled.\n", PAGE_SIZE, system_page_size);
        return -1;
    }
#else
    for (temp = system_page_size;
         temp > 1;
         temp >>= 1, system_page_size_order++) {
    }
#endif // !defined(USE_SYSTEM_PAGE_SIZE)
    
    DPRINT("System page size: 0x%x (order %u)\n", PAGE_SIZE, PAGE_SIZE_ORDER);
    return 0;
}

file_t* file_open(const char* path, int forcenew)
{
    file_handle_t handle = INVALID_FILE_HANDLE;
    file_hdr_t* hdr = NULL;
    file_t* file;
    int result;
    u32 size;
    
    if (system_page_size == 0) {
        if (get_system_page_size() < 0)
            return NULL;
    }
    
    /* Open/create the backing file. */
    result = os_open_file(path, &handle);
    if (result < 0)
        goto error;

    /* Make sure the file is at least 16 pages in size. */
    if (os_get_file_size(handle, &size) < 0)
        goto error;

    if (size < MIN_FILE_SIZE) {
        if (os_set_file_size(handle, MIN_FILE_SIZE, &size) < 0)
            goto error;

        if (size < MIN_FILE_SIZE) {
            fprintf(stderr, "file_open: file with size %u is smaller than the"
                    " minimum size (%u)\n", size, MIN_FILE_SIZE);
            goto error;
        }
    }
    
    /* Map the first page of the file as the B-Tree's header. */
    hdr = (file_hdr_t*)os_get_page(handle, 0);
    if (hdr == NULL)
        goto error;

    if ((result == 0) && (forcenew == 0)) {
        DPRINT("Opening existing file\n");
        
        /* Existing file: validate the cookie. */
        if (hdr->cookie != FILE_COOKIE) {
            fprintf(stderr, "Found a bad file cookie (%x) while opening %s",
                    hdr->cookie, path);
            goto error;
        }
    }
    else {
        DPRINT("Creating a new file\n");
        
        /* New file. Initialize the header. */
        hdr->cookie = FILE_COOKIE;
        hdr->head = 0;
        hdr->tail = 0;
        hdr->free = 0;
        hdr->npages = 0;
        hdr->nrecs = 0;
    }

    /* Create the file object. */
    file = (file_t*)malloc(sizeof(file_t));
    if (file == NULL) {
        fprintf(stderr, "file_open: failed to allocate the file object\n");
        goto error;
    }
    
    /* Fill the B-Tree structure. */
    file->handle = handle;
    file->header = hdr;
    file->size = size >> PAGE_SIZE_ORDER;
    cache_invalidate(&file->cache, 1);
    memset(&file->stats, 0, sizeof(stats_t));
    
    DPRINT("file_open successful\n");
    return file;

error:
    if (handle != INVALID_FILE_HANDLE) {
        if (hdr != NULL)
            os_put_page(handle, hdr);

        os_close_file(handle);
    }
    
    return NULL;
}

void file_dump_info(file_t* file);

int file_insert(file_t* file, const char* data, u32 len)
{
    page_hdr_t* page;
    page_hdr_t* newpage;
    page_id_t newpageid;
    record_t rec;
    int result;

    /* Validate the data length. */
    if (len >= 0xff) {
        fprintf(stderr, "file_insert: data is too long (%u)\n", len);
        return -1;
    }

    DPRINT("file_insert: adding data=%s len=%d\n", data, len);
    
    rec.data = (char*)data;
    rec.len = (u8)len;
    
    /* Check for an available tail page. */
    if (file->header->tail == 0) {
        page = NULL;
        goto new_page;
    }
    
    /* Map the tail page. */
    page = file_get_page(file, file->header->tail);
    if (page == NULL)
        return -1;

    /* Try to add a record to the page. */
    result = page_add_record(page, &rec);
    if (result < 1) {
        file_put_page(file, file->header->tail, page);
        goto done;
    }

new_page:
    /* Get a new page. */
    newpage = file_get_new_page(file, &newpageid);
    if (newpage == NULL)
        return -1;

    /* Link to the current tail, if any. */
    if (page) {
        page->next = newpageid;
        file_put_page(file, file->header->tail, page);
    }

    /* Update the file header. */
    file->header->tail = newpageid;
    if (file->header->head == 0)
        file->header->head = newpageid;
    
    /*
     * Try to add a record to the new page
     * (should not result in a full page error).
     */
    result = page_add_record(newpage, &rec);
    file_put_page(file, newpageid, newpage);
    if (result != 0) {
        fprintf(stderr, "file_insert: failed to add record to an empty page\n");
        return -1;
    }

done:
    if (result == 0)
        file->header->nrecs++;
    DPRINT("file_insert: result=%d\n", result);
    return result;
}

page_hdr_t* file_get_page(file_t* file, page_id_t pageid)
{
    page_hdr_t* page;
    page_id_t oldid;
    void* oldptr;
    
#if !defined(NDEBUG)
    if (pageid >= file->size) {
        fprintf(stderr, "file_get_page: requested page %u, file has %u pages\n",
                pageid, file->size);
        DASSERT(0);
    }
#endif /* !defined(NDEBUG) */

    /* Try to find an already mapped page in the cache. */
    page = (page_hdr_t*)cache_get_page(&file->cache, pageid);
    if (page != NULL)
        return page;

    /* Page was not found in the cache, need to map it. */
    page = (page_hdr_t*)os_get_page(file->handle, pageid);
    if (page == NULL) {
        file_dump_info(file);
        return NULL;
    }

    file->stats.mapped++;
    DPRINT("get_page: mapped page %u (%p)\n", pageid, page);
    
    /* Update the cache. */
    if (cache_replace(&file->cache, pageid, page, &oldid, &oldptr) == 0) {
        if (oldptr != NULL) {
            DPRINT("get_page: unmapping cached page %u (%p)\n", oldid,
                   oldptr);
            if (os_put_page(file->handle, oldptr) == 0)
                file->stats.unmapped++;
        }
    }
    else {
        /* Page could not be placed in the cache. */
        file->stats.ncmapped++;
    }

    return page;
}

page_hdr_t* file_get_new_page(file_t* file, page_id_t* pageidp)
{
    page_id_t id;
    page_hdr_t* page;
    u32 size;

    /* Use a free page, if available. */
    if (file->header->free != 0) {
        id = file->header->free;
        page = file_get_page(file, id);
        if (page != NULL) {
            /* Update the free page list. */
            file->header->free = page->next;
            goto done;
        }
    }

    /* Get a page from the end of the file. */
    id = file->header->npages + 1;
    if (id == file->size) {
        /* Double the size of the file. */
        size = file->size << (PAGE_SIZE_ORDER + 1);
        if (os_set_file_size(file->handle, size, &size) < 0)
            return NULL;

        file->size = size >> PAGE_SIZE_ORDER;
    }

    page = file_get_page(file, id);
    
done:
    page->next = 0;
    page->nrecs = 0;
    page->nextoff = sizeof(page_hdr_t);
    page->freebytes = PAGE_SIZE - sizeof(page_hdr_t);
    
    *pageidp = id;
    file->header->npages++;
    return page;
}

void file_put_page(file_t* file, page_id_t pageid, page_hdr_t* page)
{
    if (cache_dec_ref(&file->cache, pageid) < 0) {
        DPRINT("file_put_page: unmapping non-cached page %u (%p)\n", pageid,
               page);
        if (os_put_page(file->handle, page) == 0)
            file->stats.unmapped++;
    }
}

void file_free_page(file_t* file, page_id_t pageid, page_hdr_t* page)
{
    DPRINT("file_free_page: %u->%u\n", pageid, file->header->free);
    page->next = file->header->free;
    file->header->free = pageid;
    file->header->npages--;
    file_put_page(file, pageid, page);
}

void file_dump_info(file_t* file)
{
    fprintf(stderr, "=== File Information ===\n");
    fprintf(stderr, "Total pages:             %u\n", file->size);
    fprintf(stderr, "Used pages:              %u\n", file->header->npages);
    fprintf(stderr, "Records:                 %u\n", file->header->nrecs);
    fprintf(stderr, "First page:              %u\n", file->header->head);
    fprintf(stderr, "=== File Statistics ===\n");
    fprintf(stderr, "Mapped pages:            %u\n", file->stats.mapped);
    fprintf(stderr, "Unmapped pages:          %u\n", file->stats.unmapped);
    fprintf(stderr, "Non-cached mapped pages: %u\n", file->stats.ncmapped);
    fprintf(stderr, "Cache hits:              %u\n", file->cache.hits);
    fprintf(stderr, "Cache misses:            %u\n", file->cache.misses);
}

void file_close(file_t* file)
{
    u32 i;
    cache_entry_t* entry;
    
    /* Unmap cached pages. */
    for (i = 0; i < CACHE_SIZE; i++) {
        entry = &file->cache.entries[i];
        if (entry->ptr != NULL) {
            if (entry->ref > 0) {
                fprintf(stderr, "file_close: unmapping used page %u (%p)\n",
                        entry->pageid, entry->ptr);
            }
            
            DPRINT("file_close: unmapping cached page %u (%p)\n",
                   entry->pageid, entry->ptr);
            if (os_put_page(file->handle, entry->ptr) == 0)
                file->stats.unmapped++;
        }
    }
    
#if !defined(NSTATS)
    file_dump_info(file);
#endif /* !defined(NSTATS) */

    /* Unmap the header page. */
    os_put_page(file->handle, file->header);

    /* Close the file. */
    os_close_file(file->handle);

    /* Free the file object. */
    memset(file, 0, sizeof(file_t));
    free(file);
}

file_itr_t* file_itr_init(file_t* file)
{
    file_itr_t* itr;

    /* Allocate the iterator object. */
    itr = (file_itr_t*)malloc(sizeof(file_itr_t));
    if (itr == NULL) {
        fprintf(stderr, "file_itr_init: failed to allocate iterator object\n");
        return NULL;
    }
    
    /* Get the first page. */
    itr->page = file_get_page(file, file->header->head);
    if (itr->page == NULL) {
        free(itr);
        return NULL;
    }
    
    /* Initialize the rest of the structure. */
    itr->file = file;
    itr->pageid = file->header->head;
    itr->recid = 0;
    return itr;
}

int file_itr_next(file_itr_t* itr, char** datap, u32* lenp)
{
    page_id_t nextid;
    record_t rec;
    
    /* Check for an uninitialized/finished iterator. */
    if (itr->page == NULL)
        return -1;
    
    /* Try to get the record pointed to by the iterator. */
    while (page_get_record(itr->page, itr->recid++, &rec) < 0) {
        /* Finished with this page. Stop if this was the last leaf. */
        nextid = itr->page->next;
        file_put_page(itr->file, itr->pageid, itr->page);
        if (nextid == 0) {
            itr->page = NULL;
            return 1;
        }

        /* Get the next leaf page. */
        itr->page = file_get_page(itr->file, nextid);
        if (itr->page == NULL)
            return -1;

        itr->pageid = nextid;
        itr->recid = 0;
    }

    *datap = rec.data;
    *lenp = rec.len;

#if !defined(NDEBUG)
    printf("%s\n", *datap);
#endif /* !defined(NDEBUG) */
    return 0;
}

void file_itr_finish(file_itr_t* itr)
{
    if (itr->page != NULL)
        file_put_page(itr->file, itr->pageid, itr->page);

    memset(itr, 0, sizeof(file_itr_t));
    free(itr);
}

const char* sortlib_info()
{
    static char buf[256];
    char* pbuf = buf;
    
    pbuf += sprintf(pbuf, "OS=%s ",
#if defined(WIN32)
                    "Windows"
#else
                    "POSIX-compatible"
#endif /* defined(WIN32) */
        );
#if defined(USE_SYSTEM_PAGE_SIZE)
    pbuf += sprintf(pbuf, "Page size=(system default) ");
#else
    pbuf += sprintf(pbuf, "Page size=%uKB ", (1 << (PAGE_SIZE_ORDER - 10)));
#endif /* defined(USE_SYSTEM_PAGE_SIZE) */
#if defined(HAVE_QSORT_R)
    pbuf += sprintf(pbuf, "QSort func=qsort_r");
#elif defined(HAVE_QSORT_S)
    pbuf += sprintf(pbuf, "QSort func=qsort_s");
#else
    pbuf += sprintf(pbuf, "QSort func=qsort");
#endif /* defined(HAVE_QSORT_R) */

    return buf;
}

#if defined(EXE_DUMP_FILE)
int main(int argc, char** argv)
{
    int result;
    file_t* file;
    file_itr_t* itr;
    char* data;
    u32 len;
    
    if (argc < 2) {
        fprintf(stderr, "too few parameters\n");
        return 1;
    }
    
    if ((file = file_open(argv[1], 0)) == NULL) {
        fprintf(stderr, "Failed to open %s\n", argv[1]);
        return 1;
    }
    
    if ((itr = file_itr_init(file)) == NULL) {
        fprintf(stderr, "file_itr_init failed");
        goto done;
    }

    while ((result = file_itr_next(itr, &data, &len)) == 0)
        printf("%s\n", key);

    if (result < 0) {
        fprintf(stderr, "file_itr_next failed\n");
        result = 1;
    }

    file_itr_finish(itr);

done:
    file_close(file);
    return result;
}
#endif /* defined(EXE_DUMP_FILE) */

#if defined(EXE_TEST_SORT)
extern int file_sort(file_t* file);
int main()
{
    file_t* file;
    FILE* infile;
    char buf[256];
    int result = 1;
    file_itr_t* itr;
    char* data;
    u32 len;
    uint line;
    
    if ((file = file_open("sorted.file", 1)) == NULL) {
        fprintf(stderr, "Failed to open sorted.file\n");
        return 1;
    }
    
    infile = fopen("unsorted.txt", "r");
    if (infile == NULL) {
        fprintf(stderr, "Failed to open unsorted.txt\n");
        file_close(file);
        return 1;
    }

    line = 1;
    while (fgets(buf, 256, infile) != NULL) {
        len = strlen(buf) - 1;
        buf[len] = 0;
        if (file_insert(file, buf, len + 1) < 0) {
            fprintf(stderr, "file_insert failed on key %s (line %d)\n", buf, 
                    line);
            goto done;
        }
        line++;
    }
    fprintf(stderr, "Added %d records\n", line);
    
    if (file_sort(file) < 0) {
        fprintf(stderr, "failed to sort file\n");
        goto done;
    }
    
    file_close(file);
    fclose(infile);
    
    if ((file = file_open("sorted.file", 0)) == NULL) {
        fprintf(stderr, "Failed to open sorted.file\n");
        return 1;
    }
    
    if ((itr = file_itr_init(file)) == NULL) {
        fprintf(stderr, "file_itr_init failed");
        goto done;
    }

    while ((result = file_itr_next(itr, &data, &len)) == 0)
        printf("%s\n", data);

    if (result < 0) {
        fprintf(stderr, "file_itr_next failed\n");
        result = 1;
    }

    file_itr_finish(itr);
    
done:
    fclose(infile);
    file_close(file);
    return result;
}
#endif // defined(EXE_TEST_SORT)
