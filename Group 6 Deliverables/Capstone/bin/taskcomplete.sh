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

 

# Path to the file storing task statuses 

TASK_LOG="$HOME/Desktop/Capstone/obj/task_log.txt" 

 

# Function to mark task as complete 

mark_task_complete() { 

    local task_name="$1" 

 

    # Check if the task exists in the file 

    if grep -q "$task_name" "$TASK_LOG"; then 

        # Mark the task as complete by adding a timestamp 

        sed -i "/$task_name/ s/$/ -$(date '+%Y-%m-%d %H:%M:%S')/" "$TASK_LOG" 

        echo "Task '$task_name' marked as complete." 

 

        # Execute SQL query to mark task as complete in the database 

        execute_query "UPDATE tasks SET status='completed' WHERE name='$task_name'" 

    else 

        echo "Error: Task '$task_name' not found." 

    fi 

} 

 

# Example usage: mark_task_complete "Task1" 

