#ifndef _ACTIONS_
#define _ACTIONS_

void register_action(char *username, char *password, int sockfd);

void login_action(char *username, char *password, int sockfd, char **cookie, bool *logged_in);

char *enter_library_action(char *cookie, int sockfd) ;

void get_books_action(char *cookie, char *jwt_token_library, int sockfd);

void get_book_action(char *cookie, char *jwt_token_library, int sockfd);

void add_book_action(char *cookie, char *jwt_token_library, int sockfd);

void delete_book_action(char *cookie, char *jwt_token_library, int sockfd);

void logout_action(char *cookie, char *jwt_token_library, int sockfd, bool* logged_in, bool *action_auth);


#endif
