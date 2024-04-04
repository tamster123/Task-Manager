#ifndef REMINDER_H
#define REMINDER_H

#include <mysql/mysql.h>
#include "profile.h" 


// Manage reminders for the current user, such as viewing, creating, updating, or deleting reminders
void manageReminders(MYSQL *conn, User current_user);

// Display all reminders associated with the current user
void showUserReminders(MYSQL *conn, User current_user);

/* Create a new reminder and associate it with a user and a task
 - user_id: The ID of the user to whom the reminder belongs
 - task_id: The ID of the task to which the reminder is related
 - message: The reminder's message
 - status: The reminder's status (e.g., "pending", "completed")
- year, month, day, hour, minute, second: The scheduled time for the reminder */
void createReminder(MYSQL *conn, int user_id, int task_id, const char *message, const char *status, int year, int month, int day, int hour, int minute, int second);

/* Update an existing reminder's details
 - reminder_id: The ID of the reminder to be updated
 - task_id: The new task ID to associate the reminder with (if changing the task)
 - message: The new reminder message
 - status: The new reminder status
 - year, month, day, hour, minute, second: The new scheduled time for the reminder */
void updateReminder(MYSQL *conn, int reminder_id, int task_id, const char *message, const char *status, int year, int month, int day, int hour, int minute, int second);

// Delete a reminder from the system
// - reminder_id: The ID of the reminder to be deleted
void deleteReminder(MYSQL *conn, int reminder_id);

#endif