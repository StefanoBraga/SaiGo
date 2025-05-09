//
// Created by stefan on 9/05/25.
//

#include <stdlib.h>
#include <stddef.h>
#include <dbg.h>
#include "groups.h"

void init_group(Group* group, size_t initial_size) {
    group->group = malloc(initial_size * sizeof(size_t));
    if (group->group == NULL) {
        log_err("Failed to allocate for Group");
        return;
    }
    group->amount = 0;
    group->size = initial_size;
}

void insert_group(Group* group, BoardCoord* element) {
    if (group->amount == group->size) {
        group->size *= 2;
        BoardCoord** group_ptr = group->group;
        group->group = realloc(group->group, group->size * sizeof(size_t));
        if (group->group == NULL) {
            log_err("Failed to reallocate for Group");
            group->size /= 2;
            group->group = group_ptr;
            return;
        }
    }
    group->group[group->amount++] = element;
}

void free_group(Group* group) {
    for (int i = 0; i < group->amount; i++) {
        free(group->group[i]);
    }
    free(group->group);
    group->group = nullptr;
    group->amount = group->size = 0;
}