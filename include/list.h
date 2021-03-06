//
// Copyright (c) 2013, Christian Speich
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#pragma once

#include <stddef.h>
#include <stdbool.h>

typedef struct list_entry list_entry_t;

struct list_entry {
	list_entry_t* next;
	list_entry_t* prev;
};

typedef struct {
	list_entry_t* head;
} list_t;


bool list_is_empty(const list_t* list);

list_entry_t* list_next(const list_t* list, list_entry_t* entry);

void list_insert_before(list_entry_t* entry, list_entry_t* new);

void list_delete(list_t* list, list_entry_t* entry);

void list_append(list_t* list, list_entry_t* entry);

void list_lrotate(list_t* list);

void list_rrotate(list_t* list);

list_entry_t* list_first(list_t* list);

list_entry_t* list_last(list_t* list);

void list_init(list_t* list);

void list_entry_init(list_entry_t* entry);

#define LIST_ENTRY_INIT {.next = NULL, .prev = NULL}

#define list_foreach(pos, list) \
	for (pos = (list)->head; \
		pos != NULL; \
		pos = list_next((list), pos))

#define list_foreach_contained(pos, list, type, member) \
	list_entry_t* entry = (list)->head; \
	for (pos = container_of(entry, type, member); \
		entry != NULL; \
		entry = list_next((list), entry), pos = container_of(entry, type, member))
		
