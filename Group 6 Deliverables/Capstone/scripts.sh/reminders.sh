#!/bin/bash


# Database connection parameters
DB_HOST="localhost"
DB_USER="root"
DB_PASS="student"
DB_NAME="task_manager"


# Function to execute a MySQL query
execute_query() {
   local query="$1"
   mysql -h "$DB_HOST" -u "$DB_USER" -p"$DB_PASS" -D "$DB_NAME" -e "$query"
}


# Function to send reminders via email
send_reminder() {
   local task_id="$1"
   local reminder_time="$2"
   local email_address="$3"


   echo "Reminder: Task #$task_id is scheduled for $reminder_time" | /usr/bin/mail -s "Task Reminder" "$email_address"
}


# Fetch tasks from the database
tasks=$(execute_query "SELECT u.email_address, tr.task_id, tr.reminder_time
FROM user u
JOIN task_reminder tr ON u.user_id = tr.user_id
JOIN task t ON tr.task_id = t.task_id WHERE tr.reminder_time > NOW()")


# Loop through tasks and schedule reminders
while read -r email_address task_id reminder_time; do
   echo "Sending reminder for: Task #$task_id at $reminder_time to $email_address"
   send_reminder "$task_id" "$reminder_time" "$email_address"
done <<< "$tasks"