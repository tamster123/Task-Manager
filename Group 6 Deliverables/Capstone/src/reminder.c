#include "profile.h"
#include "task.h"
#include <stdio.h>
#include <string.h>
#include "database.h"
#include "reminder.h"






void manageReminders(MYSQL *conn, User current_user) {
  int choice;
  do {




      printf("Manage Reminders:\n");
      printf("1. Show Reminders\n");
      printf("2. Create a Reminder\n");
      printf("3. Update a Reminder\n");
      printf("4. Delete a Reminder\n");
      printf("5. Exit\n");
      printf("Enter your choice: ");
      scanf("%d", &choice);




      switch (choice) {
          case 1:
              showUserReminders(conn, current_user);
              break;
          case 2:
              {
                  int task_id;
                  char message[200];
                  char status[20];
                  int year, month, day, hour, minute, second;

                  printf("Enter ID of the task you want a reminder for: ");
                  scanf("%d", &task_id);

                  printf("Enter reminder message (200 characters max): ");
                  scanf(" %199[^\n]", message);

                  printf("Enter the day and time you want to be reminded in this exact format (YYYY-MM-DD HH:MM:SS): ");
                  scanf("%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);

                  // Assuming 'status' is fixed, otherwise prompt for it
                  strcpy(status, "incomplete");

                  createReminder(conn, current_user.user_id, task_id, message, status, year, month, day, hour, minute, second);
              }
              break;
          case 3:
              {
                   int reminder_id;
                  int task_id;
                  char message[200];
                  char status[20];
                  int year, month, day, hour, minute, second;

                  printf("Enter ID of the reminder you want to update: ");
                  scanf("%d", &reminder_id);

                  printf("Enter ID of the task for the updated reminder: ");
                  scanf("%d", &task_id);

                  printf("Enter updated reminder message (200 characters max): ");
                  scanf(" %199[^\n]", message);


                printf("Enter the updated day and time for the reminder in this exact format (YYYY-MM-DD HH:MM:SS): ");
                  scanf("%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);

                  strcpy(status, "incomplete");
                  updateReminder(conn, reminder_id, task_id, message, status, year, month, day, hour, minute, second);
              }
              break;
          case 4:
              {
                   // Delete a Reminder
                  int reminder_id;
                  printf("Enter ID of the reminder you want to delete: ");
                  scanf("%d", &reminder_id);


            // Call a function to delete the reminder from the database
                  deleteReminder(conn, reminder_id);
              }
              break;
          case 5:
              printf("Exiting...\n");
              break;
          default:
              printf("Invalid choice\n");
      }
  } while (choice != 5);
}






void showUserReminders(MYSQL *conn, User current_user) {
  // Query to retrieve all reminders associated with the current user
  char query[1000];
  sprintf(query, "SELECT reminder_id, task_id, reminder_time, message, status FROM task_reminder WHERE user_id = (SELECT user_id FROM user WHERE username = '%s')", current_user.username);

 // Execute the query
  if (mysql_query(conn, query)) {
      fprintf(stderr, "Query execution error: %s\n", mysql_error(conn));
      return;
  }

  MYSQL_RES *result = mysql_store_result(conn);
  if (result == NULL) {
      fprintf(stderr, "Error retrieving result set: %s\n", mysql_error(conn));
      return;
  }

  // Print the header
  printf("%-10s %-10s %-30s %-30s %-10s\n", "Reminder ID", "Task ID", "Reminder Time", "Message", "Status");


  // Fetch and print each row
  MYSQL_ROW row;
  while ((row = mysql_fetch_row(result))) {
      printf("%-10s %-10s %-30s %-30s %-10s\n", row[0], row[1], row[2], row[3], row[4]);
  }

  mysql_free_result(result);
}




// Function to create a new reminder
void createReminder(MYSQL *conn, int user_id, int task_id, const char *message, const char *status, int year, int month, int day, int hour, int minute, int second) {
    // Initialize variables for prepared statement and result checking
    MYSQL_STMT *stmt_reminder;
    my_ulonglong affected_rows;

    // Initialize the prepared statement
    stmt_reminder = mysql_stmt_init(conn);
    if (!stmt_reminder) {
        fprintf(stderr, "mysql_stmt_init() failed\n");
        return;
    }

    // Prepare the SQL query for inserting a new reminder
    if (mysql_stmt_prepare(stmt_reminder, "INSERT INTO task_reminder (task_id, user_id, reminder_time, message, status) VALUES (?, ?, ?, ?, ?)", strlen("INSERT INTO task_reminder (task_id, user_id, reminder_time, message, status) VALUES (?, ?, ?, ?, ?)")) != 0) {
        fprintf(stderr, "mysql_stmt_prepare() failed: %s\n", mysql_stmt_error(stmt_reminder));
        mysql_stmt_close(stmt_reminder); // Clean up statement on failure
        return;
    }

    // Bind parameters for the SQL query
    MYSQL_BIND params[5];
    memset(params, 0, sizeof(params)); // Initialize the params array

    // Setup the MYSQL_TIME structure for the datetime parameter
    MYSQL_TIME mysql_time;
    memset(&mysql_time, 0, sizeof(mysql_time));
    mysql_time.year = year;
    mysql_time.month = month;
    mysql_time.day = day;
    mysql_time.hour = hour;
    mysql_time.minute = minute;
    mysql_time.second = second;

    // Bind the task ID parameter
    params[0].buffer_type = MYSQL_TYPE_LONG;
    params[0].buffer = (char *)&task_id;

    // Bind the user ID parameter
    params[1].buffer_type = MYSQL_TYPE_LONG;
    params[1].buffer = (char *)&user_id;

    // Bind the reminder time parameter
    params[2].buffer_type = MYSQL_TYPE_DATETIME;
    params[2].buffer = (char *)&mysql_time;
    params[2].is_null = 0; // DateTime cannot be NULL
    params[2].length = 0; // Not used for fixed-length types

    // Bind the message parameter
    params[3].buffer_type = MYSQL_TYPE_STRING;
    params[3].buffer = (char *)message;
    params[3].buffer_length = strlen(message);

    // Bind the status parameter
    params[4].buffer_type = MYSQL_TYPE_STRING;
    params[4].buffer = (char *)status;
    params[4].buffer_length = strlen(status);

    // Execute the statement
    if (mysql_stmt_bind_param(stmt_reminder, params)) {
        fprintf(stderr, "mysql_stmt_bind_param() failed\n");
        mysql_stmt_close(stmt_reminder); // Clean up on failure
        return;
    }

    // Execute the statement
    if (mysql_stmt_execute(stmt_reminder)) {
        fprintf(stderr, "mysql_stmt_execute() failed\n");
        mysql_stmt_close(stmt_reminder); // Clean up on failure
        return;
    }

    // Check if the reminder was successfully inserted
    affected_rows = mysql_stmt_affected_rows(stmt_reminder);
    if (affected_rows == 1) {
        printf("Reminder set successfully!\n");
    } else {
        printf("Failed to set reminder.\n");
    }

    // Clean up the statement
    mysql_stmt_close(stmt_reminder);
}




void updateReminder(MYSQL *conn, int reminder_id, int task_id, const char *message, const char *status, int year, int month, int day, int hour, int minute, int second) {
  MYSQL_STMT *stmt;
  my_ulonglong affected_rows;


  stmt = mysql_stmt_init(conn);
  if (!stmt) {
      fprintf(stderr, "mysql_stmt_init() failed\n");
      return;
  }




  if (mysql_stmt_prepare(stmt, "UPDATE task_reminder SET task_id=?, reminder_time=?, message=?, status=? WHERE reminder_id=?", strlen("UPDATE task_reminder SET task_id=?, reminder_time=?, message=?, status=? WHERE reminder_id=?")) != 0) {
      fprintf(stderr, "mysql_stmt_prepare() failed: %s\n", mysql_stmt_error(stmt));
      mysql_stmt_close(stmt);
      return;
  }




  MYSQL_BIND params[6];
  memset(params, 0, sizeof(params));




  MYSQL_TIME mysql_time;
  memset(&mysql_time, 0, sizeof(mysql_time));
  mysql_time.year = year;
  mysql_time.month = month;
  mysql_time.day = day;
  mysql_time.hour = hour;
  mysql_time.minute = minute;
  mysql_time.second = second;




  params[0].buffer_type = MYSQL_TYPE_LONG;
  params[0].buffer = (char *)&task_id;




  params[1].buffer_type = MYSQL_TYPE_DATETIME;
  params[1].buffer = (char *)&mysql_time;
  params[1].is_null = 0;
  params[1].length = 0;




  params[2].buffer_type = MYSQL_TYPE_STRING;
  params[2].buffer = (char *)message;
  params[2].buffer_length = strlen(message);




  params[3].buffer_type = MYSQL_TYPE_STRING;
  params[3].buffer = (char *)status;
  params[3].buffer_length = strlen(status);




  params[4].buffer_type = MYSQL_TYPE_LONG;
  params[4].buffer = (char *)&reminder_id;




  if (mysql_stmt_bind_param(stmt, params)) {
      fprintf(stderr, "mysql_stmt_bind_param() failed\n");
      mysql_stmt_close(stmt);
      return;
  }




  if (mysql_stmt_execute(stmt)) {
      fprintf(stderr, "mysql_stmt_execute() failed\n");
      mysql_stmt_close(stmt);
      return;
  }




  affected_rows = mysql_stmt_affected_rows(stmt);
  if (affected_rows == 1) {
      printf("Reminder updated successfully!\n");
  } else {
      printf("Failed to update reminder.\n");
  }




  mysql_stmt_close(stmt);
}




void deleteReminder(MYSQL *conn, int reminder_id) {
  MYSQL_STMT *stmt;
  my_ulonglong affected_rows;




  stmt = mysql_stmt_init(conn);
  if (!stmt) {
      fprintf(stderr, "mysql_stmt_init() failed\n");
      return;
  }




  if (mysql_stmt_prepare(stmt, "DELETE FROM task_reminder WHERE reminder_id=?", strlen("DELETE FROM task_reminder WHERE reminder_id=?")) != 0) {
      fprintf(stderr, "mysql_stmt_prepare() failed: %s\n", mysql_stmt_error(stmt));
      mysql_stmt_close(stmt);
      return;
  }




  MYSQL_BIND param;
  memset(&param, 0, sizeof(param));




  param.buffer_type = MYSQL_TYPE_LONG;
  param.buffer = (char *)&reminder_id;




  if (mysql_stmt_bind_param(stmt, &param)) {
      fprintf(stderr, "mysql_stmt_bind_param() failed\n");
      mysql_stmt_close(stmt);
      return;
  }




  if (mysql_stmt_execute(stmt)) {
      fprintf(stderr, "mysql_stmt_execute() failed\n");
      mysql_stmt_close(stmt);
      return;
  }




  affected_rows = mysql_stmt_affected_rows(stmt);
  if (affected_rows == 1) {
      printf("Reminder deleted successfully!\n");
  } else {
      printf("Failed to delete reminder.\n");
  }




  mysql_stmt_close(stmt);
}
