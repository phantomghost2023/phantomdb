#!/usr/bin/env python3
"""
Verification script to check if PhantomDB is ready for building
"""

import os
import sys

def check_directory_exists(path, name):
    """Check if a directory exists"""
    if os.path.exists(path) and os.path.isdir(path):
        print(f"✅ {name} directory found")
        return True
    else:
        print(f"❌ {name} directory missing")
        return False

def check_file_exists(path, name):
    """Check if a file exists"""
    if os.path.exists(path) and os.path.isfile(path):
        print(f"✅ {name} found")
        return True
    else:
        print(f"❌ {name} missing")
        return False

def main():
    """Main verification function"""
    print("=== PhantomDB Build Readiness Verification ===\n")
    
    # Get the project root directory
    project_root = os.path.dirname(os.path.abspath(__file__))
    print(f"Project root: {project_root}\n")
    
    # Check required directories
    required_dirs = [
        ("src/core", "Core module"),
        ("src/storage", "Storage module"),
        ("src/query", "Query module"),
        ("src/transaction", "Transaction module"),
        ("src/api", "API module"),
        ("src/tools", "Tools module"),
        ("src/testing", "Testing module"),
        ("src/audit", "Audit module"),
        ("src/security", "Security module"),
        ("src/plugin", "Plugin module"),
        ("src/import_export", "Import/Export module"),
        ("src/distributed", "Distributed module"),
        ("tests", "Tests directory")
    ]
    
    all_dirs_present = True
    for dir_path, dir_name in required_dirs:
        full_path = os.path.join(project_root, dir_path)
        if not check_directory_exists(full_path, dir_name):
            all_dirs_present = False
    
    print()
    
    # Check required files
    required_files = [
        ("CMakeLists.txt", "Main CMake configuration"),
        ("tests/CMakeLists.txt", "Tests CMake configuration"),
        ("src/testing/CMakeLists.txt", "Testing CMake configuration"),
        ("build.bat", "Windows build script"),
        ("scripts/build.sh", "Unix build script")
    ]
    
    all_files_present = True
    for file_path, file_name in required_files:
        full_path = os.path.join(project_root, file_path)
        if not check_file_exists(full_path, file_name):
            all_files_present = False
    
    print()
    
    # Final assessment
    if all_dirs_present and all_files_present:
        print("🎉 ALL CHECKS PASSED - PhantomDB is ready for building! 🎉")
        print("\nYou can now build PhantomDB using:")
        print("  Windows: .\\build.bat")
        print("  Unix: ./scripts/build.sh")
        return 0
    else:
        print("❌ SOME CHECKS FAILED - Please review the missing components above")
        return 1

if __name__ == "__main__":
    sys.exit(main())