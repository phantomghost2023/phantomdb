#!/usr/bin/env python3
"""
Script to update progress in the PROJECT_TRACKING.md file
based on completed tasks.
"""

import re
import sys

def update_progress(tracking_file, section, task, status):
    """
    Update the progress of a specific task in the tracking file.
    
    Args:
        tracking_file (str): Path to the tracking file
        section (str): Section header (e.g., "Research & Design")
        task (str): Task description
        status (str): New status ("[x]" for complete, "[ ]" for incomplete)
    """
    try:
        with open(tracking_file, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Find the section
        section_pattern = f"(## {section}\\n)"
        section_match = re.search(section_pattern, content)
        
        if not section_match:
            print(f"Section '{section}' not found")
            return False
        
        # Find the task within the section
        # This is a simplified approach - in practice, you might want more robust parsing
        task_pattern = f"(- \\[ \\] {task})"
        task_match = re.search(task_pattern, content)
        
        if not task_match:
            # Try with completed marker
            task_pattern = f"(- \\[x\\] {task})"
            task_match = re.search(task_pattern, content)
            
        if not task_match:
            print(f"Task '{task}' not found")
            return False
        
        # Update the task status
        old_task = task_match.group(1)
        new_status = "[x]" if status.lower() == "complete" else "[ ]"
        new_task = old_task.replace("[ ]", new_status).replace("[x]", new_status)
        
        content = content.replace(old_task, new_task)
        
        with open(tracking_file, 'w', encoding='utf-8') as f:
            f.write(content)
        
        print(f"Updated task '{task}' to {status}")
        return True
        
    except Exception as e:
        print(f"Error updating progress: {e}")
        return False

def main():
    if len(sys.argv) < 4:
        print("Usage: python update-progress.py <section> <task> <status>")
        print("Example: python update-progress.py \"Research & Design\" \"Study failure points in existing databases (SQL/NoSQL)\" complete")
        sys.exit(1)
    
    section = sys.argv[1]
    task = sys.argv[2]
    status = sys.argv[3]
    
    tracking_file = "docs/PROJECT_TRACKING.md"
    update_progress(tracking_file, section, task, status)

if __name__ == "__main__":
    main()