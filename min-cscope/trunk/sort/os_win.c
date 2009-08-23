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
#include "internal.h"

static void my_perror(const char* msg)
{
    DWORD error;
    DWORD result;
    DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM;
    LPTSTR buf = NULL;

    error = GetLastError();
    result = FormatMessage(flags, NULL, error, 0, (LPTSTR)&buf, 64, NULL);
    if (result != 0)
        fprintf(stderr, "%s: %s", msg, (LPTSTR)buf);
    else
        fprintf(stderr, "%s: error %lu\n", msg, error);

    if (buf != NULL)
        LocalFree(buf);
}

/**
 * @return 0 if the file was opened, 1 if a new file was created, -1 on error
 */ 
int os_open_file(const char* path, file_handle_t* handlep)
{
    file_handle_t handle;
    int result = 0;
    u32 size;

    handle = (file_handle_t)LocalAlloc(LMEM_FIXED, sizeof(*handle));
    if (handle == NULL) {
        my_perror("LocalAlloc");
        return -1;
    }
    
    /* Open the file (create a new file if required) */
    handle->hfile = CreateFile(path, GENERIC_READ | GENERIC_WRITE, 0, NULL,
                               OPEN_ALWAYS,
                               FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,
                               NULL);
    if (handle->hfile == INVALID_HANDLE_VALUE) {
        my_perror("CreateFile");
        goto error;
    }

    /* Check if this is a new file. */
    if (GetLastError() != ERROR_ALREADY_EXISTS)
        result = 1;

    /* Get the file size. */
    if (os_get_file_size(handle, &size) < 0)
        goto error;
    
    /* Create a mapping object. */
    if (size != 0) {
        handle->hmap = CreateFileMapping(handle->hfile, NULL, PAGE_READWRITE, 0,
                                         size, NULL);
        if (handle->hmap == NULL) {
            my_perror("CreateFileMapping");
            goto error;
        }
    }
    else {
        handle->hmap = NULL;
    }
    
    DPRINT("os_open_file: returning %d\n", result);
    *handlep = handle;
    return result;

error:
    if (handle->hfile != INVALID_HANDLE_VALUE) {
        if (handle->hmap != NULL)
            CloseHandle(handle->hmap);
        CloseHandle(handle->hfile);
    }

    return -1;
}

extern WINBASEAPI BOOL WINAPI GetFileSizeEx(HANDLE,PLARGE_INTEGER);

int os_get_file_size(file_handle_t handle, u32* sizep)
{
    LARGE_INTEGER size;
    
    if (!GetFileSizeEx(handle->hfile, &size)) {
        my_perror("GetFileSizeEx");
        return -1;
    }

    if (size.HighPart > 1) {
        fprintf(stderr, "os_get_file_size: file is too big (over 4GB)\n");
        return -1;
    }
    
    *sizep = size.LowPart;
    return 0;
}

int os_set_file_size(file_handle_t handle, u32 newsize, u32* sizep)
{
    LARGE_INTEGER newsize64;
    u32 size;
    
    /* Move the file pointer to the requested location. */
    newsize64.LowPart = newsize;
    newsize64.HighPart = 0;
    if (SetFilePointerEx(handle->hfile, newsize64, NULL, FILE_BEGIN)
        == INVALID_SET_FILE_POINTER) {
        my_perror("SetFilePointer");
        return -1;
    }

    /* Must close the mapping object first. */
    if (handle->hmap != NULL) {
        if (!CloseHandle(handle->hmap)) {
            my_perror("CloseHandle (handle->hmap)");
            return -1;
        }
    }

    /* Chnge the file size. */
    if (!SetEndOfFile(handle->hfile)) {
        my_perror("SetEndOfFile");
        return -1;
    }

    /* Get the new file size. */
    if (os_get_file_size(handle, &size) < 0)
        return -1;
    
    /* Recreate the file mapping object. */
    handle->hmap = CreateFileMapping(handle->hfile, NULL, PAGE_READWRITE, 0,
                                     size, NULL);
    if (handle->hmap == NULL) {
        my_perror("CreateFileMapping");
        return -1;
    }

    *sizep = size;
    return 0;
}

void os_close_file(file_handle_t handle)
{
    /* Close all handles. */
    CloseHandle(handle->hmap);
    CloseHandle(handle->hfile);

    /* Free the file object. */
    LocalFree(handle);
}

int os_delete_file(const char* path)
{
    if (!DeleteFile(path)) {
        if (GetLastError() != ERROR_FILE_NOT_FOUND) {
            my_perror("DeleteFile");
            fprintf(stderr, "while deleting %s\n", path);
            return -1;
        }
    }

    return 0;
}

void* os_get_page(file_handle_t handle, page_id_t pageid)
{
    void* result;

    /* Map the page using the file mapping object. */
    result = MapViewOfFile(handle->hmap, FILE_MAP_READ | FILE_MAP_WRITE, 0,
                           PAGE_OFFSET(pageid), PAGE_SIZE);
    if (result == NULL) {
        my_perror("MapViewOfFile");
        fprintf(stderr, "while mapping page %u with size %u\n", pageid,
                PAGE_SIZE);
        return NULL;
    }
    
    DPRINT("os_get_page: mapped page %u (%p)\n", pageid, result);
    return result;
}

void* os_get_anon_page()
{
    static HANDLE hmap = NULL;
    void* result;
    
    if (hmap == NULL) {
        /* Create a mapping object backed by the systemm paging file. */
        hmap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0,
                                 PAGE_SIZE, NULL);
        if (hmap == NULL) {
            my_perror("CreateFileMapping (Anon)");
            return NULL;
        }
    }
    
    result = MapViewOfFile(hmap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0,
                           PAGE_SIZE);
    if (result == NULL)
        my_perror("MapViewOfFile (anon)");
    
    return result;
}

int os_put_page(file_handle_t handle, void* ptr)
{
    (void)handle;
    if (UnmapViewOfFile(ptr) == 0) {
        my_perror("UnmapViewOfFile");
        return -1;
    }

    return 0;
}

int os_put_anon_page(void* ptr)
{
    if (UnmapViewOfFile(ptr) == 0) {
        my_perror("UnmapViewOfFile (anon)");
        return -1;
    }

    return 0;
}

u32 os_get_page_size()
{
    SYSTEM_INFO info;

    GetSystemInfo(&info);
    return info.dwAllocationGranularity;
}
