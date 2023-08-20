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

int main(int argc, char *argv[])
{
    char *cookie = NULL;
    char *jwt_token_library = NULL;
    char *command = NULL;
    int sockfd;
    bool logged_in = false;
    bool action_auth = false;
    bool run = true;

    while(run) {

        sockfd = open_connection("34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);

        // Read command
        command = calloc(100, sizeof(char));
        scanf("%s", command);

        if (strcmp(command, "register") == 0) {

            char *username = calloc(100, sizeof(char));
            char *password = calloc(100, sizeof(char));

            read_input("username=", username);
            read_input("password=", password);

            // verify if the username and the password contain spaces
            if (containsSpaces(username) || containsSpaces(password)) {
                printf("Error: The username and the password can not contain spaces. Please try again\n");
                continue;
            }

            // verify if the user is already logged in
            if (logged_in) {
                printf("Error: A login is already made. You must logout first.\n");
                continue;
            }

            register_action(username, password, sockfd);

            free(username);
            free(password);

        } else if (strcmp(command, "login") == 0) {

            char *username = calloc(100, sizeof(char));
            char *password = calloc(100, sizeof(char));

            read_input("username=", username);
            read_input("password=", password);

            // verify if the username and the password contain spaces
            if (containsSpaces(username) || containsSpaces(password)) {
                printf("Error: The username and the password can not contain spaces.Please try again\n");
                continue;
            }

            // verify if the user is already logged in
            if (logged_in) {
                printf("Error: A login is already made. You must logout first.\n");
                continue;
            }

            login_action(username, password, sockfd, &cookie, &logged_in);

            free(username);
            free(password);
            
        } else if (strcmp(command, "enter_library") == 0) {

            if (!logged_in) {
                printf("Error: You must login first.\n");
                continue;
            }

            jwt_token_library = enter_library_action(cookie, sockfd);

            action_auth = true;


        } else if (strcmp(command, "get_books") == 0) {

            // verify if the user is logged in
            if (!logged_in) {
                printf("Error: You must login first and then enter the library.\n");
                continue;
            } 

            // verify if the user has entered the library
            if (!action_auth) {
                printf("Error: You must enter the library first.\n");
                continue;
            }

            get_books_action(cookie, jwt_token_library, sockfd);

        } else if (strcmp(command, "get_book") == 0) {
            
            // verify if the user is logged in
            if (!logged_in) {
                printf("Error: You must login first and then enter the library.\n");
                continue;
            } 

            // verify if the user has entered the library
            if (!action_auth) {
                printf("Error: You must enter the library first.\n");
                continue;
            }

            get_book_action(cookie, jwt_token_library, sockfd);
        
        } else if (strcmp(command, "add_book") == 0) {

            // verify if the user is logged in
            if (!logged_in) {
                printf("Error: You must login first and then enter the library.\n");
                continue;
            } 

            // verify if the user has entered the library
            if (!action_auth) {
                printf("Error: You must enter the library first.\n");
                continue;
            }

            add_book_action(cookie, jwt_token_library, sockfd);

        } else if (strcmp(command, "delete_book") == 0) {

            // verify if the user is logged in
            if (!logged_in) {
                printf("Error: You must login first and then enter the library.\n");
                continue;
            } 

            // verify if the user has entered the library
            if (!action_auth) {
                printf("Error: You must enter the library first.\n");
                continue;
            }

            delete_book_action(cookie, jwt_token_library, sockfd);

        } else if (strcmp(command, "logout") == 0) {

            // verify if the user is logged in
            if (!logged_in) {
                printf("Error: You must login first.\n");
                continue;
            }

            logout_action(cookie, jwt_token_library, sockfd, &logged_in, &action_auth);

        } else if (strcmp(command, "exit") == 0) {
            run = false;
            
        } else {
            printf("Your command is not valid\n");
        }

        close_connection(sockfd);
    }

    return 0;
}