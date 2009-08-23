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
#include <stdlib.h>
#include <string.h>
#include "internal.h"

/**
 * @return 0 if successful, 1 if the page is full, -1 if another error has
 * occurred.
 */
int page_add_record(page_hdr_t* page, record_t* rec)
{
    u32 reclen;
    char* ptr;
    rec_id_t id;
    
    /*
     * Compute the total record size:
     * 1-byte length + length of data. 
     */
    reclen = rec->len + 1;
#if defined(ALIGN_RECS)
    reclen = ((reclen + ALIGN_RECS - 1) & ~(ALIGN_RECS - 1));
#endif

    /*
     * Check that the page has enough free space to contain the new record and
     * its header.
     */
    if ((reclen + sizeof(rec_off_t)) > page->freebytes)
        return 1;

    /* Copy the record into the page. */
    ptr = (char*)page + page->nextoff;
    *ptr = rec->len;
    memcpy(ptr + 1, rec->data, rec->len);

    /* Set the record header. */
    id = page->nrecs;
    *(get_rec_header(page, id)) = page->nextoff;
    
    /* Update the page header. */
    page->nrecs++;
    page->nextoff += reclen;
    page->freebytes -= (reclen + sizeof(rec_off_t));

    return 0;
}

static int comp_recs_r(const void* offp1, const void* offp2, void* page)
{
    rec_off_t off1;
    rec_off_t off2;
    char* data1;
    char* data2;
    u32 len1;
    u32 len2;
    u32 len;
    int shorter;
    int cmp;
    
    /* Get the offsets of the two records. */
    off1 = *(rec_off_t*)offp1;
    off2 = *(rec_off_t*)offp2;
    
    /* Get the records and their lengths. */
    len1 = (u8)*((char*)page + off1);
    data1 = (char*)page + off1 + 1;
    len2 = (u8)*((char*)page + off2);
    data2 = (char*)page + off2 + 1;

    /* Determine the shorter key for memcmp. */
    if (len1 < len2) {
        len = len1;
        shorter = 1;
    }
    else {
        len = len2;
        shorter = -1;
    }
    
    /*
     * Note: comparison is reversed, since the record header area starts at the
     * end of the page and grows down.
     */
    cmp = memcmp(data2, data1, len);
    if (cmp == 0)
        return shorter;

    return cmp;
}

static page_hdr_t* comppage;
static int comp_recs(const void* offp1, const void* offp2)
{
    return comp_recs_r(offp1, offp2, comppage);
}

void page_sort(page_hdr_t* page)
{
    void* base;

    /* Run quicksort. */
    base = get_rec_header(page, page->nrecs - 1);
#if defined(HAVE_QSORT_R)
    qsort_r(base, page->nrecs, sizeof(rec_off_t), comp_recs_r, page);
#elif defined(HAVE_QSORT_S)
    qsort_s(base, page->nrecs, sizeof(rec_off_t), comp_recs_r, page);
#else
    comppage = page;
    qsort(base, page->nrecs, sizeof(rec_off_t), comp_recs);
#endif
}

int page_get_record(page_hdr_t* page, rec_id_t id, record_t* rec)
{
    char* ptr;
    rec_off_t off;

    /* Make sure the record exists. */
    if (id >= page->nrecs)
        return -1;

    /* Get the offset from the record header. */
    off = *get_rec_header(page, id);
    ptr = (char*)page + off;

    rec->len = (u8)*ptr;
    rec->data = (ptr + 1);
    return 0;
}

void page_dump(file_handle_t handle, page_id_t pageid)
{
    page_hdr_t* page;

    page = os_get_page(handle, pageid);
    if (page == NULL)
        return;
    
    fprintf(stderr, "Page ID: %u\n", pageid);
    page_dump_header(page);
	
    os_put_page(handle, page);
}

void page_dump_header(page_hdr_t* page)
{
    fprintf(stderr, "Header:\n");
    fprintf(stderr, "  Next=%u\n", page->next);
    fprintf(stderr, "  #Records=%u\n", page->nrecs);
    fprintf(stderr, "  Offset=%u\n", page->nextoff);
    fprintf(stderr, "  Free=%u\n", page->freebytes);
}

void page_dump_records(page_hdr_t* page, rec_id_t nrecs)
{
    rec_id_t i;
    record_t rec;
    
    if (nrecs > page->nrecs)
        nrecs = page->nrecs;

    fprintf(stderr, "Records:\n");
    for (i = 0; i < nrecs; i++) {
        if (page_get_record(page, i, &rec) == 0) {
            fprintf(stderr, "  %u data=", i);
            fwrite(rec.data, rec.len, 1, stderr);
            fprintf(stderr, "\n");
        }
    }
}
