#include "profile.h" 
#include "database.h"
#include "task.h"
#include "reminder.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define DB_HOST "localhost"
#define DB_USER "root"
#define DB_PASSWORD "student"
#define DB_NAME "task_manager"

//MYSQL *conn;
// Global variable to store the current user's information
User current_user;

// Function to authenticate a user's login credentials
void signIn(MYSQL *conn)
{
    char password[15]; // Buffer to store the user's password input

    // Prompt the user for their username and password
    printf("Enter username: ");
    scanf("%199s", current_user.username); // Read the username input into the current_user structure

    printf("Enter password: ");
    scanf("%14s", current_user.password); // Read the password input into the current_user structure

    // Attempt to authenticate the user with the provided credentials
    int resultAuth = userAuthentication(conn, current_user.username, current_user.password);

    switch (resultAuth)
    {
    case 1: // Authentication succeeded
        printf("User verified\n");
        // Query to fetch the user ID from the database using the username
        char user_id_query[1000];
        sprintf(user_id_query, "SELECT user_id FROM user WHERE username='%s'", current_user.username);

        if (mysql_query(conn, user_id_query)) // Execute the query
        {
            fprintf(stderr, "Query execution error: %s\n", mysql_error(conn));
            return;
        }

        MYSQL_RES *result = mysql_store_result(conn); // Retrieve the query result
        if (result == NULL)
        {
            fprintf(stderr, "Error retrieving result set: %s\n", mysql_error(conn));
            return;
        }

        MYSQL_ROW row = mysql_fetch_row(result); // Fetch the result row
        if (row == NULL)
        {
            fprintf(stderr, "Error: Failed to fetch user ID.\n");
            mysql_free_result(result); // Clean up result set
            return;
        }

        current_user.user_id = atoi(row[0]); // Update the global current_user with the fetched user ID
        mysql_free_result(result); // Clean up result set
        secondMenu(current_user, conn); // Proceed to the second menu after successful login
        break;
    case 2: // Incorrect password
        printf("Unable to verify user. Wrong password.\n");
        break;
    case 0: // User does not exist
        printf("Unable to verify. User doesn't exist.\n");
        break;
    default: // An unknown error occurred
        printf("Authentication failure. Unknown error.\n");
        break;
    }

  
}
void createUserProfile(MYSQL *conn)
{
    User user; // Temporary User structure to hold input from user

   while (1) {
       printf("Enter Name (50 characters): ");
       scanf(" %[^\n]", user.name);// Captures full line input including spaces

       printf("Enter Username (200 characters max): ");
       scanf("%199s", user.username);


       // Check if username is unique
       if (!isUsernameUnique(conn, user.username))
       {
           printf("Username is not unique. Please choose a different one.\n");
           continue;// Repeats the loop if username is not unique
       }


       printf("Enter Password (15 characters): ");
       scanf("%14s", user.password);// Ensures input does not exceed buffer size


       if (strlen(user.password) == 0 || strlen(user.password) > 15)
       {
           printf("Invalid password.\n");
           continue;// Ensures password is within the valid range, repeats loop if not
       }


       printf("Enter date of birth (yyyy-mm-dd in the exact format): ");
       scanf("%19s", user.dob);


       if (!dateFormatValid(user.dob))
       {
           printf("Error! Please try to type date again in yyyy-mm-dd format\n");
           continue;
       }


       printf("Enter Phone number (10 digits only): ");
       scanf("%14s", user.phone);


       if (!phoneNumberValid(user.phone))
       {
           printf("Invalid Phone Number. Please enter a 10 digit number.\n");
           continue;
       }


       printf("Enter Email Address (200 characters max): ");
       scanf("%199s", user.email);


       if (isEmailUsed(user.email) > 0)
       {
           printf("Email is already in use. Please choose a different one.\n");
           continue;
       }

    // Call the stored procedure to create a user
    char query[1000];
    sprintf(query, "CALL create_user('%s', '%s', '%s', '%s', '%s', '%s')",
            user.name, user.username, user.password, user.dob, user.email, user.phone);

    // Execute the query
    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Query execution error: %s\n", mysql_error(conn));
        return;
    }
   }

    printf("User profile created successfully!\n");
}

void secondMenu(User current_user, MYSQL *conn)
{
    int choice; // Variable to store the user's menu choice
    do
    {
        // Greet the user and display the menu options
        printf("Welcome %s!\n",current_user.username);
        printf("Current Tasks:\n");
        showUserTasks(conn, current_user); // Display the user's current tasks
        printf("Enter 1: Create Task.\n");
        printf("Enter 2: Manage Task\n");
        printf("Enter 3: Manage Profile\n");
        printf("Enter 4: Manage Reminder\n");
        printf("Enter 5: Exit\n");

        printf("Enter your choice:\n ");
        scanf("%d", &choice);

// Execute the operation based on the user's choice
        switch (choice)
        {
        case 1:
            createTaskForm(conn, current_user);
            break;
        case 2:
            manageTask(conn, current_user);
            break;
        case 3:
            manageProfile(conn,current_user);
            break;
        case 4:
            manageReminders(conn, current_user);
            break;
        case 5:
            exitProgram();
            break;
        default:
            printf("Invalid choice. Please choose a valid option.\n");
        }
    } while (choice != 5);
    return;
}
// Function to authenticate a user based on username and password

int userAuthentication(MYSQL *conn, const char *username, const char *password)
{
    char query[1000]; // Buffer to hold the SQL query

    // Prepare an SQL query to count users matching the provided username and password
    sprintf(query, "SELECT COUNT(*) FROM user WHERE username='%s' AND password='%s'", username, password);

    if (mysql_query(conn, query))
    {
        // If there's an error executing the query, print the error message and return -1
        fprintf(stderr, "Query execution error: %s\n", mysql_error(conn));
        return -1;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL)
    {
        fprintf(stderr, "Error retrieving result set: %s\n", mysql_error(conn));
        return -1;
    }

    int rowCount = 0;  // Variable to store the count of records found

 // If the result set has rows, it means a user with the given username and password exists
    if (mysql_num_rows(result) > 0)
    {
        MYSQL_ROW row = mysql_fetch_row(result); // Fetch the first (and only) row
        rowCount = atoi(row[0]); // Convert the count result to an integer
    }
    mysql_free_result(result);// Free the result set

    if (rowCount > 0)
    {
        return 1; // User verified
    }
    else
    {
        // Check if user exists
        sprintf(query, "SELECT COUNT(*) FROM user WHERE username='%s'", username);

        if (mysql_query(conn, query))
        {
            fprintf(stderr, "Query execution error: %s\n", mysql_error(conn));
            return -1;
        }
        result = mysql_store_result(conn);
        if (result == NULL)
        {
            fprintf(stderr, "Error retrieving result set: %s\n", mysql_error(conn));
            return -1;
        }

        if (mysql_num_rows(result) > 0)
        {
            return 2; // Wrong password
        }
        else
        {
            return 0; // User doesn't exist
        }
    }
}

void manageProfile(MYSQL *conn, User current_user){
    int choice;
    do{
        printf("Manage Profile:\n");
        printf("1. Update Name:\n");
        printf("2. Update Username\n");
        printf("3. Update Password\n");
        printf("4. Update DOB\n");
        printf("5. Update Phone Number\n");
        printf("6. Update Email\n");
        printf("7. Delete Profile\n");
        printf("0. Back to Main Menu\n");
        printf("Enter option below: ");
        scanf("%d", &choice);

        switch (choice){
            case 1:
                updateName(conn, current_user);
                break;
            case 2:
                updateUsername(conn, current_user);
                break;
            case 3:
                updatePassword(conn, current_user);
                break;
            case 4:
                updateDOB(conn, current_user);
                break;
            case 5:
                updatePhoneNum(conn, current_user);
                break;
            case 6:
                updateEmail(conn, current_user);
                break;
            case 7:
                deleteProfile(conn, current_user);
                break;
            case 0:
                printf("Return to Main Menu\n");
                return;
            default:
                printf("Invalid choice. Please try again.\n");

        }
    } while (choice !=6);
    
     return;
 }

 void updateName(MYSQL *conn, User current_user){
    char updatedName[50];

    printf("Enter new name (50 characters max): ");
    scanf(" %[^\n]", updatedName);
    char query[100];
    sprintf(query, "UPDATE user SET name = '%s' WHERE user_id = %d", updatedName, current_user.user_id);
    execute_query(conn,query);
    printf("Name updated successfully!\n");
}

void updateUsername(MYSQL *conn, User current_user){
    char updatedUsername[50];
    int unique = 0;

    while(!unique){
        printf("Enter new username: ");
        scanf("%49s", updatedUsername);

        if (isUsernameUnique(conn, updatedUsername) == 0){
            printf("Username is already in use. Please try again.\n");
        } else {
            unique = 1;
        }
    }
    char query[100];
    sprintf(query, "UPDATE user SET username = '%s' WHERE user_id = %d", updatedUsername, current_user.user_id);
    execute_query(conn,query);
    printf("Username updated successfully!\n");
}

void updatePassword(MYSQL *conn, User current_user){
    char updatedPwd[15];

    printf("Enter new password (15 characters max): ");
    scanf("%14s", updatedPwd);
    char query[100];
    sprintf(query, "UPDATE user SET password = '%s' WHERE user_id = %d", updatedPwd, current_user.user_id);
    execute_query(conn,query);
    printf("Password updated successfully!\n");
}

void updateDOB(MYSQL *conn, User current_user){
    char updatedDOB[20];

    while(true){
        printf("Enter new DOB in this exact format (YYYY-MM-DD): ");
        scanf("%19s", updatedDOB);
        if (dateFormatValid(updatedDOB)){
            break;
        } else {
            printf("Error: Please enter the date in YYYY-MM-DD format\n");
        }
    }
    char query[100];
    sprintf(query, "UPDATE user SET dob = '%s' WHERE user_id = %d", updatedDOB, current_user.user_id);
    execute_query(conn,query);
    printf("DOB was updated successfully!\n");
}

void updatePhoneNum(MYSQL *conn, User current_user){
    char updatedNum[15];

    while(true){
        printf("Enter your new number (10 digits): ");
        scanf("%14s", updatedNum);
        if(phoneNumberValid(updatedNum)){
            break;
        } else {
            printf("Invalid Phone Number. Please try again.\n");
        }
    }
    char query[100];
    sprintf(query, "UPDATE user SET phone_number = '%s' WHERE user_id = %d", updatedNum, current_user.user_id);
    execute_query(conn,query);
    printf("Phone number updated successfully!\n");
}

void updateEmail(MYSQL *conn, User current_user){
    char updatedEmail[200];

    while(true){
        printf("Enter new email address: ");
        scanf("%199s", updatedEmail);
        if (isEmailUsed(updatedEmail) <= 0){
            break;
        } else {
            printf("Email is already in use. Please choose a different one.\n");
        }
    }
    char query[100];
    sprintf(query, "UPDATE user SET email_address = '%s' WHERE user_id = %d", updatedEmail, current_user.user_id);
    execute_query(conn,query);
    printf("Email was updated successfully!\n");
}
void deleteProfile(MYSQL *conn, User current_user) {
    char confirm;
    printf("Are you sure you want to delete your profile? (Y/N): ");
    scanf(" %c", &confirm);

    if (confirm == 'Y' || confirm == 'y') {
        // First, delete task categories associated with the tasks
        char taskCategoryDeleteQuery[1000];
        sprintf(taskCategoryDeleteQuery, "DELETE FROM task_category WHERE task_id IN (SELECT task_id FROM task WHERE user_id = %d)", current_user.user_id);
        execute_query(conn,taskCategoryDeleteQuery);

        // Second, delete tasks associated with the user
        char taskDeleteQuery[1000];
        sprintf(taskDeleteQuery, "DELETE FROM task WHERE user_id = %d", current_user.user_id);
        execute_query(conn,taskDeleteQuery);

        // Now, delete the user
        char userDeleteQuery[1000];
        sprintf(userDeleteQuery, "DELETE FROM user WHERE user_id = %d", current_user.user_id);
        execute_query(conn,userDeleteQuery);

        printf("Your profile has been deleted.\n");

        // Return the user to the main menu
        int choice;
        do {
            printf("Enter 1 to create a profile.\n");
            printf("Enter 2 to sign into profile.\n");
            printf("Enter your choice: ");
            scanf("%d", &choice);

            switch (choice) {
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
    } else {
        printf("Profile deletion canceled.\n");
    }
}



int isUsernameUnique(MYSQL *conn, const char *username)
{
    if (username == NULL || username[0] == '\0' || strlen(username) > 200)
    {
        printf("Invalid username.\n");
        return -1;
    }

    char query[10000];
    sprintf(query, "SELECT username FROM user WHERE username = '%s'", username);

    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Query execution error: %s\n", mysql_error(conn));
        return -1;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL)
    {
        fprintf(stderr, "Error retrieving result set: %s\n", mysql_error(conn));
        return -1;
    }

    int num_rows = mysql_num_rows(result);
    mysql_free_result(result);

    return (num_rows == 0) ? 1 : 0;
}
int isEmailUsed(char *email)
{
    MYSQL *conn = mysql_init(NULL);

    if (conn == NULL)
    {
        fprintf(stderr, "mysql_init() failed\n");
        return -1;
    }

    if (mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASSWORD, DB_NAME, 0, NULL, 0) == NULL)
    {
        fprintf(stderr, "mysql_real_connect() failed\n");
        mysql_close(conn);
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM user WHERE email_address = '%s'", email);

    if (mysql_query(conn, query))
    {
        fprintf(stderr, "Query execution error: %s\n", mysql_error(conn));
        mysql_close(conn);
        return -1;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL)
    {
        fprintf(stderr, "mysql_store_result() failed\n");
        mysql_close(conn);
        return -1;
    }

    int rowCount = 0;

    if (mysql_num_rows(result) > 0)
    {
        MYSQL_ROW row = mysql_fetch_row(result);
        rowCount = atoi(row[0]);
    }
    mysql_free_result(result);
    mysql_close(conn);
    return rowCount;
}

bool dateFormatValid(char *date)
{

    // Checks to make sure the date is the right length
    if (strlen(date) != 10)
    {
        return false;
    }

    // checks to make sure it is the correct format yyyy-mm-dd
    for (int i = 0; i < 10; i++)
    {
        if (i == 4 || i == 7)
        {
            // checking for '-'
            if (date[i] != '-')
            {
                return false;
            }
        }
        else
        {
            if (!isdigit(date[i]))
            {
                return false;
            }
        }
    }
    return true;
}
bool timeFormatValid(const char *time)
{
    // Checks to make sure the time is in the correct format (hh:mm:ss)
    if (strlen(time) != 8)
    {
        return false;
    }

    // Checks each character of the time string
    for (int i = 0; i < 8; i++)
    {
        // Checks for ':' separator at appropriate positions
        if (i == 2 || i == 5)
        {
            if (time[i] != ':')
            {
                return false; // Incorrect separator
            }
        }
        else
        {
            // Checks if each character is a digit
            if (!isdigit(time[i]))
            {
                return false; // Non-numeric character found
            }
        }
    }

    return true; // Time format is valid
}

bool phoneNumberValid(const char *phoneNumber)
{
    // Phone number must be exactly 10 digits
    if (strlen(phoneNumber) != 10)
    {
        return false;
    }

    // Check if all characters are digits
    for (int i = 0; i < 10; i++)
    {
        if (!isdigit(phoneNumber[i]))
        {
            return false;
        }
    }
    return true;
} 
void exitProgram()
{
    return;
}