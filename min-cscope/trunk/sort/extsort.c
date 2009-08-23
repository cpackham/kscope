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
#include "internal.h"

typedef struct
{
    page_id_t head;
    page_id_t size;
} run_info_t;

typedef struct
{
    file_t*     file;
    page_hdr_t* page;
    page_id_t   pageid;
    rec_id_t    recid;
    page_id_t   npages;
} run_itr_t;

static int comp_records(record_t* rec1, record_t* rec2)
{
    u32 len;
    int cmp;
    int shorter;

    if (rec1->len <= rec2->len) {
        len = rec1->len;
        shorter = -1;
    }
    else {
        len = rec2->len;
        shorter = 1;
    }

    cmp = memcmp(rec1->data, rec2->data, len);
    if (cmp == 0)
        return shorter;

    return cmp;
}

static int run_itr_init(file_t* file, run_info_t* info, run_itr_t* itr)
{
    /* Load the first page in the run. */
    itr->page = file_get_page(file, info->head);
    if (itr->page == NULL)
        return -1;

    /* Fill-in the iterator structure. */
    itr->file = file;
    itr->recid = 0;
    itr->pageid = info->head;
    itr->npages = info->size;

    return 0;
}

static int run_itr_next(run_itr_t* itr, record_t* rec)
{
    page_id_t pageid;
    
    /* Get a record from the current page. */
    if (itr->recid < itr->page->nrecs) {
        if (page_get_record(itr->page, itr->recid++, rec) < 0)
            return -1;

        return 1;
    }

    /* Done with this page, free it. */
    pageid = itr->page->next;
    file_free_page(itr->file, itr->pageid, itr->page);
    itr->npages--;
    if (itr->npages == 0) {
        itr->page = NULL;
        return 0;
    }

    /* Get the next page in the run. */
    itr->page = file_get_page(itr->file, pageid);
    if (itr->page == NULL)
        return -1;

    /* Get the first record from the page. */
    if (page_get_record(itr->page, 0, rec) < 0)
        return -1;

    itr->pageid = pageid;
    itr->recid = 1;
    return 1;
}

static void run_itr_finish(run_itr_t* itr)
{
    if (itr->page)
        file_put_page(itr->file, itr->pageid, itr->page);
}

static int merge_runs(file_t* file, run_info_t* run1, run_info_t* run2,
                      run_info_t* outrun)
{
    run_itr_t itr1;
    run_itr_t itr2;
    record_t rec1;
    record_t rec2;
    int needrec1 = 1;
    int needrec2 = 1;
    int haverec1 = 0;
    int haverec2 = 0;
    page_hdr_t* outpage = NULL;
    page_id_t outpageid;
    record_t* outrec;
    int result = -1;

    DPRINT("merge_runs: {%u,%u} with {%u,%u}\n", run1->head, run1->size,
           run2->head, run2->size);
    
    /* Initialize the run iterators. */
    itr1.page = NULL;
    itr2.page = NULL;
    if ((run_itr_init(file, run1, &itr1) < 0)
        || (run_itr_init(file, run2, &itr2) < 0)) {
        goto done;
    }
    
    /* Get an initial output page. */
    outpage = file_get_new_page(file, &outpageid);
    if (outpage == NULL)
        goto done;

    outrun->head = outpageid;
    outrun->size = 1;
    
    for (;;) {
        /* Get records as required. */
        if (needrec1) {
            haverec1 = run_itr_next(&itr1, &rec1);
            if (haverec1 < 0) {
                result = -1;
                goto done;
            }

            needrec1 = 0;
        }
        
        if (needrec2) {
            haverec2 = run_itr_next(&itr2, &rec2);
            if (haverec2 < 0) {
                result = -1;
                goto done;
            }

            needrec2 = 0;
        }

        /* Determine which record to output. */
        if (haverec1 && haverec2) {
            /* Compare the two records. */
            if (comp_records(&rec1, &rec2) < 0) {
                outrec = &rec1;
                needrec1 = 1;
            }
            else {
                outrec = &rec2;
                needrec2 = 2;
            }
        }
        else if (haverec1) {
            outrec = &rec1;
            needrec1 = 1;
        }
        else if (haverec2) {
            outrec = &rec2;
            needrec2 = 1;
        }
        else {
            result = 0;
            goto done;
        }

        /* Write the selected record to the output page. */
        result = page_add_record(outpage, outrec);
        if (result < 0)
            goto done;

        if (result > 0) {
            page_hdr_t* newpage;
            page_id_t newpageid;
            
            /* Output page is full, get a new one */
            newpage = file_get_new_page(file, &newpageid);
            if (newpage == NULL) {
                result = -1;
                goto done;
            }

            DPRINT("merge_runs: adding page to run %u->%u\n", outpageid, newpageid);
            outpage->next = newpageid;
            file_put_page(file, outpageid, outpage);
            outpage = newpage;
            outpageid = newpageid;
            outrun->size++;

            /* Write to the empty page (must not fail). */
            if (page_add_record(outpage, outrec) != 0) {
                result = -1;
                goto done;
            }
        }
    }

done:
    run_itr_finish(&itr1);
    run_itr_finish(&itr2);
    if (outpage)
        file_put_page(file, outpageid, outpage);

    DPRINT("merge_runs: finished with result=%d\n", result);
    return result;
}

static inline int ilog2(uint number)
{
    int i;

    for (i = -1; number > 0; number >>= 1, i++)
        ;

    return i;
}

int file_sort(file_t* file, progress_func_t progfunc)
{
    run_info_t* runqueue;
    uint itrnum;
    uint curitr;
    uint rqnum;
    uint rqtail;
    page_id_t runsize;
    page_hdr_t* page;
    page_id_t pageid;
    page_hdr_t* nextpage;
    page_id_t nextpageid;
    int result = -1;

    /* Do nothing for an empty file. */
    if (file->header->npages == 0)
        return 0;

    /* Estimate the number of iterations as ceil(log2(npages)). */
    itrnum = (uint)(ilog2(file->header->npages - 1) + 1);
    
    /*
     * Allocate the run queue with enough slots for half the number of pages in
     * the file (as the first iteration generates these many runs).
     */
    runqueue = (run_info_t*)malloc(sizeof(run_info_t)
                                   * ((file->header->npages + 1) << 1));
    rqnum = 0;
    
    /* First iteration: page-sort and merge for single-page runs. */
    DPRINT("file_sort: run size=1\n");
    pageid = file->header->head;
    while (pageid != 0) {
        run_info_t run1;
        run_info_t run2;
        
        /* Get and sort the first page of two consecutive ones. */
        page = file_get_page(file, pageid);
        if (page == NULL)
            goto done;
        
        page_sort(page);
        
        /* Check if the next page exists. */
        nextpageid = page->next;
        file_put_page(file, pageid, page);
        if (nextpageid == 0) {
            runqueue[rqnum].head = pageid;
            runqueue[rqnum].size = 1;
            rqnum++;
            break;
        }

        /* Get and sort the next page. */
        nextpage = file_get_page(file, nextpageid);
        if (nextpage == NULL)
            goto done;

        page_sort(nextpage);

        /* Prepare the run information. */
        run1.head = pageid;
        run1.size = 1;
        run2.head = nextpageid;
        run2.size = 1;
        
        /* Get the next page ID. */
        pageid = nextpage->next;
        file_put_page(file, nextpageid, nextpage);

        /* Merge the two pages. */
        if (merge_runs(file, &run1, &run2, &runqueue[rqnum]) < 0)
            goto done;

        rqnum++;
    }

    if (progfunc)
        progfunc(1, itrnum);
    
    /* Perform iterations until a single run is obtained. */
    runsize = 2;
    curitr = 2;
    while (runsize < file->header->npages) {
        DPRINT("file_sort: run size=%u run queue size=%u\n", runsize, rqnum);
        rqtail = 0;
        /* Merge runs. */
        while (rqnum > 1) {
            if (merge_runs(file, &runqueue[rqtail], &runqueue[rqtail + 1],
                           &runqueue[rqtail >> 1]) < 0) {
                goto done;
            }
            rqtail += 2;
            rqnum -= 2;
        }

        /* Check for a left-over run. */
        if (rqnum == 1)
            runqueue[rqtail >> 1] = runqueue[rqtail];

        rqnum = (rqtail >> 1) + rqnum;
        runsize <<= 1;

        if (progfunc)
            progfunc(curitr++, itrnum);
    }

    file->header->head = runqueue[0].head;
    result = 0;
    
done:
    free(runqueue);
    return result;
}
