#!/usr/bin/env python3
"""
PhantomDB Interactive REPL

An interactive command line interface for PhantomDB with autocomplete and query history.
"""

import argparse
import sys
import json
import readline
import atexit
import os
from typing import List, Dict, Any, Optional
import requests


class PhantomDBREPL:
    """Interactive REPL for PhantomDB with autocomplete and query history."""
    
    def __init__(self, base_url: str = "http://localhost:8080"):
        """Initialize the REPL with the base URL of the PhantomDB server."""
        self.base_url = base_url.rstrip('/')
        self.session = requests.Session()
        self.history_file = os.path.expanduser("~/.phantomdb_history")
        self.current_database = None
        
        # Commands with their descriptions
        self.commands = {
            "help": "Show this help message",
            "exit": "Exit the REPL",
            "quit": "Exit the REPL",
            "use": "Select a database to work with",
            "show databases": "List all databases",
            "show tables": "List all tables in current database",
            "create database": "Create a new database",
            "drop database": "Drop a database",
            "create table": "Create a new table in current database",
            "drop table": "Drop a table from current database",
            "describe": "Show table schema",
            "insert": "Insert data into a table",
            "select": "Select data from a table",
            "update": "Update data in a table",
            "delete": "Delete data from a table",
            "begin": "Begin a new transaction",
            "commit": "Commit current transaction",
            "rollback": "Rollback current transaction",
            "history": "Show command history"
        }
        
        # SQL keywords for autocomplete
        self.sql_keywords = [
            "SELECT", "FROM", "WHERE", "INSERT", "INTO", "VALUES", "UPDATE", "SET",
            "DELETE", "CREATE", "DROP", "TABLE", "DATABASE", "INDEX", "ON",
            "PRIMARY", "KEY", "FOREIGN", "REFERENCES", "CONSTRAINT", "UNIQUE",
            "NOT", "NULL", "DEFAULT", "AUTO_INCREMENT", "INT", "VARCHAR", "TEXT",
            "DATE", "DATETIME", "BOOLEAN", "AND", "OR", "LIKE", "IN", "BETWEEN",
            "ORDER", "BY", "GROUP", "HAVING", "LIMIT", "OFFSET", "ASC", "DESC",
            "JOIN", "INNER", "LEFT", "RIGHT", "OUTER", "FULL", "UNION", "ALL"
        ]
        
        # Setup readline for autocomplete and history
        self.setup_readline()
    
    def setup_readline(self):
        """Setup readline for autocomplete and history."""
        # Set up autocomplete
        readline.set_completer(self.completer)
        readline.parse_and_bind("tab: complete")
        readline.set_completer_delims(" \t\n;")
        
        # Load history
        if os.path.exists(self.history_file):
            readline.read_history_file(self.history_file)
        
        # Save history on exit
        atexit.register(readline.write_history_file, self.history_file)
    
    def completer(self, text: str, state: int) -> Optional[str]:
        """Autocomplete function for readline."""
        # Get the current line buffer
        line = readline.get_line_buffer()
        words = line.split()
        
        # If we're at the beginning of the line or after a semicolon
        if not words or (len(words) == 1 and not line.endswith(" ")):
            # Complete commands
            matches = [cmd for cmd in self.commands.keys() if cmd.startswith(text.lower())]
        else:
            # Complete SQL keywords
            matches = [kw for kw in self.sql_keywords if kw.lower().startswith(text.lower())]
            
            # If we have a current database, also complete table names
            if self.current_database:
                try:
                    tables = self.get_tables(self.current_database)
                    matches.extend([table for table in tables if table.lower().startswith(text.lower())])
                except:
                    pass
        
        # Return the match for the current state
        if state < len(matches):
            return matches[state]
        else:
            return None
    
    def _make_request(self, method: str, endpoint: str, 
                     data: Optional[Dict[str, Any]] = None) -> Dict[str, Any]:
        """Make an HTTP request to the PhantomDB REST API."""
        url = f"{self.base_url}{endpoint}"
        
        try:
            if data is not None:
                response = self.session.request(
                    method, url, 
                    json=data,
                    headers={'Content-Type': 'application/json'}
                )
            else:
                response = self.session.request(method, url)
                
            response.raise_for_status()
            return response.json() if response.content else {}
            
        except requests.exceptions.RequestException as e:
            return {"error": str(e)}
        except json.JSONDecodeError as e:
            return {"error": f"Error decoding JSON response: {e}"}
    
    def get_databases(self) -> List[str]:
        """Get list of databases."""
        result = self._make_request("GET", "/databases")
        if "error" in result:
            raise Exception(result["error"])
        return result.get("databases", [])
    
    def get_tables(self, database: str) -> List[str]:
        """Get list of tables in a database."""
        result = self._make_request("GET", f"/databases/{database}/tables")
        if "error" in result:
            raise Exception(result["error"])
        return result.get("tables", [])
    
    def show_help(self):
        """Show help message."""
        print("PhantomDB Interactive REPL")
        print("=" * 30)
        print("Available commands:")
        for cmd, desc in self.commands.items():
            print(f"  {cmd:<20} - {desc}")
        print("\nYou can also execute SQL queries directly.")
        print("Use 'use <database>' to select a database first.")
    
    def select_database(self, database: str):
        """Select a database to work with."""
        try:
            databases = self.get_databases()
            if database in databases:
                self.current_database = database
                print(f"Using database: {database}")
            else:
                print(f"Database '{database}' not found.")
        except Exception as e:
            print(f"Error selecting database: {e}")
    
    def show_databases(self):
        """Show all databases."""
        try:
            databases = self.get_databases()
            print("Databases:")
            for db in databases:
                print(f"  {db}")
        except Exception as e:
            print(f"Error listing databases: {e}")
    
    def show_tables(self):
        """Show all tables in current database."""
        if not self.current_database:
            print("No database selected. Use 'use <database>' first.")
            return
            
        try:
            tables = self.get_tables(self.current_database)
            print(f"Tables in database '{self.current_database}':")
            for table in tables:
                print(f"  {table}")
        except Exception as e:
            print(f"Error listing tables: {e}")
    
    def execute_sql(self, query: str):
        """Execute a SQL query."""
        if not self.current_database:
            print("No database selected. Use 'use <database>' first.")
            return
            
        try:
            data = {"query": query}
            result = self._make_request("POST", f"/databases/{self.current_database}/query", data)
            if "error" in result:
                print(f"Query error: {result['error']}")
            else:
                # Pretty print the result
                if result:
                    print(json.dumps(result, indent=2))
                else:
                    print("Query executed successfully.")
        except Exception as e:
            print(f"Error executing query: {e}")
    
    def run(self):
        """Run the interactive REPL."""
        print("PhantomDB Interactive REPL")
        print("Type 'help' for available commands or 'exit' to quit.")
        print()
        
        while True:
            try:
                # Show prompt with current database if selected
                if self.current_database:
                    prompt = f"phantomdb [{self.current_database}]> "
                else:
                    prompt = "phantomdb> "
                
                # Get user input
                user_input = input(prompt).strip()
                
                # Skip empty lines
                if not user_input:
                    continue
                
                # Handle commands
                if user_input.lower() in ["exit", "quit"]:
                    print("Goodbye!")
                    break
                elif user_input.lower() == "help":
                    self.show_help()
                elif user_input.lower() == "history":
                    # Show command history
                    for i in range(readline.get_current_history_length()):
                        print(readline.get_history_item(i + 1))
                elif user_input.lower().startswith("use "):
                    db_name = user_input[4:].strip()
                    self.select_database(db_name)
                elif user_input.lower() == "show databases":
                    self.show_databases()
                elif user_input.lower() == "show tables":
                    self.show_tables()
                elif user_input.lower().startswith(("create", "drop", "insert", "select", "update", "delete")):
                    # Execute SQL query
                    self.execute_sql(user_input)
                else:
                    # Try to execute as SQL query
                    self.execute_sql(user_input)
                    
            except KeyboardInterrupt:
                print("\nUse 'exit' or 'quit' to exit.")
            except EOFError:
                print("\nGoodbye!")
                break
            except Exception as e:
                print(f"Error: {e}")


def main():
    """Main entry point for the PhantomDB REPL."""
    parser = argparse.ArgumentParser(
        description="PhantomDB Interactive REPL",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  phantomdb-repl.py
  phantomdb-repl.py --url http://localhost:8080
        """
    )
    
    parser.add_argument(
        "--url",
        default="http://localhost:8080",
        help="Base URL of the PhantomDB server (default: http://localhost:8080)"
    )
    
    args = parser.parse_args()
    
    repl = PhantomDBREPL(args.url)
    repl.run()


if __name__ == "__main__":
    main()