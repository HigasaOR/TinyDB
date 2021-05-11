#ifndef MY_UTILS_H_
#define MY_UTILS_H_

#include <stdint.h>
#include <stdlib.h>

#include "my_settings.h"

/* from row structure to memory block */
void serialize_row(Row *source, void *destination) {
    memcpy(destination + ID_OFFSET, &(source->id), ID_SIZE);
    memcpy(destination + USERNAME_OFFSET, &(source->username), USERNAME_SIZE);
    memcpy(destination + EMAIL_OFFSET, &(source->email), EMAIL_SIZE);
}

/* from memory block to assigned row structure */
void deserialize_row(void *source, Row *destination) {
    memcpy(&(destination->id), source + ID_OFFSET, ID_SIZE);
    memcpy(&(destination->username), source + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(destination->email), source + EMAIL_OFFSET, EMAIL_SIZE);
}

/* get page from pager given page number */
void *get_page(Pager *pager, uint32_t page_num) {
    if (page_num > TABLE_MAX_PAGES) {
        printf("Tried to fetch page number out of bounds. %d > %d\n", page_num,
               TABLE_MAX_PAGES);
        exit(EXIT_FAILURE);
    }

    if (pager->pages[page_num] == NULL) {  // Cache miss
        // Allocate memory
        void *page = malloc(PAGE_SIZE);

        // if page is in the file range, load from file
        uint32_t num_pages = pager->file_length / PAGE_SIZE;
        if (page_num <= num_pages) {
            lseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);
            ssize_t bytes_read = read(pager->file_descriptor, page, PAGE_SIZE);
            if (bytes_read == -1) {
                printf("Error reading file: %d\n", errno);
                exit(EXIT_FAILURE);
            }
        }

        pager->pages[page_num] = page;

        if (page_num >= pager->num_pages) {
            // update the max. number of pages of the pager
            pager->num_pages = page_num + 1;
        }
    }

    return pager->pages[page_num];
}

/* initializes and returns a pager associated to given file */
Pager *pager_open(const char *filename) {
    int fd = open(filename,
                  O_RDWR |       // Read/Write mode
                      O_CREAT,   // Create file if it does not exist
                  S_IWUSR |      // User write permission
                      S_IRUSR);  // User read permission

    if (fd == -1) {
        printf("Unable to open file\n");
        exit(EXIT_FAILURE);
    }

    off_t file_length = lseek(fd, 0, SEEK_END);

    Pager *pager = (Pager *)malloc(sizeof(Pager));
    pager->file_descriptor = fd;
    pager->file_length = file_length;
    pager->num_pages = file_length / PAGE_SIZE;

    if (file_length % PAGE_SIZE != 0) {
        printf("Db file is not a whole number of pages. Corrupt file.\n");
        exit(EXIT_FAILURE);
    }

    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        pager->pages[i] = NULL;
    }
    return pager;
}

/* flush pager memory content into file */
void pager_flush(Pager *pager, uint32_t page_num) {
    if (pager->pages[page_num] == NULL) {
        printf("Tried to flush null page\n");
        exit(EXIT_FAILURE);
    }

    // writing offset
    off_t offset =
        lseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);

    if (offset == -1) {
        printf("Error seeking: %d\n", errno);
        exit(EXIT_FAILURE);
    }

    ssize_t bytes_written =
        write(pager->file_descriptor, pager->pages[page_num], PAGE_SIZE);

    if (bytes_written == -1) {  // write failed
        printf("Error writing: %d\n", errno);
        exit(EXIT_FAILURE);
    }
}

/*
Until we start recycling free pages, new pages will always
go onto the end of the database file
*/
uint32_t get_unused_page_num(Pager *pager) { return pager->num_pages; }

#endif