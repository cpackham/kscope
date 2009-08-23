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

#ifndef __SORTLIB_H__
#define __SORTLIB_H__

typedef unsigned int u32;
typedef void* file_t;
typedef void* file_itr_t;
typedef void (*progress_func_t)(unsigned int, unsigned int);

file_t file_open(const char* path, int forcenew);
int file_insert(file_t* file, const char* data, u32 len);
int file_sort(file_t file, progress_func_t func);
void file_close(file_t btree);

file_itr_t file_itr_init(file_t file);
int file_itr_next(file_itr_t itr, char** datap, u32* lenp);
void file_itr_finish(file_itr_t itr);

void file_dump_info(file_t* file);

int file_delete(const char* path);

const char* sortlib_info();

#endif /* __SORTLIB_H__ */
