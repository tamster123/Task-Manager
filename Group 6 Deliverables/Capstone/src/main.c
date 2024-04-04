#include <stdio.h>
#include <stdlib.h>
#include <mysql/mysql.h>
#include <string.h>
#include <ctype.h>
#include <openssl/sha.h>
#include "database.h"
#include "profile.h"
#include "task.h"
#include "reminder.h"
#include <ctype.h>
#define DB_HOST "localhost"
#define DB_USER "root"
#define DB_PASSWORD "student"
#define DB_NAME "task_manager"



int main()
{
    //MYSQL *conn;
    MYSQL *conn = mysql_init(NULL);
    if (conn == NULL)
    {
        fprintf(stderr, "mysql_init() failed\n");
        return EXIT_FAILURE;
    }

    if (mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASSWORD, DB_NAME, 0, NULL, 0) == NULL)
    {
        fprintf(stderr, "mysql_real_connect() failed: %s\n", mysql_error(conn));
        mysql_close(conn);
        return EXIT_FAILURE;
    }
    // Need to create the database
  
    execute_query(conn,"DROP TABLE IF EXISTS task_reminder;");
    execute_query(conn,"DROP TABLE IF EXISTS task_category;");
    execute_query(conn,"DROP TABLE IF EXISTS task_history;");
    execute_query(conn,"DROP TABLE IF EXISTS task;");
    execute_query(conn,"DROP TABLE IF EXISTS bash_script;");
    execute_query(conn,"DROP TABLE IF EXISTS category;");
    execute_query(conn,"DROP TABLE IF EXISTS user;");
    execute_query(conn,"CREATE TABLE user (user_id INT AUTO_INCREMENT PRIMARY KEY, name VARCHAR(50) NOT NULL, username VARCHAR(200) UNIQUE NOT NULL, password VARCHAR(15) NOT NULL, dob DATE, email_address VARCHAR(200) UNIQUE NOT NULL, phone_number VARCHAR(15) NOT NULL);");
    execute_query(conn,"CREATE TABLE task (task_id INT AUTO_INCREMENT PRIMARY KEY, user_id INT, task_name VARCHAR(100), description VARCHAR(200), task_duration TIME, priority VARCHAR(6) DEFAULT 'low', complete BOOLEAN DEFAULT 0, FOREIGN KEY (user_id) REFERENCES user(user_id));");
    execute_query(conn,"CREATE TABLE category (category_id INT AUTO_INCREMENT PRIMARY KEY, category_name VARCHAR(30));");
    execute_query(conn,"CREATE TABLE task_category (task_id INT, category_id INT, PRIMARY KEY (task_id, category_id), FOREIGN KEY (task_id) REFERENCES task(task_id), FOREIGN KEY (category_id) REFERENCES category(category_id));");
    execute_query(conn,"CREATE TABLE task_history (history_id INT AUTO_INCREMENT PRIMARY KEY, task_id INT, user_id INT, history VARCHAR(15), time_completion TIMESTAMP DEFAULT CURRENT_TIMESTAMP, FOREIGN KEY (task_id) REFERENCES task(task_id), FOREIGN KEY (user_id) REFERENCES user(user_id));");
    execute_query(conn,"CREATE TABLE task_reminder (reminder_id INT AUTO_INCREMENT PRIMARY KEY, task_id INT, user_id INT, reminder_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP, message VARCHAR(200), status VARCHAR(50) DEFAULT 'incomplete', FOREIGN KEY (task_id) REFERENCES task(task_id), FOREIGN KEY (user_id) REFERENCES user(user_id));");
    execute_query(conn,"CREATE TABLE bash_script (script_id INT AUTO_INCREMENT PRIMARY KEY, script_name VARCHAR(255), script_content TEXT);");

    // Insert task categories
    execute_query(conn,"INSERT INTO category (category_name) VALUES ('Leisure');");
    execute_query(conn,"INSERT INTO category (category_name) VALUES ('Work');");
    execute_query(conn,"INSERT INTO category (category_name) VALUES ('Exercise');");
    execute_query(conn,"INSERT INTO category (category_name) VALUES ('Personal');");
    execute_query(conn,"INSERT INTO category (category_name) VALUES ('Health');");
    execute_query(conn,"INSERT INTO category (category_name) VALUES ('Fitness');");
    execute_query(conn,"INSERT INTO category (category_name) VALUES ('Education');");
    execute_query(conn,"INSERT INTO category (category_name) VALUES ('Errands');");
    execute_query(conn,"INSERT INTO category (category_name) VALUES ('Social');");
    execute_query(conn,"INSERT INTO category (category_name) VALUES ('Finance');");
    execute_query(conn,"INSERT INTO category (category_name) VALUES ('Hobby');");
    execute_query(conn,"INSERT INTO category (category_name) VALUES ('Travel');");
    execute_query(conn,"INSERT INTO category (category_name) VALUES ('Cleaning');");
    execute_query(conn,"INSERT INTO category (category_name) VALUES ('Self-care');");
    execute_query(conn,"INSERT INTO category (category_name) VALUES ('Project');");
    execute_query(conn,"INSERT INTO category (category_name) VALUES ('Appointment');");
    execute_query(conn,"INSERT INTO category (category_name) VALUES ('Meeting');");
    execute_query(conn,"INSERT INTO category (category_name) VALUES ('Fun');");
    execute_query(conn,"INSERT INTO category (category_name) VALUES ('Shopping');");

    // Insert sample data into the user table
    execute_query(conn,"INSERT INTO user (name, username, password, dob, email_address, phone_number) VALUES ('Jared', 'jared123', 'password123', '1990-05-15', 'jared@example.com', '1234567890');");
    execute_query(conn,"INSERT INTO user (name, username, password, dob, email_address, phone_number) VALUES ('Aysia', 'aysia456', 'password456', '1985-09-20', 'aysia@example.com', '9876543210');");
    execute_query(conn,"INSERT INTO user (name, username, password, dob, email_address, phone_number) VALUES ('Tammie', 'tammie789', 'password789', '1978-12-10', 'tammie@example.com', '4567890123');");

    execute_query(conn,"INSERT INTO task (user_id, task_name, description, task_duration, priority, complete) VALUES (1, 'Finish project proposal', 'Prepare project proposal document for review', '02:30:00', 'high', 0), (2, 'Gym workout', 'Go to the gym and do strength training', '01:00:00', 'medium', 0), (3, 'Grocery shopping', 'Buy groceries for the week', '01:30:00', 'high', 0);");

    execute_query(conn,"INSERT INTO task_category (task_id, category_id) VALUES (1, 2), (2, 3), (3, 8);");
    int choice;

    do
    {
        // Display menu and get user choice
        printf("Enter 1 to create a profile.\n");
        printf("Enter 2 to sign into profile.\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            createUserProfile(conn);
            break;
        case 2:
            signIn(conn);
            break;
        default:
            printf("Invalid choice. Please choose a valid option.\n");
        }
    } while (choice != 0);

    mysql_close(conn);
    return 0;
}