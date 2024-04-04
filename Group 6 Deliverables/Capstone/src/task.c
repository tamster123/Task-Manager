#include "task.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "database.h"
#include "profile.h"
#include "reminder.h"

// Implement createTaskForm
void createTaskForm(MYSQL *conn, User current_user)
{
    Task task;
    // task.user_id = current_user.user_id;
    // Prompt the user for task details and read input
    while (1)
    {
        printf("Enter Task Name (100 characters max): ");
        if (scanf(" %100[^\n]", task.task_name) == 1)
        {
            break;
        }
        else
        {
            printf("Error: Failed to read task name.\n");
            while (getchar() != '\n')
                ; // Clear input buffer
        }
    }

    while (1)
    {
        printf("Enter Description (200 characters max): ");
        if (scanf(" %200[^\n]", task.description) == 1)
        {
            break;
        }
        else
        {
            printf("Error: Failed to read description.\n");
            while (getchar() != '\n')
                ; // Clear input buffer
        }
    }

    while (1)
    {
        printf("Enter Task Duration (HH:MM:SS): \n");
        if (scanf(" %8[^\n]", task.task_duration) == 1)
        {

            if (!timeFormatValid(task.task_duration))
            {
                printf("Error: Please write the time in HH:MM:SS format.\n");
            }
            else
            {
                break;
            }
        }
        else
        {
            printf("Error: Failed to read task duration");
            while (getchar() != '\n')
                ; // Exit function if input fails
        }
    }

    while (1)
    {
        printf("Enter Priority (low/medium/high): ");
        if (scanf(" %6[^\n]", task.priority) == 1)
        {
            break;
        }
        else
        {
            printf("Error: Failed to read priority.\n");
            while (getchar() != '\n')
                ; // Clear input buffer
        }
    }

    // Prepare the query to insert the task
    char query[1000];
    sprintf(query, "INSERT INTO task (user_id, task_name, description, task_duration, priority, complete) VALUES (%d, '%s', '%s', '%s', '%s', 0)",
            current_user.user_id, task.task_name, task.description, task.task_duration, task.priority);

    // Execute the query to create the task
    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Query execution error: %s\n", mysql_error(conn));
        return;
    }

    // Perform a separate query to retrieve the task_id of the newly created task
    if (mysql_query(conn, "SELECT LAST_INSERT_ID()"))
    {
        fprintf(stderr, "Error retrieving task ID: %s\n", mysql_error(conn));
        return;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL)
    {
        fprintf(stderr, "Error retrieving result set: %s\n", mysql_error(conn));
        return;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row == NULL)
    {
        fprintf(stderr, "Error: Failed to fetch task ID.\n");
        mysql_free_result(result);
        return;
    }

    int taskID = atoi(row[0]);
    mysql_free_result(result);

    // Prompt the user to choose a category
    printf("Choose a category for the task:\n");
    printCategories(conn);
    int categoryID;
    printf("Enter the ID of the category: ");
    scanf("%d", &categoryID);

    // Insert the selected category into task_category table
    char categoryQuery[100];
    sprintf(categoryQuery, "INSERT INTO task_category (task_id, category_id) VALUES (%d, %d)", taskID, categoryID);
    if (mysql_query(conn, categoryQuery))
    {
        fprintf(stderr, "Error inserting category: %s\n", mysql_error(conn));
        return;
    }

    printf("Task created successfully!\n");
}

void printCategories(MYSQL*conn)
{
    MYSQL_RES *result;
    MYSQL_ROW row;

    execute_query(conn,"SELECT category_id, category_name FROM category;");
    result = mysql_store_result(conn);

    if (result == NULL)
    {
        fprintf(stderr, "Error retrieving result set: %s\n", mysql_error(conn));
        return;
    }

    // Print each category with its ID
    while ((row = mysql_fetch_row(result)))
    {
        printf("Category ID: %s, Name: %s\n", row[0], row[1]);
    }

    // Free the result set
    mysql_free_result(result);
}

// Implement manageTask
void manageTask(MYSQL *conn, User current_user) {
    int choice;
    Task task; // Declaration of Task structure for use within this function
    do
    {
        showUserTasks(conn, current_user);

        printf("\nEnter 1 to Update Task.\n");
        printf("Enter 2 to Delete Task.\n");
        printf("Enter 0 to exit back to main menu.\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); // Consume newline

        if (choice == 1 || choice == 2)
        {
            printf("Enter the ID of the task you want to manage: ");
            scanf("%d", &task.task_id);
            getchar(); // Consume newline

            if (validateTaskID(conn, current_user, task.task_id))
            {
                if (choice == 1)
                {
                    updateTaskForm(conn, task, current_user);
                }
                else
                {
                    deleteTaskByID(conn, task, current_user);
                }
            }
            else
            {
                printf("Invalid Task ID or task does not belong to you.\n");
            }
        }
        else if (choice == 0)
        {
            printf("Exiting to main menu.\n");
            return;
        }
        else
        {
            printf("Invalid choice. Please enter a valid option.\n");
        }
    } while (choice != 0);
}

// Implement updateTaskForm
void updateTaskForm(MYSQL *conn, Task task, User current_user) {
    char inputBuffer[256]; // Buffer for user input
    char query[1024];      // Query buffer

    printf("You're updating the task with ID: %d\n", task.task_id);
    printf("Leave the input empty and press enter if you don't want to update a field.\n");

    // Update Task Name
    printf("Enter new Task Name (100 characters max): ");
    fgets(inputBuffer, sizeof(inputBuffer), stdin);
    if (strcmp(inputBuffer, "\n") != 0)
    {
        inputBuffer[strcspn(inputBuffer, "\n")] = 0; // Remove newline character
        sprintf(query, "UPDATE task SET task_name = '%s' WHERE task_id = %d AND user_id = %d",
                inputBuffer, task.task_id, current_user.user_id);
        if (mysql_query(conn, query))
        {
            fprintf(stderr, "Failed to update task name: %s\n", mysql_error(conn));
        }
        else
        {
            printf("Task name updated successfully.\n");
        }
    }

    // Update Description
    printf("Enter new Description (200 characters max): ");
    fgets(inputBuffer, sizeof(inputBuffer), stdin);
    if (strcmp(inputBuffer, "\n") != 0)
    {
        inputBuffer[strcspn(inputBuffer, "\n")] = 0; // Remove newline character
        sprintf(query, "UPDATE task SET description = '%s' WHERE task_id = %d AND user_id = %d",
                inputBuffer, task.task_id, current_user.user_id);
        if (mysql_query(conn, query))
        {
            fprintf(stderr, "Failed to update description: %s\n", mysql_error(conn));
        }
        else
        {
            printf("Description updated successfully.\n");
        }
        // Update Task Duration
        printf("Enter new Task Duration (HH:MM:SS): ");
        fgets(inputBuffer, sizeof(inputBuffer), stdin);
        if (strcmp(inputBuffer, "\n") != 0)
        {
            inputBuffer[strcspn(inputBuffer, "\n")] = 0; // Remove newline character
            sprintf(query, "UPDATE task SET task_duration = '%s' WHERE task_id = %d AND user_id = %d",
                    inputBuffer, task.task_id, current_user.user_id);
            if (mysql_query(conn, query))
            {
                fprintf(stderr, "Failed to update task duration: %s\n", mysql_error(conn));
            }
            else
            {
                printf("Task duration updated successfully.\n");
            }
        }

        // Update Priority
        printf("Enter new Priority (low/medium/high): ");
        fgets(inputBuffer, sizeof(inputBuffer), stdin);
        if (strcmp(inputBuffer, "\n") != 0)
        {
            inputBuffer[strcspn(inputBuffer, "\n")] = 0; // Remove newline character
            sprintf(query, "UPDATE task SET priority = '%s' WHERE task_id = %d AND user_id = %d",
                    inputBuffer, task.task_id, current_user.user_id);
            if (mysql_query(conn, query))
            {
                fprintf(stderr, "Failed to update priority: %s\n", mysql_error(conn));
            }
            else
            {
                printf("Priority updated successfully.\n");
            }
        }

        // Update Complete Status
        printf("Is the task complete? (yes/no): ");
        fgets(inputBuffer, sizeof(inputBuffer), stdin);
        int complete = 0; // Default to not complete
        if (strncmp(inputBuffer, "yes", 3) == 0)
        {
            complete = 1; // Task is complete
        }
        else if (strncmp(inputBuffer, "no", 2) == 0)
        {
            complete = 0; // Task is not complete
        }

        // Update the database only if "yes" or "no" was explicitly mentioned
        if (strncmp(inputBuffer, "yes", 3) == 0 || strncmp(inputBuffer, "no", 2) == 0)
        {
            sprintf(query, "UPDATE task SET complete = %d WHERE task_id = %d AND user_id = %d",
                    complete, task.task_id, current_user.user_id);
            if (mysql_query(conn, query))
            {
                fprintf(stderr, "Failed to update task completion status: %s\n", mysql_error(conn));
            }
            else
            {
                printf("Task completion status updated successfully.\n");
            }
        }
    }
    return;
}
// Implement deleteTaskByID
void deleteTaskByID(MYSQL *conn, Task task, User current_user) {
       char userInput[10]; // Buffer for user input
                        // Ask the user for confirmation before deletion
    printf("Are you sure you want to delete the task with ID %d? (yes/no): ", task.task_id);
    char confirmation[4];
    fgets(confirmation, sizeof(confirmation), stdin);
    confirmation[strcspn(confirmation, "\n")] = 0; // Remove newline character

    if (strcmp(confirmation, "yes") == 0)
    {
        // Start transaction
        mysql_query(conn, "START TRANSACTION");

        char deleteCategoryQuery[512];
        sprintf(deleteCategoryQuery, "DELETE FROM task_category WHERE task_id = %d", task.task_id);

        if (mysql_query(conn, deleteCategoryQuery))
        {
            fprintf(stderr, "Failed to delete from task_category: %s\n", mysql_error(conn));
            mysql_query(conn, "ROLLBACK"); // Rollback the transaction
            return;
        }

        char deleteTaskQuery[512];
        sprintf(deleteTaskQuery, "DELETE FROM task WHERE task_id = %d AND user_id = %d", task.task_id, current_user.user_id);

        if (mysql_query(conn, deleteTaskQuery))
        {
            fprintf(stderr, "Failed to delete task: %s\n", mysql_error(conn));
            mysql_query(conn, "ROLLBACK"); // Rollback the transaction
            return;
        }

        // If no errors occurred, commit the transaction
        if (mysql_query(conn, "COMMIT"))
        {
            fprintf(stderr, "Failed to commit the transaction: %s\n", mysql_error(conn));
        }
        else
        {
            printf("Task and associated categories successfully deleted.\n");
        }
    }
    else
    {
        printf("Task deletion cancelled.\n");
    }
}

// Implement validateTaskID
bool validateTaskID(MYSQL *conn, User current_user, int taskID) {
      char query[256];
    sprintf(query, "SELECT EXISTS(SELECT 1 FROM task WHERE task_id = %d AND user_id = %d)", taskID, current_user.user_id);

    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Query execution error: %s\n", mysql_error(conn));
        return false;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL)
    {
        fprintf(stderr, "Error retrieving result set: %s\n", mysql_error(conn));
        return false;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    bool exists = row && atoi(row[0]);
    mysql_free_result(result);

    return exists; 
}

// Implement fetchTaskById
int fetchTaskById(MYSQL *conn, User current_user, Task *task) {
    // Implementation detail...
    return 0; // Example return
}


void showUserTasks(MYSQL *conn, User current_user)
{
    // Query to retrieve all tasks associated with the current user
    char query[1000];
    sprintf(query, "SELECT task_id, task_name, description, task_duration, priority FROM task WHERE user_id = (SELECT user_id FROM user WHERE username = '%s')", current_user.username);

    // Execute the query
    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Query execution error: %s\n", mysql_error(conn));
        return;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL)
    {
        fprintf(stderr, "Error retrieving result set: %s\n", mysql_error(conn));
        return;
    }

    // Print the header
    printf("%-10s %-30s %-30s %-10s %-10s\n", "Task ID", "Task Name", "Description", "Duration", "Priority");
    printf("%-10s %-30s %-30s %-10s %-10s\n", "----------", "------------------------------", "------------------------------", "----------", "----------");
    // Fetch and print each row
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)))
    {
        printf("%-10s %-30s %-30s %-10s %-10s\n", row[0], row[1], row[2], row[3], row[4]);
    }

    mysql_free_result(result);
}