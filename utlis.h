#ifndef _UTILS_
#define _UTILS_

bool isPositiveNumber(const char* str);

bool containsSpaces(char* str);

char *form_json_logging_dates(char *username, char *password);

char *form_json_book(char *title, char *author, char *genre, char *publisher,
                         int page_count_int);

void printBooks(char *response);

void printBook(char *response);

void read_input(const char *prompt, char *buffer);

#endif
