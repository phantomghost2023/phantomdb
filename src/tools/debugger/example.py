#!/usr/bin/env python3
"""
PhantomDB Query Debugger Examples

Examples demonstrating how to use the PhantomDB Query Debugger and Profiler.
"""

import sys
import os

# Add the debugger directory to the path so we can import it
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'debugger'))

from debugger.phantomdb_debugger import QueryDebugger


def example_basic_debugging():
    """Example of basic query debugging."""
    print("=== Basic Query Debugging Example ===")
    
    # Create a debugger instance
    debugger = QueryDebugger("http://localhost:8080")
    
    # Execute a query with debugging information
    result = debugger.execute_query("myapp", "SELECT * FROM users WHERE age > 18", debug=True)
    
    print("Query result:")
    print(result)
    print()


def example_query_profiling():
    """Example of query profiling."""
    print("=== Query Profiling Example ===")
    
    # Create a debugger instance
    debugger = QueryDebugger("http://localhost:8080")
    
    # Profile a query by running it multiple times
    profile_result = debugger.profile_query("myapp", "SELECT * FROM users WHERE id = 1", iterations=5)
    
    print("Profiling result:")
    print(f"Average execution time: {profile_result.get('average_time_ms', 0):.2f} ms")
    print(f"Minimum execution time: {profile_result.get('min_time_ms', 0):.2f} ms")
    print(f"Maximum execution time: {profile_result.get('max_time_ms', 0):.2f} ms")
    print()


def example_query_explanation():
    """Example of query explanation."""
    print("=== Query Explanation Example ===")
    
    # Create a debugger instance
    debugger = QueryDebugger("http://localhost:8080")
    
    # Explain a query execution plan
    explanation = debugger.explain_query("myapp", "SELECT u.name, o.total FROM users u JOIN orders o ON u.id = o.user_id")
    
    print("Query explanation:")
    print(explanation)
    print()


def example_query_history():
    """Example of query history tracking."""
    print("=== Query History Example ===")
    
    # Create a debugger instance
    debugger = QueryDebugger("http://localhost:8080")
    
    # Execute a few queries
    debugger.execute_query("myapp", "SELECT * FROM users", debug=True)
    debugger.execute_query("myapp", "SELECT * FROM orders", debug=True)
    debugger.execute_query("myapp", "SELECT COUNT(*) FROM users", debug=True)
    
    # Show query history
    debugger.show_query_history()
    print()


if __name__ == "__main__":
    print("PhantomDB Query Debugger Examples")
    print("=" * 40)
    
    # Run examples
    example_basic_debugging()
    example_query_profiling()
    example_query_explanation()
    example_query_history()
    
    print("All examples completed!")