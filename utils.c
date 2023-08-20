#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include <stdbool.h>

#include "helpers.h"
#include "requests.h"
#include "parson.h"
#include "utlis.h"

/**
 * @brief Checks if a string represents a positive number
*/
bool isPositiveNumber(const char* str) {
    // Check if the string is empty
    if (str == NULL || str[0] == '\0') {
        return false;
    }

    // Iterate through each character in the string
    for (int i = 0; str[i] != '\0'; i++) {
        // Check if the character is not a digit
        if (str[i] < '0' || str[i] > '9') {
            return false;
        }
    }

    // Convert the string to an integer
    int num = atoi(str);

    // Check if the number is positive
    if (num > 0) {
        return true;
    }

    return false;
}

/**
 * @brief Return true if the given string contains spaces
*/
bool containsSpaces(char* str) {
    while (*str != '\0') {
        if (*str == ' ') {
            return true;
        }
        str++;
    }
    return false;
}

/**
 * @brief Create a json string with the username and password
*/
char *form_json_logging_dates(char *username, char *password) {

    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);

    json_object_set_string(root_object, "username", username);
    json_object_set_string(root_object, "password", password);

    char *serialized_string = json_serialize_to_string_pretty(root_value);

    json_value_free(root_value);

    return serialized_string;
}

/**
 * @brief Create a json string with the book data
*/
char *form_json_book(char *title, char *author, char *genre, char *publisher,
                         int page_count_int) {
    
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);

    json_object_set_string(root_object, "title", title);
    json_object_set_string(root_object, "author", author);
    json_object_set_string(root_object, "genre", genre);
    json_object_set_string(root_object, "publisher", publisher);
    json_object_set_number(root_object, "page_count", page_count_int);

    char *serialized_string = json_serialize_to_string_pretty(root_value);

    return serialized_string;
}

/**
 * @brief Extracts the array of JSON objects from the response and prints it
 *     in a pretty format.
*/
void printBooks(char *response) {

    char *array_of_books 
        = strstr(response, "[{\"") != NULL ? strstr(response, "[{\"") : strstr(response, "[]");

    // verify if the size of the array is 0
    if (array_of_books[1] == ']') {
        printf("No books in library\n");
        return;
    }

    JSON_Value *root_value = json_parse_string(array_of_books);
    char *serialized_string = json_serialize_to_string_pretty(root_value);

    printf("All books from the library:\n");
    printf("%s\n", serialized_string);

    json_value_free(root_value);
}


/**
 * @brief Prints the details of a book. Extracts the JSON object from the response
 *       and prints it in a pretty format.
*/
void printBook(char *response) {

    char *book_details = basic_extract_json_response(response);
    JSON_Value *root_value = json_parse_string(book_details);
    char *serialized_string = json_serialize_to_string_pretty(root_value);
    printf("%s\n", serialized_string);

    json_value_free(root_value);
}

/**
 * @brief Print the given prompt and read what the user types on the keyboard
 *      until a newline is encountered and store the result in the given buffer.
*/
void read_input(const char *prompt, char *buffer) {
    printf("%s", prompt);
    fflush(stdout);  // Make sure the prompt is displayed immediately
    scanf(" %[^\n]", buffer);  // Read until a newline
}