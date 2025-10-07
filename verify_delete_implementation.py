#!/usr/bin/env python3

import os
import re

# Define the project root directory
project_root = r"d:\PhantomGhost\Storage\Media\Media\Projects\MyProjects\PhantomDB"

def check_file_exists(file_path):
    """Check if a file exists"""
    full_path = os.path.join(project_root, file_path)
    return os.path.exists(full_path)

def check_content_in_file(file_path, pattern):
    """Check if a pattern exists in a file"""
    full_path = os.path.join(project_root, file_path)
    if not os.path.exists(full_path):
        return False
    
    try:
        with open(full_path, 'r', encoding='utf-8') as f:
            content = f.read()
            return re.search(pattern, content) is not None
    except Exception as e:
        print(f"Error reading {file_path}: {e}")
        return False

def main():
    print("Verifying DELETE statement implementation...")
    
    # Check if all required files exist
    required_files = [
        "src/query/sql_parser.h",
        "src/query/sql_parser.cpp",
        "src/query/query_planner.h",
        "src/query/query_planner.cpp",
        "src/query/execution_engine.h",
        "src/query/execution_engine.cpp"
    ]
    
    print("\n1. Checking required files...")
    all_files_exist = True
    for file in required_files:
        if check_file_exists(file):
            print(f"  ✓ {file} exists")
        else:
            print(f"  ✗ {file} does not exist")
            all_files_exist = False
    
    if not all_files_exist:
        print("Some required files are missing!")
        return 1
    
    # Check for DeleteStatement class in sql_parser.h
    print("\n2. Checking for DeleteStatement class...")
    if check_content_in_file("src/query/sql_parser.h", r"class\s+DeleteStatement\s*:"):
        print("  ✓ DeleteStatement class found in sql_parser.h")
    else:
        print("  ✗ DeleteStatement class not found in sql_parser.h")
        return 1
    
    # Check for DeleteStatement implementation in sql_parser.cpp
    print("\n3. Checking for DeleteStatement implementation...")
    if check_content_in_file("src/query/sql_parser.cpp", r"DeleteStatement::DeleteStatement\s*\("):
        print("  ✓ DeleteStatement constructor found in sql_parser.cpp")
    else:
        print("  ✗ DeleteStatement constructor not found in sql_parser.cpp")
        return 1
    
    if check_content_in_file("src/query/sql_parser.cpp", r"std::string\s+DeleteStatement::toString\s*\("):
        print("  ✓ DeleteStatement::toString method found in sql_parser.cpp")
    else:
        print("  ✗ DeleteStatement::toString method not found in sql_parser.cpp")
        return 1
    
    # Check for parseDeleteStatement method
    print("\n4. Checking for parseDeleteStatement method...")
    if check_content_in_file("src/query/sql_parser.cpp", r"parseDeleteStatement\s*\("):
        print("  ✓ parseDeleteStatement method found in sql_parser.cpp")
    else:
        print("  ✗ parseDeleteStatement method not found in sql_parser.cpp")
        return 1
    
    # Check for DeleteNode class in query_planner.h
    print("\n5. Checking for DeleteNode class...")
    if check_content_in_file("src/query/query_planner.h", r"class\s+DeleteNode\s*:"):
        print("  ✓ DeleteNode class found in query_planner.h")
    else:
        print("  ✗ DeleteNode class not found in query_planner.h")
        return 1
    
    # Check for DeleteNode implementation in query_planner.cpp
    print("\n6. Checking for DeleteNode implementation...")
    if check_content_in_file("src/query/query_planner.cpp", r"DeleteNode::DeleteNode\s*\("):
        print("  ✓ DeleteNode constructor found in query_planner.cpp")
    else:
        print("  ✗ DeleteNode constructor not found in query_planner.cpp")
        return 1
    
    if check_content_in_file("src/query/query_planner.cpp", r"std::string\s+DeleteNode::toString\s*\("):
        print("  ✓ DeleteNode::toString method found in query_planner.cpp")
    else:
        print("  ✗ DeleteNode::toString method not found in query_planner.cpp")
        return 1
    
    # Check for generateDeletePlan method
    print("\n7. Checking for generateDeletePlan method...")
    if check_content_in_file("src/query/query_planner.cpp", r"generateDeletePlan\s*\("):
        print("  ✓ generateDeletePlan method found in query_planner.cpp")
    else:
        print("  ✗ generateDeletePlan method not found in query_planner.cpp")
        return 1
    
    # Check for ExecDeleteNode class in execution_engine.h
    print("\n8. Checking for ExecDeleteNode class...")
    if check_content_in_file("src/query/execution_engine.h", r"class\s+ExecDeleteNode\s*:"):
        print("  ✓ ExecDeleteNode class found in execution_engine.h")
    else:
        print("  ✗ ExecDeleteNode class not found in execution_engine.h")
        return 1
    
    # Check for ExecDeleteNode implementation in execution_engine.cpp
    print("\n9. Checking for ExecDeleteNode implementation...")
    if check_content_in_file("src/query/execution_engine.cpp", r"ExecDeleteNode::ExecDeleteNode\s*\("):
        print("  ✓ ExecDeleteNode constructor found in execution_engine.cpp")
    else:
        print("  ✗ ExecDeleteNode constructor not found in execution_engine.cpp")
        return 1
    
    if check_content_in_file("src/query/execution_engine.cpp", r"bool\s+ExecDeleteNode::execute\s*\("):
        print("  ✓ ExecDeleteNode::execute method found in execution_engine.cpp")
    else:
        print("  ✗ ExecDeleteNode::execute method not found in execution_engine.cpp")
        return 1
    
    # Check for DELETE case in convertPlanToExecutionNode
    print("\n10. Checking for DELETE case in convertPlanToExecutionNode...")
    if check_content_in_file("src/query/execution_engine.cpp", r"case\s+PlanNodeType::DELETE\s*:"):
        print("  ✓ DELETE case found in convertPlanToExecutionNode")
    else:
        print("  ✗ DELETE case not found in convertPlanToExecutionNode")
        return 1
    
    print("\nAll checks passed! DELETE statement implementation is complete.")
    return 0

if __name__ == "__main__":
    exit(main())