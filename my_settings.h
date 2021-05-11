#ifndef MY_SETTINGS_H_
#define MY_SETTINGS_H_

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct {
    char *buffer;
    size_t buffer_length;
    ssize_t input_length;
} InputBuffer;

/* define meta command parsing results */
typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

/* define sql command parsing results */
typedef enum {
    PREPARE_SUCCESS,
    PREPARE_STRING_TOO_LONG,
    PREPARE_SYNTAX_ERROR,
    PREPARE_UNRECOGNIZED_STATEMENT
} PrepareResult;

/* define sql statement types */
typedef enum { STATEMENT_INSERT, STATEMENT_SELECT } StatementType;

/* define executing results */
typedef enum {
    EXECUTE_SUCCESS,
    EXECUTE_DUPLICATE_KEY,
    EXECUTE_TABLE_FULL
} ExecuteResult;

/* define column sizes */
#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255

/* define row record structure*/
typedef struct {
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE];
    char email[COLUMN_EMAIL_SIZE];
} Row;

/* statement struct, storing type and content */
typedef struct {
    StatementType type;
    Row row_to_insert;
} Statement;

/* Input buffer */
InputBuffer *new_input_buffer() {
    InputBuffer *input_buffer = (InputBuffer *)malloc(sizeof(InputBuffer));
    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_length = 0;

    return input_buffer;
}

#define size_of_attribute(Struct, Attribute) sizeof(((Struct *)0)->Attribute)

/* define attribute sizes and offsets */
const uint32_t ID_SIZE = size_of_attribute(Row, id);
const uint32_t USERNAME_SIZE = size_of_attribute(Row, username);
const uint32_t EMAIL_SIZE = size_of_attribute(Row, email);
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

/* define paging constants*/
const uint32_t PAGE_SIZE = 4096;
#define TABLE_MAX_PAGES 100

/* pager structure */
typedef struct {
    int file_descriptor;
    uint32_t file_length;
    uint32_t num_pages;  // how many pages
    void *pages[TABLE_MAX_PAGES];
} Pager;

/* table structure */
typedef struct {
    Pager *pager;
    uint32_t root_page_num;
} Table;

typedef struct {
    Table *table;
    uint32_t page_num;
    uint32_t cell_num;
    bool end_of_table;  // if the cursor is at the end of table
} Cursor;

#endif
