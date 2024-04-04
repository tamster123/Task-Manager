#ifndef DATABASE_H
#define DATABASE_H
#define DB_HOST "localhost"
#define DB_USER "root"
#define DB_PASSWORD "student"
#define DB_NAME "task_manager"

#include <mysql/mysql.h> 
MYSQL *init_connection(const char *host, const char *user, const char *pwd, const char *db_name);

void execute_query(MYSQL *conn, const char *query);
//void execute_query(const char *query);



#endif // DATABASE_H