#ifndef TASK_H
#define TASK_H

#include <mysql/mysql.h> // Include the MySQL definitions
#include <stdbool.h>     // For `bool` type

// Assuming User is defined in profile.h
#include "profile.h"
#include "database.h"
#include "reminder.h"
#include <ctype.h>

// Struct representing a task in the system
typedef struct {
    int task_id;                // Unique identifier for the task
    char task_name[101];        // Name of the task
    char description[201];      // Description of the task
    char task_duration[9];      // Duration of the task in HH:MM:SS format
    char priority[7];           // Priority of the task, e.g., "low", "medium", "high"
    int user_id;                // ID of the user who owns the task
    bool complete;              // Completion status of the task
} Task;

// Function prototypes for task-related operations:

// Create a task entry form for the user to fill out and save a new task
void createTaskForm(MYSQL *conn, User current_user);

// Manage tasks for the current user, allowing them to view, update, or delete tasks
void manageTask(MYSQL *conn, User current_user);

// Update the details of an existing task based on user input
void updateTaskForm(MYSQL *conn, Task task, User current_user);

// Delete a specific task identified by its task ID
void deleteTaskByID(MYSQL *conn, Task task, User current_user);

// Check if a task ID is valid and belongs to the current user
bool validateTaskID(MYSQL *conn, User current_user, int taskID);

// Fetch the details of a task by its ID and populate a Task struct with the data
int fetchTaskById(MYSQL *conn, User current_user, Task *task);

// Print the list of all available task categories
void printCategories(MYSQL *conn);

// Show all tasks associated with the current user
void showUserTasks(MYSQL *conn, User current_user);


#endif // TASK_H