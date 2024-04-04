#ifndef PROFILE_H
#define PROFILE_H

#include <mysql/mysql.h> 
#include "database.h"
#include <ctype.h>

// Define a structure for a user's profile information
typedef struct {
    int user_id;            // Unique identifier for the user
    char name[50];          // User's full name
    char username[200];     // User's chosen username
    char password[15];      // User's password
    char dob[20];           // User's date of birth
    char phone[15];         // User's phone number
    char email[200];        // User's email address
} User;

// Declaration for a global variable that holds the current user's information
extern User current_user;

// Function prototypes for user-related operations:

// Create a new user profile in the database
void createUserProfile(MYSQL *conn);

// Sign in a user by verifying their credentials
void signIn(MYSQL *conn);

// Display the second menu after the user is signed in
void secondMenu(User current_user, MYSQL *conn);

// Authenticate a user's username and password
int userAuthentication(MYSQL *conn, const char *username, const char *password);

// Manage the current user's profile, allowing them to edit their details
void manageProfile(MYSQL *conn, User current_user);

// Update the current user's username in the database
void updateUsername(MYSQL *conn, User current_user);

// Update the current user's name in the database
void updateName(MYSQL *conn, User current_user);

// Update the current user's password in the database
void updatePassword(MYSQL *conn, User current_user);

// Update the current user's date of birth in the database
void updateDOB(MYSQL *conn, User current_user);

// Update the current user's phone number in the database
void updatePhoneNum(MYSQL *conn, User current_user);

// Update the current user's email address in the database
void updateEmail(MYSQL *conn, User current_user);

// Delete the current user's profile from the database
void deleteProfile(MYSQL *conn, User current_user);

// Check if the given username is unique in the database
int isUsernameUnique(MYSQL *conn, const char *username);

// Check if the given email is already used by another user in the database
int isEmailUsed(char *email);

// Validate the format of a date string (e.g., "YYYY-MM-DD")
bool dateFormatValid(char *date);

// Validate the format of a phone number string (assumes 10 digits)
bool phoneNumberValid(const char *phoneNumber);

// Validate the format of a time string (e.g., "HH:MM:SS")
bool timeFormatValid(const char *time);

// Exit the program
void exitProgram();

#endif // PROFILE_H