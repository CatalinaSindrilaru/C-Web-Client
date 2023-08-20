#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"

char *compute_get_request(char *host, char *url, char *query_params,
                            char **cookies, int cookies_count, char *jwt)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // method name, URL, protocol type and query parameters if existent
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // add authorization header if existent
    if (jwt != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: Bearer %s", jwt);
        compute_message(message, line);
    }

    // add cookies if existent
    if (cookies != NULL) {
       sprintf(line, "Cookie: ");

       for(int i = 0; i < cookies_count; i++) {
        sprintf(line + strlen(line), "%s; ", cookies[i]);
       }

       compute_message(message, line);
    }
    
    compute_message(message, "");
    return message;
}

char *compute_post_request(char *host, char *url, char *content_type, char **body_data,
                            int body_data_fields_count, char **cookies, int cookies_count, char *jwt)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    char *body_data_buffer = calloc(LINELEN, sizeof(char));

    // method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    // add the host
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // add authorization header if existent
    if (jwt != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: Bearer %s", jwt);
        compute_message(message, line);
    }

    // form the body data buffer
    for(int i = 0; i < body_data_fields_count; i++) {
        if (i != 0) {
            strcat(body_data_buffer, "&");
        }

        strcat(body_data_buffer, body_data[i]);
    }

    // add content type and content length
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    sprintf(line, "Content-Length: %lu", strlen(body_data_buffer));
    compute_message(message, line);

    // add cookies
    memset(line, 0, LINELEN);
    if (cookies != NULL) {
       sprintf(line, "Cookie: ");

       for(int i = 0; i < cookies_count; i++) {
        sprintf(line + strlen(line), "%s; ", cookies[i]);
       }

       compute_message(message, line);
    }

    compute_message(message, "");

    // add the actual payload data
    memset(line, 0, LINELEN);
    strcat(message, body_data_buffer);

    free(line);
    free(body_data_buffer);
    return message;
}

char *compute_delete_request(char *host, char *url, char **cookies, int cookies_count, char *jwt) {

    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    snprintf(line, LINELEN, "DELETE %s HTTP/1.1", url);
    compute_message(message, line);

    // add the host
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // add authorization header if existent
    if (jwt != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: Bearer %s", jwt);
        compute_message(message, line);
    }

    // add cookies if existent
    if (cookies != NULL) {
       sprintf(line, "Cookie: ");

       for(int i = 0; i < cookies_count; i++) {
        sprintf(line + strlen(line), "%s; ", cookies[i]);
       }

       compute_message(message, line);
    }

    compute_message(message, "");
    return message;
}