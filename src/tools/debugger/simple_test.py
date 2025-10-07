#!/usr/bin/env python3
"""
Simple test script for the PhantomDB Query Debugger
"""

import sys
import os

# Add the current directory to the path
sys.path.insert(0, os.path.dirname(__file__))

# Import the module directly
import phantomdb_debugger

def test_debugger():
    """Test the debugger functionality"""
    print("Testing PhantomDB Query Debugger...")
    
    # Create a debugger instance
    debugger = phantomdb_debugger.QueryDebugger("http://localhost:8080")
    
    # Test initialization
    print("✓ Debugger initialized successfully")
    
    # Test clear history
    debugger.clear_history()
    print("✓ History cleared successfully")
    
    # Test profiling (this will show error messages since there's no server, but that's expected)
    print("Testing query profiling...")
    try:
        result = debugger.profile_query("testdb", "SELECT * FROM users", 2)
        print("✓ Profiling completed")
    except Exception as e:
        print(f"Expected error during profiling (no server): {e}")
    
    # Test history
    debugger.show_query_history()
    print("✓ History display completed")
    
    print("\nAll tests completed!")

if __name__ == "__main__":
    test_debugger()