#!/usr/bin/env python3
"""
PhantomDB Command Line Interface

A command line tool for administering PhantomDB instances.
"""

import argparse
import sys
import json
from typing import List, Dict, Any, Optional
import requests


class PhantomDBCLI:
    """Command line interface for PhantomDB administration."""
    
    def __init__(self, base_url: str = "http://localhost:8080"):
        """Initialize the CLI with the base URL of the PhantomDB server."""
        self.base_url = base_url.rstrip('/')
        self.session = requests.Session()
    
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
            print(f"Error: {e}")
            return {"error": str(e)}
        except json.JSONDecodeError as e:
            print(f"Error decoding JSON response: {e}")
            return {"error": str(e)}
    
    def health_check(self) -> None:
        """Check the health of the PhantomDB server."""
        result = self._make_request("GET", "/health")
        if "error" in result:
            print(f"Health check failed: {result['error']}")
        else:
            print(f"Server health: {json.dumps(result, indent=2)}")
    
    def list_databases(self) -> None:
        """List all databases in PhantomDB."""
        result = self._make_request("GET", "/databases")
        if "error" in result:
            print(f"Failed to list databases: {result['error']}")
        else:
            print("Databases:")
            for db in result.get("databases", []):
                print(f"  - {db}")
    
    def create_database(self, name: str) -> None:
        """Create a new database."""
        data = {"name": name}
        result = self._make_request("POST", "/databases", data)
        if "error" in result:
            print(f"Failed to create database: {result['error']}")
        else:
            print(f"Database '{name}' created successfully")
    
    def drop_database(self, name: str) -> None:
        """Drop a database."""
        result = self._make_request("DELETE", f"/databases/{name}")
        if "error" in result:
            print(f"Failed to drop database: {result['error']}")
        else:
            print(f"Database '{name}' dropped successfully")
    
    def list_tables(self, database: str) -> None:
        """List all tables in a database."""
        result = self._make_request("GET", f"/databases/{database}/tables")
        if "error" in result:
            print(f"Failed to list tables: {result['error']}")
        else:
            print(f"Tables in database '{database}':")
            for table in result.get("tables", []):
                print(f"  - {table}")
    
    def create_table(self, database: str, name: str, columns: List[str]) -> None:
        """Create a new table in a database."""
        # Parse columns from "name:type" format
        column_defs = []
        for col in columns:
            if ":" in col:
                col_name, col_type = col.split(":", 1)
                column_defs.append({"name": col_name, "type": col_type})
            else:
                column_defs.append({"name": col, "type": "string"})
        
        data = {
            "name": name,
            "columns": column_defs
        }
        result = self._make_request("POST", f"/databases/{database}/tables", data)
        if "error" in result:
            print(f"Failed to create table: {result['error']}")
        else:
            print(f"Table '{name}' created successfully in database '{database}'")
    
    def drop_table(self, database: str, name: str) -> None:
        """Drop a table from a database."""
        result = self._make_request("DELETE", f"/databases/{database}/tables/{name}")
        if "error" in result:
            print(f"Failed to drop table: {result['error']}")
        else:
            print(f"Table '{name}' dropped successfully from database '{database}'")
    
    def execute_query(self, database: str, query: str) -> None:
        """Execute a custom SQL query."""
        data = {"query": query}
        result = self._make_request("POST", f"/databases/{database}/query", data)
        if "error" in result:
            print(f"Failed to execute query: {result['error']}")
        else:
            print("Query result:")
            print(json.dumps(result, indent=2))
    
    def begin_transaction(self) -> str:
        """Begin a new transaction."""
        result = self._make_request("POST", "/transactions")
        if "error" in result:
            print(f"Failed to begin transaction: {result['error']}")
            return ""
        else:
            txn_id = result.get("transaction_id", "")
            print(f"Transaction started with ID: {txn_id}")
            return txn_id
    
    def commit_transaction(self, transaction_id: str) -> None:
        """Commit a transaction."""
        result = self._make_request("POST", f"/transactions/{transaction_id}/commit")
        if "error" in result:
            print(f"Failed to commit transaction: {result['error']}")
        else:
            print(f"Transaction {transaction_id} committed successfully")
    
    def rollback_transaction(self, transaction_id: str) -> None:
        """Rollback a transaction."""
        result = self._make_request("POST", f"/transactions/{transaction_id}/rollback")
        if "error" in result:
            print(f"Failed to rollback transaction: {result['error']}")
        else:
            print(f"Transaction {transaction_id} rolled back successfully")
    
    def import_data(self, database: str, table: str, file_path: str, format: str = "csv") -> None:
        """Import data from a file."""
        data = {
            "database": database,
            "table": table,
            "file_path": file_path,
            "format": format
        }
        result = self._make_request("POST", f"/databases/{database}/tables/{table}/import", data)
        if "error" in result:
            print(f"Failed to import data: {result['error']}")
        else:
            rows_imported = result.get("rows_imported", 0)
            rows_failed = result.get("rows_failed", 0)
            print(f"Data import completed. Rows imported: {rows_imported}, Rows failed: {rows_failed}")
    
    def export_data(self, database: str, table: str, file_path: str, format: str = "csv") -> None:
        """Export data to a file."""
        data = {
            "database": database,
            "table": table,
            "file_path": file_path,
            "format": format
        }
        result = self._make_request("POST", f"/databases/{database}/tables/{table}/export", data)
        if "error" in result:
            print(f"Failed to export data: {result['error']}")
        else:
            rows_exported = result.get("rows_exported", 0)
            print(f"Data export completed. Rows exported: {rows_exported}")


def main():
    """Main entry point for the PhantomDB CLI."""
    parser = argparse.ArgumentParser(
        description="PhantomDB Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  phantomdb-cli.py health
  phantomdb-cli.py list-databases
  phantomdb-cli.py create-database mydb
  phantomdb-cli.py drop-database mydb
  phantomdb-cli.py list-tables mydb
  phantomdb-cli.py create-table mydb users id:int name:string email:string
  phantomdb-cli.py drop-table mydb users
  phantomdb-cli.py execute-query mydb "SELECT * FROM users"
  phantomdb-cli.py begin-transaction
  phantomdb-cli.py commit-transaction txn_12345
  phantomdb-cli.py rollback-transaction txn_12345
  phantomdb-cli.py import mydb users data.csv --format csv
  phantomdb-cli.py export mydb users data.json --format json
        """
    )
    
    parser.add_argument(
        "--url",
        default="http://localhost:8080",
        help="Base URL of the PhantomDB server (default: http://localhost:8080)"
    )
    
    subparsers = parser.add_subparsers(dest="command", help="Available commands")
    
    # Health check command
    subparsers.add_parser("health", help="Check server health")
    
    # Database commands
    subparsers.add_parser("list-databases", help="List all databases")
    
    create_db_parser = subparsers.add_parser("create-database", help="Create a new database")
    create_db_parser.add_argument("name", help="Name of the database to create")
    
    drop_db_parser = subparsers.add_parser("drop-database", help="Drop a database")
    drop_db_parser.add_argument("name", help="Name of the database to drop")
    
    # Table commands
    list_tables_parser = subparsers.add_parser("list-tables", help="List tables in a database")
    list_tables_parser.add_argument("database", help="Name of the database")
    
    create_table_parser = subparsers.add_parser("create-table", help="Create a new table")
    create_table_parser.add_argument("database", help="Name of the database")
    create_table_parser.add_argument("name", help="Name of the table to create")
    create_table_parser.add_argument("columns", nargs="+", 
                                   help="Column definitions (name:type format)")
    
    drop_table_parser = subparsers.add_parser("drop-table", help="Drop a table")
    drop_table_parser.add_argument("database", help="Name of the database")
    drop_table_parser.add_argument("name", help="Name of the table to drop")
    
    # Query command
    query_parser = subparsers.add_parser("execute-query", help="Execute a custom SQL query")
    query_parser.add_argument("database", help="Name of the database")
    query_parser.add_argument("query", help="SQL query to execute")
    
    # Transaction commands
    subparsers.add_parser("begin-transaction", help="Begin a new transaction")
    
    commit_txn_parser = subparsers.add_parser("commit-transaction", help="Commit a transaction")
    commit_txn_parser.add_argument("transaction_id", help="ID of the transaction to commit")
    
    rollback_txn_parser = subparsers.add_parser("rollback-transaction", help="Rollback a transaction")
    rollback_txn_parser.add_argument("transaction_id", help="ID of the transaction to rollback")
    
    # Import command
    import_parser = subparsers.add_parser("import", help="Import data from a file")
    import_parser.add_argument("database", help="Name of the database")
    import_parser.add_argument("table", help="Name of the table")
    import_parser.add_argument("file_path", help="Path to the file to import")
    import_parser.add_argument("--format", choices=["csv", "json"], default="csv", 
                              help="Format of the file to import (default: csv)")
    
    # Export command
    export_parser = subparsers.add_parser("export", help="Export data to a file")
    export_parser.add_argument("database", help="Name of the database")
    export_parser.add_argument("table", help="Name of the table")
    export_parser.add_argument("file_path", help="Path to the file to export")
    export_parser.add_argument("--format", choices=["csv", "json"], default="csv", 
                              help="Format of the file to export (default: csv)")
    
    args = parser.parse_args()
    
    if not args.command:
        parser.print_help()
        return
    
    cli = PhantomDBCLI(args.url)
    
    # Execute the appropriate command
    if args.command == "health":
        cli.health_check()
    elif args.command == "list-databases":
        cli.list_databases()
    elif args.command == "create-database":
        cli.create_database(args.name)
    elif args.command == "drop-database":
        cli.drop_database(args.name)
    elif args.command == "list-tables":
        cli.list_tables(args.database)
    elif args.command == "create-table":
        cli.create_table(args.database, args.name, args.columns)
    elif args.command == "drop-table":
        cli.drop_table(args.database, args.name)
    elif args.command == "execute-query":
        cli.execute_query(args.database, args.query)
    elif args.command == "begin-transaction":
        cli.begin_transaction()
    elif args.command == "commit-transaction":
        cli.commit_transaction(args.transaction_id)
    elif args.command == "rollback-transaction":
        cli.rollback_transaction(args.transaction_id)
    elif args.command == "import":
        cli.import_data(args.database, args.table, args.file_path, args.format)
    elif args.command == "export":
        cli.export_data(args.database, args.table, args.file_path, args.format)
    else:
        parser.print_help()


if __name__ == "__main__":
    main()