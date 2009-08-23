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
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "internal.h"

int os_open_file(const char* path, file_handle_t* handlep)
{
    int result = 0;
    file_handle_t handle;
    
    /* Open the file (create a new file if required) */
    handle = open(path, O_RDWR);
    if (handle >= 0) {
        result = 0;
    }
    else if (errno == ENOENT) {
        handle = open(path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR );
        if (handle < 0) {
            perror("create");
            return -1;
        }
        result = 1;
    }
    else {
        perror("open");
        return -1;
    }

    *handlep = handle;
    
    return result;
}

int os_get_file_size(file_handle_t handle, u32* sizep)
{
    struct stat st;

    if (fstat(handle, &st) < 0) {
        perror("fstat");
        return -1;
    }

    *sizep = st.st_size;
    return 0;
}

int os_set_file_size(file_handle_t handle, u32 newsize, u32* sizep)
{
    if (ftruncate(handle, newsize) < 0) {
        perror("ftruncate");
        return -1;
    }
    
    return os_get_file_size(handle, sizep);
}

void os_close_file(file_handle_t handle)
{
    close(handle);
}

int os_delete_file(const char* path)
{
    if (unlink(path) < 0) {
        if (errno != ENOENT) {
            perror("unlink");
            return -1;
        }
    }

    return 0;
}

void* os_get_page(file_handle_t handle, page_id_t pageid)
{
    void* result;

    result = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, handle,
                  PAGE_OFFSET(pageid));
    if (result == MAP_FAILED) {
        perror("mmap");
        return NULL;
    }

    return result;
}

void* os_get_anon_page()
{
    void* result;

    result = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE,
                  MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (result == MAP_FAILED) {
        perror("mmap (anon)");
        return NULL;
    }

    return result;
}

int os_put_page(file_handle_t handle, void* ptr)
{
    (void)handle;
    if (munmap(ptr, PAGE_SIZE) < 0) {
        perror("munmap");
        return -1;
    }

    return 0;
}

int os_put_anon_page(void* ptr)
{
    if (munmap(ptr, PAGE_SIZE) < 0) {
        perror("munmap");
        return -1;
    }

    return 0;
}

u32 os_get_page_size()
{
    return (u32)sysconf(_SC_PAGESIZE);
}
