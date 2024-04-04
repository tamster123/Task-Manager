#!/bin/bash 

 

 

# Path to the file storing task entries 

TASK_LOG="$HOME/Desktop/Capstone/task_log.txt" 

 

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

 

# Function to generate a report on time spent 

generate_report() { 

    echo "Task Report:" 

    echo "----------" 

 

    while IFS='|' read -r task_name start_time; do 

        end_time=$(grep "$task_name" "$TASK_LOG" | tail -n 1 | cut -d '|' -f 2) 

        if [ -n "$end_time" ]; then 

            start_seconds=$(date -d "$start_time" '+%s') 

            end_seconds=$(date -d "$end_time" '+%s') 

            duration=$((end_seconds - start_seconds)) 

 

            printf "Task: %-20s Duration: %02d:%02d:%02d\n" "$task_name" $((duration / 3600)) $(((duration % 3600) / 60)) $((duration % 60)) 

        fi 

    done < <(sort "$TASK_LOG" | uniq) 

} 

 

# Function to show future scheduled tasks 

show_scheduled_tasks() { 

    echo "Future Scheduled Tasks:" 

    echo "------------------------" 

    cut -d '|' -f 1 "$TASK_LOG" | sort | uniq 

} 

 

# Function to prompt user for task selection 

select_task() { 

    echo "Please select a task (1-19):" 

    read task_number 

    case $task_number in 

        1) start_task "Task1" ;; 

        2) start_task "Task2" ;; 

        3) start_task "Task3" ;; 

        4) start_task "Task4" ;; 

        5) start_task "Task5" ;; 

        6) start_task "Task6" ;; 

        7) start_task "Task7" ;; 

        8) start_task "Task8" ;; 

        9) start_task "Task9" ;; 

        10) start_task "Task10" ;; 

        11) start_task "Task11" ;; 

        12) start_task "Task12" ;; 

        13) start_task "Task13" ;; 

        14) start_task "Task14" ;; 

        15) start_task "Task15" ;; 

        16) start_task "Task16" ;; 

        17) start_task "Task17" ;; 

        18) start_task "Task18" ;; 

        19) start_task "Task19" ;; 

        *) echo "Invalid selection. Please choose a number between 1 and 19." ;; 

    esac 

} 

 

# Prompt user for task selection 

select_task 

 

# Show scheduled tasks 

#show_scheduled_tasks 

 

# Generate report 

generate_report 