#!/bin/bash

# Path to the file storing task entries 

TASK_LOG="$HOME/Desktop/Capstone/obj/task_log.txt" 

 

# Function to record the start time of a task 

start_task() { 

   task_name="$1" 

   start_time=$(date "+%Y-%m-%d %H:%M:%S") 

   echo "$task_name|$start_time" >> "$TASK_LOG" 

   echo "Task '$task_name' started at $start_time." 

} 

 

# Function to record the end time of a task 

end_task() { 

   task_name="$1" 

   end_time=$(date '+%Y-%m-%d %H:%M:%S') 

   echo "$task_name|$end_time" >> "$TASK_LOG" 

   echo "Task '$task_name' ended at $end_time." 

} 

generate_report() { 

    echo "Task Report:" 

    echo "----------" 

 

    # Loop through each line in the log file 

    while IFS='|' read -r task_name timestamp action; do 

        # Check if the line is empty or contains invalid data 

        if [ -z "$task_name" ] || [ -z "$timestamp" ] || [ -z "$action" ]; then 

            continue 

        fi 

 

        # Ignore lines that don't end with "start" or "end" 

        if [ "$action" != "start" ] && [ "$action" != "end" ]; then 

            continue 

        fi 

 

        # Check if the action is "start" and record the start time 

        if [ "$action" == "start" ]; then 

            start_time="$timestamp" 

        fi 

 

        # Check if the action is "end" and calculate duration if start time exists 

        if [ "$action" == "end" ] && [ -n "$start_time" ]; then 

            end_time="$timestamp" 

            start_seconds=$(date -d "$start_time" '+%s') 

            end_seconds=$(date -d "$end_time" '+%s') 

            duration=$((end_seconds - start_seconds)) 

 

            printf "Task: %-20s Duration: %02d:%02d:%02d\n" "$task_name" $((duration / 3600)) $(((duration % 3600) / 60)) $((duration % 60)) 

 

            # Reset start time for the next task 

            start_time="" 

        fi 

    done < "$TASK_LOG" 

} 
