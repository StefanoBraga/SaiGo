//
// Created by stefan on 9/05/25.
//

#ifndef GROUPS_H
#define GROUPS_H

#include "types.h"

struct Group {
    BoardCoord** group;
    size_t amount;
    size_t size;
};

typedef struct Group Group;

extern void init_group(Group* group, size_t initial_size);
extern void insert_group(Group* group, BoardCoord* element);
extern void free_group(Group* group);

#endif //GROUPS_H

