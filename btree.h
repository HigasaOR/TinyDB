#include <stdint.h>

#include "my_settings.h"

typedef enum { NODE_INTERNAL, NODE_LEAF } NodeType;

/* define node header constants */
/*
 *  | TYPE | IS_ROOT | PARANT_POINTER |  other data
 */
const uint32_t NODE_TYPE_SIZE =
    sizeof(uint8_t);  // uint8_t: using char suffices for node type
const uint32_t NODE_TYPE_OFFSET = 0;
const uint32_t IS_ROOT_SIZE = sizeof(uint8_t);
const uint32_t IS_ROOT_OFFSET = NODE_TYPE_SIZE;
const uint32_t PARENT_POINTER_SIZE = sizeof(uint32_t);
const uint32_t PARENT_POINTER_OFFSET = IS_ROOT_OFFSET + IS_ROOT_SIZE;
const uint8_t COMMON_NODE_HEADER_SIZE =
    NODE_TYPE_SIZE + IS_ROOT_SIZE + PARENT_POINTER_SIZE;

/* leaf node header constants */
/*
 *  ... | PARANT_POINTER | LEAF_NODE_NUM_CELLS | data...
 */
const uint32_t LEAF_NODE_NUM_CELLS_SIZE = sizeof(uint32_t);
const uint32_t LEAF_NODE_NUM_CELLS_OFFSET = COMMON_NODE_HEADER_SIZE;
const uint32_t LEAF_NODE_HEADER_SIZE =
    COMMON_NODE_HEADER_SIZE + LEAF_NODE_NUM_CELLS_SIZE;

const uint32_t LEAF_NODE_KEY_SIZE = sizeof(uint32_t);
const uint32_t LEAF_NODE_KEY_OFFSET = 0;
const uint32_t LEAF_NODE_VALUE_SIZE = ROW_SIZE;
const uint32_t LEAF_NODE_VALUE_OFFSET =
    LEAF_NODE_KEY_OFFSET + LEAF_NODE_KEY_SIZE;
const uint32_t LEAF_NODE_CELL_SIZE = LEAF_NODE_KEY_SIZE + LEAF_NODE_VALUE_SIZE;
const uint32_t LEAF_NODE_SPACE_FOR_CELLS =
    PAGE_SIZE - LEAF_NODE_HEADER_SIZE;  // each node corresponds to one page
// how many cell(key + record)s can be stored in a leaf node
const uint32_t LEAF_NODE_MAX_CELLS =
    LEAF_NODE_SPACE_FOR_CELLS / LEAF_NODE_CELL_SIZE;

/* returns pointer to the "NUM_CELLS" cell of the leaf node */
uint32_t* leaf_node_num_cells(void* node) {
    return node + LEAF_NODE_NUM_CELLS_OFFSET;
}

/* returns pointer to the offset of the cell given the node and the cell number
 * (in that node) */
void* leaf_node_cell(void* node, uint32_t cell_num) {
    return node + LEAF_NODE_HEADER_SIZE + cell_num * LEAF_NODE_CELL_SIZE;
}

/* the key offset of the cell, where the key stores the offset of the cell */
uint32_t* leaf_node_key(void* node, uint32_t cell_num) {
    return leaf_node_cell(node, cell_num);
}

/* the value offset of the cell (which is the record) */
void* leaf_node_value(void* node, uint32_t cell_num) {
    return leaf_node_cell(node, cell_num) + LEAF_NODE_KEY_SIZE;
}

/* initializes the leaf node, setting num cells to 0 */
void initialize_leaf_node(void* node) { *leaf_node_num_cells(node) = 0; }
