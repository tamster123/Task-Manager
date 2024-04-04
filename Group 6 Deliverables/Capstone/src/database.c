#include "database.h"
#include <stdio.h>
#include <stdlib.h>


// Initialize connection to the database
MYSQL *init_connection(const char *host, const char *user, const char *pwd, const char *db_name) {
    MYSQL *conn = mysql_init(NULL);
    if (!conn) {
        fprintf(stderr, "mysql_init() failed\n");
        exit(EXIT_FAILURE);
    }
    if (!mysql_real_connect(conn, host, user, pwd, db_name, 0, NULL, 0)) {
        fprintf(stderr, "mysql_real_connect() failed: %s\n", mysql_error(conn));
        mysql_close(conn);
        exit(EXIT_FAILURE);
    }
    return conn;
}

// Implementation of execute_query
void execute_query(MYSQL *conn, const char *query) {
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Query execution error: %s\n", mysql_error(conn));
        mysql_close(conn);
        exit(EXIT_FAILURE);
    }
}

// void execute_query(const char *query)
// {
//     if (mysql_query(MYSQL *conn, query))
//     {
//         fprintf(stderr, "Query execution error: %s\n", mysql_error(conn));
//         mysql_close(conn);
//         exit(EXIT_FAILURE);
//     }
// }