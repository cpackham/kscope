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
#include "internal.h"

void* cache_get_page(cache_t* cache, page_id_t pageid)
{
    cache_entry_t* entry = &cache->entries[pageid & (CACHE_SIZE - 1)];

    if (entry->pageid == pageid) {
        cache->hits++;
        entry->ref++;
        return entry->ptr;
    }

    cache->misses++;
    return NULL;
}

int cache_replace(cache_t* cache, page_id_t pageid, void* ptr, page_id_t* oldid,
                  void** oldptr)
{
    cache_entry_t* entry = &cache->entries[pageid & (CACHE_SIZE - 1)];

    if (entry->ref > 0)
        return -1;

    *oldid = entry->pageid;
    *oldptr = entry->ptr;
    entry->pageid = pageid;
    entry->ptr = ptr;
    entry->ref = 1;
    
    return 0;
}

int cache_remove(cache_t* cache, page_id_t pageid)
{
    cache_entry_t* entry = &cache->entries[pageid & (CACHE_SIZE - 1)];

    if (entry->ref > 0)
        return -1;

    entry->pageid = 0;
    entry->ptr = NULL;
    entry->ref = 0;
    
    return 0;
}

int cache_dec_ref(cache_t* cache, page_id_t pageid)
{
    cache_entry_t* entry = &cache->entries[pageid & (CACHE_SIZE - 1)];

    if (entry->pageid != pageid)
        return -1;
    
    entry->ref--;
    return 0;
}

void cache_invalidate(cache_t* cache, int reset)
{
    if (reset)
        memset(cache, 0, sizeof(cache_t));
    else
        memset(cache->entries, 0, sizeof(cache_entry_t) << CACHE_SIZE_ORDER);
}
