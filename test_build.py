#!/usr/bin/env python3
"""
Test script to build and verify PhantomDB
"""

import os
import subprocess
import sys
import platform

def run_command(cmd, shell=False):
    """Run a command and return the result"""
    print(f"Running: {cmd}")
    try:
        result = subprocess.run(cmd, shell=shell, capture_output=True, text=True, check=True)
        print(result.stdout)
        return True
    except subprocess.CalledProcessError as e:
        print(f"Error: {e}")
        print(f"stderr: {e.stderr}")
        return False

def main():
    """Main test function"""
    print("=== PhantomDB Build and Test Script ===")
    
    # Get the current directory
    project_dir = os.path.dirname(os.path.abspath(__file__))
    print(f"Project directory: {project_dir}")
    
    # Create build directory
    build_dir = os.path.join(project_dir, "build")
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)
    
    # Change to build directory
    os.chdir(build_dir)
    
    # Configure with CMake
    print("\n--- Configuring with CMake ---")
    cmake_path = r"C:\Program Files\CMake\bin\cmake.exe"
    if platform.system() == "Windows":
        if not run_command([cmake_path, ".."]):
            print("CMake configuration failed")
            return 1
    else:
        if not run_command(["cmake", ".."]):
            print("CMake configuration failed")
            return 1
    
    # Build the project
    print("\n--- Building Project ---")
    if platform.system() == "Windows":
        if not run_command([cmake_path, "--build", "."]):
            print("Build failed")
            return 1
    else:
        if not run_command(["cmake", "--build", "."]):
            print("Build failed")
            return 1
    
    # Check if executable was created
    print("\n--- Checking Build Output ---")
    if platform.system() == "Windows":
        exe_path = os.path.join(build_dir, "Debug", "phantomdb.exe")
        if not os.path.exists(exe_path):
            exe_path = os.path.join(build_dir, "phantomdb.exe")
    else:
        exe_path = os.path.join(build_dir, "phantomdb")
    
    if os.path.exists(exe_path):
        print(f"SUCCESS: Executable created at {exe_path}")
    else:
        print("ERROR: Executable not found")
        # List files in build directory
        print("Files in build directory:")
        for root, dirs, files in os.walk(build_dir):
            for file in files:
                print(f"  {os.path.join(root, file)}")
        return 1
    
    print("\n=== Build Test Completed Successfully ===")
    return 0

if __name__ == "__main__":
    sys.exit(main())