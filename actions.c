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
#include "actions.h"

#define MAX_LEN 100

// Server path defines
#define REGISTER "/api/v1/tema/auth/register"
#define LOGIN "/api/v1/tema/auth/login"
#define ACCESS_LIBRARY "/api/v1/tema/library/access"
#define BOOKS "/api/v1/tema/library/books"
#define LOGOUT "/api/v1/tema/auth/logout"

void register_action(char *username, char *password, int sockfd) {
    char *message;
    char *response;

    // create a string json with username and password
    char *register_data = form_json_logging_dates(username, password);

    // create the message
    message = compute_post_request("34.254.242.81:8080", REGISTER, "application/json",
                                             &register_data, 1, NULL, 0, NULL);

    // send the message
    send_to_server(sockfd, message);

    // receive the response
    response = receive_from_server(sockfd);

    // take the second word from the response
    char *copy_response = calloc(strlen(response) + 1, sizeof(char));
    strcpy(copy_response, response);
    char *code = strtok(copy_response, " ");
    code = strtok(NULL, " ");

    // if the second word begins with 2xx, the register was successful
    if (strncmp(code, "2", 1) == 0) {
        printf("200 - OK. Registration for the username %s was successful.\n",
                     username);
    } else {
        printf("Error: Username %s is already taken. Try another username.\n",
                     username);
    }

    free(copy_response);
}

void login_action(char *username, char *password, int sockfd, char **cookie,
                     bool *logged_in) {
    char * message;
    char *response;

    // create a string json with username and password
    char *register_data = form_json_logging_dates(username, password);

    // create the message
    message = compute_post_request("34.254.242.81:8080", LOGIN, "application/json",
                                            &register_data, 1, NULL, 0, NULL);

    // send the message
    send_to_server(sockfd, message);

    // receive the response
    response = receive_from_server(sockfd);
            
    char *copy_response = calloc(strlen(response) + 1, sizeof(char));
    strcpy(copy_response, response);
    char *code = strtok(copy_response, " ");
    code = strtok(NULL, " ");
                
    if (strncmp(code, "2", 1) == 0) {
        printf("200 - OK. Logging for the username %s was successful.\n", username);

        char *find_connect_sid = strstr(response, "connect.sid");
        *cookie = strtok(find_connect_sid, ": ;");
        *logged_in = true;

    } else {
        printf("Error: The account doesn't exists. You need to register first.\n");
    }

    free(copy_response);
}

char *enter_library_action(char *cookie, int sockfd) {

    char *message;
    char *response;

    // create the message
    message = compute_get_request("34.254.242.81:8080", ACCESS_LIBRARY,
                                     NULL, &cookie, 1, NULL);

    // send the message
    send_to_server(sockfd, message);

    // receive the response
    response = receive_from_server(sockfd);

    // extract the token
    char *begining_token = strstr(response, "{\"token\":\"");
    begining_token += strlen("{\"token\":\"");
    char *end_token = strstr(begining_token, "\"}");

    char *jwt_token_library = calloc(end_token - begining_token + 1, sizeof(char));
    strncpy(jwt_token_library, begining_token, end_token - begining_token);
    jwt_token_library[end_token - begining_token] = '\0';

    printf("You entered the library.\n");

    return jwt_token_library;
}

void get_books_action(char *cookie, char *jwt_token_library, int sockfd) {
    char *message;
    char *response;

    // create the message
    message = compute_get_request("34.254.242.81:8080", BOOKS, NULL,
                                     &cookie, 1, jwt_token_library);

    // send the message
    send_to_server(sockfd, message);

    // receive the response
    response = receive_from_server(sockfd);

    printBooks(response);
}

void get_book_action(char *cookie, char *jwt_token_library, int sockfd) {
    char *message;
    char *response;

    char *id = calloc(100, sizeof(char));
    printf("id=");
    scanf("%s", id);

    char *path_for_book = calloc(strlen(BOOKS) + strlen(id) + 2, sizeof(char));
    strcpy(path_for_book, BOOKS);
    strcat(path_for_book, "/");
    strcat(path_for_book, id);
            
    // create the message
    message = compute_get_request("34.254.242.81:8080", path_for_book, NULL,
                                             &cookie, 1, jwt_token_library);

    // send the message
    send_to_server(sockfd, message);

    // receive the response
    response = receive_from_server(sockfd);

    // take the second word from the response
    char *copy_response = calloc(strlen(response) + 1, sizeof(char));
    strcpy(copy_response, response);
    char *code = strtok(copy_response, " ");
    code = strtok(NULL, " ");

    // if the second word begins with 2xx, the book was found
    if (strncmp(code, "2", 1) == 0) {
        printf("Informations about the book with id %s:\n", id);
        printBook(response);
                
    } else {
        printf("Error: The book with id %s doesn't exists.\n", id);
    }

    free(copy_response);
    free(id);
    free(path_for_book);
}

void add_book_action(char *cookie, char *jwt_token_library, int sockfd) {

    char *message;
    char *response;

    char *title = calloc(MAX_LEN, sizeof(char));
    char *author = calloc(MAX_LEN, sizeof(char));
    char *genre = calloc(MAX_LEN, sizeof(char));
    char *publisher = calloc(MAX_LEN, sizeof(char));
    char *page_count = calloc(MAX_LEN, sizeof(char));

    read_input("title=", title);
    read_input("author=", author);
    read_input("genre=", genre);
    read_input("publisher=", publisher);
    read_input("page_count=", page_count);

    if (isPositiveNumber(page_count) == false) {
        printf("Error: The page_count must be a positive number.\n");
        free(title);
        free(author);
        free(genre);
        free(publisher);
        free(page_count);
        return;
    }

    int page_count_int = atoi(page_count);

    // create the json for the book
    char *book_data = form_json_book(title, author, genre, publisher, page_count_int);

    // create the message
    message = compute_post_request("34.254.242.81:8080", BOOKS, "application/json",
                                    &book_data, 1, &cookie, 1, jwt_token_library);

    // send the message
    send_to_server(sockfd, message);

    response = receive_from_server(sockfd);

    // take the second word from the response
    char *copy_response = calloc(strlen(response) + 1, sizeof(char));
    strcpy(copy_response, response);
    char *code = strtok(copy_response, " ");
    code = strtok(NULL, " ");

    // if the second word begins with 2xx, the book added successfully
    if (strncmp(code, "2", 1) == 0) {
        printf("The book was added successfully.\n");
    } else {
        printf("Error: The book wasn't added.\n");
    }
            
    free(copy_response);
    free(title);
    free(author);
    free(genre);
    free(publisher);
    free(page_count);
}

void delete_book_action(char *cookie, char *jwt_token_library, int sockfd) {

    char *message;
    char *response;

    char *id = calloc(100, sizeof(char));
    printf("id=");
    scanf("%s", id);

    char *path_for_book = calloc(strlen(BOOKS) + strlen(id) + 2, sizeof(char));
    strcpy(path_for_book, BOOKS);
    strcat(path_for_book, "/");
    strcat(path_for_book, id);
            
    // create the message
    message = compute_delete_request("34.254.242.81:8080", path_for_book,
                                     &cookie, 1, jwt_token_library);

    // send the message
    send_to_server(sockfd, message);

    // receive the response
    response = receive_from_server(sockfd);

    char *copy_response = calloc(strlen(response) + 1, sizeof(char));
    strcpy(copy_response, response);
    char *code = strtok(copy_response, " ");
    code = strtok(NULL, " ");

    // if the second word begins with 2xx, the book deleted successfully
    if (strncmp(code, "2", 1) == 0) {
        printf("The book with id %s was deleted successfully.\n", id);
    } else {
        printf("Error: The book with id %s doesn't exist in library\n", id);
    }

    free(copy_response);
    free(id);
    free(path_for_book);
}

void logout_action(char *cookie, char *jwt_token_library, int sockfd, bool* logged_in,
                     bool *action_auth) {

    char *message;
    char *response;
    // create the message
    message = compute_get_request("34.254.242.81:8080", LOGOUT, NULL,
                                     &cookie, 1, jwt_token_library);

    // send the message
    send_to_server(sockfd, message);

    // receive the response
    response = receive_from_server(sockfd);

    char *copy_response = calloc(strlen(response) + 1, sizeof(char));
    strcpy(copy_response, response);
    char *code = strtok(copy_response, " ");
    code = strtok(NULL, " ");
            
    // if the second word begins with 2xx, the user logged out successfully
    if (strncmp(code, "2", 1) == 0) {
        printf("You are logged out.\n");
        *logged_in = false;
        *action_auth = false;

    } else {
        printf("Error: Your logout request had a problem.\n");
    }

    free(copy_response);
}
