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
extern bool group_contains_board_coord(const Group* group, const BoardCoord* board_coord);
extern bool group_contains_indices(const Group* group, u_char x_index, u_char y_index);
extern void free_group(Group* group);

#endif //GROUPS_H

