#!/usr/bin/env python3
"""
PhantomDB Query Debugger and Profiler

A tool for debugging and profiling SQL queries in PhantomDB.
"""

import argparse
import sys
import json
import time
from typing import List, Dict, Any, Optional
import requests
from datetime import datetime


class QueryDebugger:
    """Query debugger and profiler for PhantomDB."""
    
    def __init__(self, base_url: str = "http://localhost:8080"):
        """Initialize the debugger with the base URL of the PhantomDB server."""
        self.base_url = base_url.rstrip('/')
        self.session = requests.Session()
        self.query_history = []
    
    def _make_request(self, method: str, endpoint: str, 
                     data: Optional[Dict[str, Any]] = None) -> Dict[str, Any]:
        """Make an HTTP request to the PhantomDB REST API."""
        url = f"{self.base_url}{endpoint}"
        
        try:
            start_time = time.time()
            if data is not None:
                response = self.session.request(
                    method, url, 
                    json=data,
                    headers={'Content-Type': 'application/json'}
                )
            else:
                response = self.session.request(method, url)
            end_time = time.time()
                
            response.raise_for_status()
            result = response.json() if response.content else {}
            
            # Record timing information
            execution_time = (end_time - start_time) * 1000  # Convert to milliseconds
            result['execution_time_ms'] = execution_time
            
            return result
            
        except requests.exceptions.RequestException as e:
            print(f"Error: {e}")
            return {"error": str(e)}
        except json.JSONDecodeError as e:
            print(f"Error decoding JSON response: {e}")
            return {"error": str(e)}
    
    def execute_query(self, database: str, query: str, debug: bool = False) -> Dict[str, Any]:
        """Execute a query with debugging information."""
        print(f"Executing query: {query}")
        
        # Record query start time
        start_timestamp = datetime.now().isoformat()
        
        # Execute the query
        data = {"query": query}
        result = self._make_request("POST", f"/databases/{database}/query", data)
        
        # Record query end time
        end_timestamp = datetime.now().isoformat()
        
        # Add debugging information
        if debug:
            debug_info = {
                "query": query,
                "database": database,
                "start_time": start_timestamp,
                "end_time": end_timestamp,
                "execution_time_ms": result.get('execution_time_ms', 0),
                "result_size": len(str(result)) if 'error' not in result else 0
            }
            
            # Store in query history
            self.query_history.append(debug_info)
            
            # Print debugging information
            print(f"Query executed in {debug_info['execution_time_ms']:.2f} ms")
            print(f"Start time: {debug_info['start_time']}")
            print(f"End time: {debug_info['end_time']}")
            print(f"Result size: {debug_info['result_size']} bytes")
        
        return result
    
    def profile_query(self, database: str, query: str, iterations: int = 10) -> Dict[str, Any]:
        """Profile a query by executing it multiple times."""
        print(f"Profiling query: {query}")
        print(f"Running {iterations} iterations...")
        
        execution_times = []
        results = []
        
        for i in range(iterations):
            print(f"  Iteration {i+1}/{iterations}")
            data = {"query": query}
            result = self._make_request("POST", f"/databases/{database}/query", data)
            
            if 'execution_time_ms' in result:
                execution_times.append(result['execution_time_ms'])
                results.append(result)
        
        if execution_times:
            # Calculate statistics
            avg_time = sum(execution_times) / len(execution_times)
            min_time = min(execution_times)
            max_time = max(execution_times)
            
            profile_result = {
                "query": query,
                "iterations": iterations,
                "average_time_ms": avg_time,
                "min_time_ms": min_time,
                "max_time_ms": max_time,
                "execution_times": execution_times
            }
            
            print(f"Profiling complete:")
            print(f"  Average execution time: {avg_time:.2f} ms")
            print(f"  Minimum execution time: {min_time:.2f} ms")
            print(f"  Maximum execution time: {max_time:.2f} ms")
            
            return profile_result
        else:
            return {"error": "No execution times recorded"}
    
    def explain_query(self, database: str, query: str) -> Dict[str, Any]:
        """Get query execution plan (simulated)."""
        print(f"Explaining query: {query}")
        
        # In a real implementation, this would return the actual execution plan
        # For now, we'll simulate it
        explanation = {
            "query": query,
            "plan": {
                "operation": "SELECT",
                "table": "users",
                "columns": ["id", "name", "email"],
                "filters": [],
                "estimated_rows": 1000,
                "cost": 15.5
            }
        }
        
        print("Query execution plan:")
        print(json.dumps(explanation['plan'], indent=2))
        
        return explanation
    
    def show_query_history(self) -> None:
        """Show query execution history."""
        if not self.query_history:
            print("No query history available.")
            return
        
        print("Query History:")
        print("-" * 80)
        for i, query_info in enumerate(self.query_history, 1):
            print(f"{i}. Query: {query_info['query'][:50]}...")
            print(f"   Database: {query_info['database']}")
            print(f"   Execution time: {query_info['execution_time_ms']:.2f} ms")
            print(f"   Time: {query_info['start_time']}")
            print(f"   Result size: {query_info['result_size']} bytes")
            print("-" * 80)
    
    def clear_history(self) -> None:
        """Clear query history."""
        self.query_history = []
        print("Query history cleared.")


def main():
    """Main entry point for the PhantomDB Query Debugger."""
    parser = argparse.ArgumentParser(
        description="PhantomDB Query Debugger and Profiler",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  phantomdb-debugger.py execute mydb "SELECT * FROM users"
  phantomdb-debugger.py profile mydb "SELECT * FROM users WHERE id = 1"
  phantomdb-debugger.py explain mydb "SELECT * FROM users"
  phantomdb-debugger.py history
        """
    )
    
    parser.add_argument(
        "--url",
        default="http://localhost:8080",
        help="Base URL of the PhantomDB server (default: http://localhost:8080)"
    )
    
    subparsers = parser.add_subparsers(dest="command", help="Available commands")
    
    # Execute command
    execute_parser = subparsers.add_parser("execute", help="Execute a query with debugging info")
    execute_parser.add_argument("database", help="Name of the database")
    execute_parser.add_argument("query", help="SQL query to execute")
    execute_parser.add_argument("--debug", action="store_true", help="Enable debugging output")
    
    # Profile command
    profile_parser = subparsers.add_parser("profile", help="Profile a query")
    profile_parser.add_argument("database", help="Name of the database")
    profile_parser.add_argument("query", help="SQL query to profile")
    profile_parser.add_argument("--iterations", type=int, default=10, 
                              help="Number of iterations (default: 10)")
    
    # Explain command
    explain_parser = subparsers.add_parser("explain", help="Explain query execution plan")
    explain_parser.add_argument("database", help="Name of the database")
    explain_parser.add_argument("query", help="SQL query to explain")
    
    # History command
    subparsers.add_parser("history", help="Show query execution history")
    
    # Clear history command
    subparsers.add_parser("clear-history", help="Clear query execution history")
    
    args = parser.parse_args()
    
    if not args.command:
        parser.print_help()
        return
    
    debugger = QueryDebugger(args.url)
    
    # Execute the appropriate command
    if args.command == "execute":
        result = debugger.execute_query(args.database, args.query, args.debug)
        if "error" in result:
            print(f"Query execution failed: {result['error']}")
        else:
            print("Query result:")
            print(json.dumps(result, indent=2))
    elif args.command == "profile":
        result = debugger.profile_query(args.database, args.query, args.iterations)
        if "error" in result:
            print(f"Query profiling failed: {result['error']}")
        else:
            print("Profiling result:")
            print(json.dumps(result, indent=2))
    elif args.command == "explain":
        result = debugger.explain_query(args.database, args.query)
        if "error" in result:
            print(f"Query explanation failed: {result['error']}")
        else:
            print("Explanation result:")
            print(json.dumps(result, indent=2))
    elif args.command == "history":
        debugger.show_query_history()
    elif args.command == "clear-history":
        debugger.clear_history()
    else:
        parser.print_help()


if __name__ == "__main__":
    main()