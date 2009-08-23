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

#ifndef __INTERNAL_H__
#define __INTERNAL_H__

#include "config.h"

/* Basic integral types. */ 
typedef unsigned int uint;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

#if defined(PAGE_SIZE_ORDER)
#define PAGE_SIZE (1 << PAGE_SIZE_ORDER)
#define PAGE_OFFSET(page) (page << PAGE_SIZE_ORDER)
/*
 * Page size rules:
 * 4K-64K - 2 byte record ID/offset, 256 byte maximum record size
 * 128K-64M - 4 byte record ID/offset, 1024 byte maximum record size
 * <4K or >64M - Not supported
 */
#if PAGE_SIZE_ORDER > 26
#error Unsupported page size (too big)
#elif PAGE_SIZE_ORDER > 16
typedef u32 rec_id_t;
#define MAX_RECORD_SIZE 1024
#elif PAGE_SIZE_ORDER >= 12
typedef u16 rec_id_t;
#define MAX_RECORD_SIZE 256
#else
#error Unsupported page size (too small)
#endif // PAGE_SIZE_ORDER >= 16
#else
#warning PAGE_SIZE_ORDER not defined, using system defaults (less efficient)
#define USE_SYSTEM_PAGE_SIZE
#define PAGE_SIZE system_page_size
#define PAGE_SIZE_ORDER system_page_size_order
#define PAGE_OFFSET(page) (page << system_page_size_order)
typedef u32 rec_id_t;
#define MAX_RECORD_SIZE 256
#endif // defined(PAGE_SIZE_ORDER)

/*
 * Each record takes at least 8 bytes in a page:
 * 4 bytes data
 * 1 byte key length
 * 1 byte (at least) key
 * 2 bytes (at least) header
 * The page header accounts for at least one extra record 
 */
#define MAX_PAGE_RECORDS ((PAGE_SIZE >> 3) - 1)

typedef rec_id_t rec_off_t;
typedef u32 page_id_t;

extern u32 system_page_size;
#if defined(USE_SYSTEM_PAGE_SIZE)
extern u32 system_page_size_order;
#endif // defined(USE_SYSTEM_PAGE_SIZE)

#if !defined(NDEBUG)
#include <assert.h>
#define DPRINT(...) do { fprintf(stderr, __VA_ARGS__); fflush(stderr); } while (0)
#define DASSERT(x)  assert(x)
#else
#define DPRINT(...) do {} while (0)
#define DASSERT(x)
#endif /* !defined(NDEBUG) */

#define CACHE_SIZE_ORDER 6
#define CACHE_SIZE       (1 << CACHE_SIZE_ORDER)

typedef struct
{
    page_id_t pageid;
    void*     ptr;
    u32       ref;
} cache_entry_t;

typedef struct
{
    cache_entry_t entries[CACHE_SIZE];
    u32           hits;
    u32           misses;
} cache_t;

#if defined(WIN32)
#include <windows.h>
typedef struct
{
    HANDLE  hfile;
    HANDLE  hmap;
}* file_handle_t;
#define INVALID_FILE_HANDLE NULL
#else
typedef int file_handle_t;
#define INVALID_FILE_HANDLE -1
#endif /* defined(WIN32) */

typedef struct
{
    page_id_t next;
    rec_id_t  nrecs;
    rec_off_t nextoff;
    rec_off_t freebytes;
} page_hdr_t;

typedef struct
{
    u8    len;
    char* data;
} record_t;

typedef struct
{
    u32 cookie;
    /** First used page. */
    page_id_t head;
    /** Last used page. */
    page_id_t tail;
    /** First unused page. */
    page_id_t free;
    /** Total number of used pages in the file. */
    page_id_t npages;
    /** Total number of records in the B-Tree. */
    u32 nrecs;
} file_hdr_t;

typedef struct
{
    u32 mapped;
    u32 unmapped;
    u32 ncmapped;
} stats_t;

typedef struct
{
    file_handle_t handle;
    file_hdr_t*   header;
    u32           size;
    cache_t       cache;
    stats_t       stats;
} file_t;

typedef void (*progress_func_t)(unsigned int, unsigned int);

int os_open_file(const char* path, file_handle_t* handlep);
int os_get_file_size(file_handle_t handle, u32* sizep);
int os_set_file_size(file_handle_t handle, u32 newsize, u32* sizep);
void os_close_file(file_handle_t handle);
int os_delete_file(const char* path);
void* os_get_page(file_handle_t handle, page_id_t pageid);
int os_put_page(file_handle_t handle, void* ptr);
void* os_get_anon_page();
int os_put_anon_page(void*);
u32 os_get_page_size();

int page_add_record(page_hdr_t* page, record_t* rec);
void page_sort(page_hdr_t* page);
int page_get_record(page_hdr_t* page, rec_id_t id, record_t* rec);
int page_find(page_hdr_t* page, const char* key, u32 keylen, u32* data);
void page_dump(file_handle_t handle, page_id_t pageid);
void page_dump_header(page_hdr_t* page);
void page_dump_records(page_hdr_t* page, rec_id_t nrecs);

void* cache_get_page(cache_t* cache, page_id_t pageid);
int cache_dec_ref(cache_t* cache, page_id_t pageid);
int cache_replace(cache_t* cache, page_id_t pageid, void* ptr, page_id_t* oldid,
                  void** oldptr);
int cache_remove(cache_t* cache, page_id_t pageid);
void cache_invalidate(cache_t* cache, int reset);

page_hdr_t* file_get_page(file_t* file, page_id_t pageid);
page_hdr_t* file_get_new_page(file_t* file, page_id_t* pageidp);
void file_put_page(file_t* file, page_id_t pageid, page_hdr_t* page);
void file_free_page(file_t* file, page_id_t pageid, page_hdr_t* page);

static inline rec_off_t* get_rec_header(page_hdr_t* page, rec_id_t id)
{
    return (rec_off_t*)((char*)page + PAGE_SIZE) - id - 1;
}

#endif /* __INTERNAL_H__ */
