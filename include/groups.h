/* Needs board_coord.h to be included. */
struct Group {
    struct BoardCoord** group;
    size_t amount;
    size_t size;
};

typedef struct Group Group;

void init_group(Group* group, size_t initial_size);
void insert_group(Group* group, struct BoardCoord* element);
bool group_contains_board_coord(const Group* group, const struct BoardCoord* board_coord);
bool group_contains_indices(const Group* group, u_char x_index, u_char y_index);
void free_group(Group* group);
