#include <stdlib.h>

#include "types.h"       /* Needed by board_coord.h. */
#include "board_coord.h" /* Needed by groups.h. */
#include "dbg.h"
#include "groups.h"

void
init_group(Group* group, size_t initial_size)
{
    group->group = malloc(initial_size * sizeof(size_t));
    if (group->group == NULL) {
        log_err("Failed to allocate for Group");
        return;
    }
    group->amount = 0;
    group->size = initial_size;
}

void
insert_group(Group* group, struct BoardCoord* element)
{
    if (group->amount == group->size) {
        group->size *= 2;
        struct BoardCoord** group_ptr = group->group;
        group->group = realloc(group->group, group->size * sizeof(size_t));
        if (group->group == NULL) {
            log_err("Failed to reallocate for Group");
            group->size /= 2;
            group->group = group_ptr;
            return;
        }
    }
    group->group[group->amount] = element;
    group->amount++;
}

bool
group_contains_board_coord(const Group* group, const struct BoardCoord* board_coord)
{
    for (size_t i = 0; i < group->amount; i++) {
        if (group->group[i]->x_index == board_coord->x_index && group->group[i]->y_index == board_coord->y_index) {
            return true;
        }
    }
    return false;
}

bool
group_contains_indices(const Group* group, u_char x_index, u_char y_index)
{
    for (size_t i = 0; i < group->amount; i++) {
        if (group->group[i]->x_index == x_index && group->group[i]->y_index == y_index) {
            return true;
        }
    }
    return false;
}

void
free_group(Group* group)
{
    for (size_t i = 0; i < group->amount; i++) {
        free(group->group[i]);
    }
    free(group->group);
    group->group = NULL;
    group->amount = group->size = 0;
}
