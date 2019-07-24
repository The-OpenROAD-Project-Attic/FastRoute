////////////////////////////////////////////////////////////////////////////////
// BSD 3-Clause License
//
// Copyright (c) 2018, Iowa State University All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holder nor the names of its contributors
// may be used to endorse or promote products derived from this software
// without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
// USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
////////////////////////////////////////////////////////////////////////////////

#include "dl.h"
#include <assert.h>
#include <stdio.h>
namespace FastRoute {
dl_t dl_alloc() {
        dl_t dl = (dl_t)malloc(sizeof(dl_s));
        if (!dl) {
                printf("Out of memory!!\n");
        } else {
                dl->first = dl->last = 0;
                dl->count = 0;
        }
        return dl;
}

void dl_delete(dl_t dl, dl_el *el) {
        if (dl->first == el) {
                dl->first = el->next;
        }
        if (dl->last == el) {
                dl->last = el->prev;
        }
        if (el->next) {
                el->next->prev = el->prev;
        }
        if (el->prev) {
                el->prev->next = el->next;
        }
        free(el);
        dl->count--;
}

void dl_clear(dl_t dl) {
        dl_el *el, *next;
        if (dl->count > 0) {
                for (el = dl->first; el; el = next) {
                        next = el->next;
                        free(el);
                }
        }
        dl->first = dl->last = 0;
        dl->count = 0;
}

void dl_concat(dl_t first_list, dl_t second_list) {
        if (first_list->count <= 0) {
                *first_list = *second_list;
        } else if (second_list->count > 0) {
                first_list->last->next = second_list->first;
                second_list->first->prev = first_list->last;
                first_list->last = second_list->last;
                first_list->count += second_list->count;
        }

        free(second_list);
}

static void dl_insertion_sort(dl_t dl, size_t el_size,
                              int (*compar)(void *, void *)) {
        char *buf;
        void *curr_d, *srch_d;
        dl_el *curr, *srch;

        if (dl_length(dl) <= 1) {
                return;
        }

        buf = (char *)malloc(el_size);

        for (curr = dl->first; curr != dl->last; curr = curr->next) {
                curr_d = (void *)(((dl_el *)curr) + 1);

                for (srch = dl->last; srch != curr; srch = srch->prev) {
                        srch_d = (void *)(((dl_el *)srch) + 1);
                        if (compar(curr_d, srch_d) > 0) {
                                memcpy((void *)buf, curr_d, el_size);
                                memcpy(curr_d, srch_d, el_size);
                                memcpy(srch_d, (void *)buf, el_size);
                        }
                }
        }

        free(buf);
}

void dl_sort(dl_t dl, size_t el_size, int (*compar)(void *, void *)) {
        dl_el *el, *first_head, *second_head;
        dl_s first_list, second_list;
        void *first_item, *second_item;
        int i, len;

        if (dl_length(dl) <= 25) {
                dl_insertion_sort(dl, el_size, compar);
                return;
        }

        len = dl_length(dl) / 2;
        for (i = 0, el = dl->first; i < len; i++) {
                el = el->next;
        }

        first_list.first = dl->first;
        first_list.last = el->prev;
        first_list.count = len;
        /* TODO:  <19-07-19, dereference to null pointer? > */
        /* first_list.last->next = 0; */

        second_list.first = el;
        second_list.last = dl->last;
        second_list.count = dl_length(dl) - len;
        second_list.first->prev = 0;

        dl_sort(&first_list, el_size, compar);
        dl_sort(&second_list, el_size, compar);

        /* in-place merging */
        first_head = first_list.first;
        second_head = second_list.first;

        first_item = (void *)(((dl_el *)first_head) + 1);
        second_item = (void *)(((dl_el *)second_head) + 1);
        if (compar(first_item, second_item) <= 0) {
                dl->first = el = first_head;
                first_head = first_head->next;
        } else {
                dl->first = el = second_head;
                second_head = second_head->next;
        }

        while (1) {
                first_item = (void *)(((dl_el *)first_head) + 1);
                second_item = (void *)(((dl_el *)second_head) + 1);
                if (compar(first_item, second_item) <= 0) {
                        el->next = first_head;
                        first_head->prev = el;
                        el = first_head;
                        first_head = first_head->next;
                        if (!first_head) {
                                el->next = second_head;
                                second_head->prev = el;
                                dl->last = second_list.last;
                                break;
                        }
                } else {
                        el->next = second_head;
                        second_head->prev = el;
                        el = second_head;
                        second_head = second_head->next;
                        if (!second_head) {
                                el->next = first_head;
                                first_head->prev = el;
                                dl->last = first_list.last;
                                break;
                        }
                }
        }
}
}  // namespace FastRoute
