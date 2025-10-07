"""
PhantomDB Python Client

This module provides a Python interface to interact with the PhantomDB REST API.
"""

import requests
import json
from typing import Dict, List, Any, Optional


class PhantomDBError(Exception):
    """Custom exception for PhantomDB client errors."""
    pass


class PhantomDB:
    """
    A client for interacting with PhantomDB through its REST API.
    
    This client provides methods for database operations such as creating databases,
    tables, inserting data, querying data, etc.
    """
    
    def __init__(self, base_url: str = "http://localhost:8080"):
        """
        Initialize the PhantomDB client.
        
        Args:
            base_url (str): The base URL of the PhantomDB REST API server.
                           Defaults to "http://localhost:8080".
        """
        self.base_url = base_url.rstrip('/')
        self.session = requests.Session()
    
    def _make_request(self, method: str, endpoint: str, 
                     data: Optional[Dict] = None) -> Dict:
        """
        Make an HTTP request to the PhantomDB REST API.
        
        Args:
            method (str): HTTP method (GET, POST, PUT, DELETE, etc.)
            endpoint (str): API endpoint
            data (dict, optional): Data to send with the request
            
        Returns:
            dict: JSON response from the server
            
        Raises:
            PhantomDBError: If the request fails
        """
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
            raise PhantomDBError(f"Request failed: {str(e)}")
        except json.JSONDecodeError as e:
            raise PhantomDBError(f"Failed to decode JSON response: {str(e)}")
    
    def health_check(self) -> Dict:
        """
        Check the health of the PhantomDB server.
        
        Returns:
            dict: Health status information
        """
        return self._make_request("GET", "/health")
    
    def list_databases(self) -> List[str]:
        """
        List all databases in PhantomDB.
        
        Returns:
            list: List of database names
        """
        response = self._make_request("GET", "/databases")
        # This would return a list of database names in a real implementation
        return response.get("databases", [])
    
    def create_database(self, name: str) -> Dict:
        """
        Create a new database.
        
        Args:
            name (str): Name of the database to create
            
        Returns:
            dict: Response from the server
        """
        data = {"name": name}
        return self._make_request("POST", "/databases", data)
    
    def drop_database(self, name: str) -> Dict:
        """
        Drop a database.
        
        Args:
            name (str): Name of the database to drop
            
        Returns:
            dict: Response from the server
        """
        return self._make_request("DELETE", f"/databases/{name}")
    
    def list_tables(self, database: str) -> List[str]:
        """
        List all tables in a database.
        
        Args:
            database (str): Name of the database
            
        Returns:
            list: List of table names
        """
        response = self._make_request("GET", f"/databases/{database}/tables")
        # This would return a list of table names in a real implementation
        return response.get("tables", [])
    
    def create_table(self, database: str, name: str, 
                    columns: List[Dict[str, str]]) -> Dict:
        """
        Create a new table in a database.
        
        Args:
            database (str): Name of the database
            name (str): Name of the table to create
            columns (list): List of column definitions
            
        Returns:
            dict: Response from the server
        """
        data = {
            "name": name,
            "columns": columns
        }
        return self._make_request("POST", f"/databases/{database}/tables", data)
    
    def drop_table(self, database: str, name: str) -> Dict:
        """
        Drop a table from a database.
        
        Args:
            database (str): Name of the database
            name (str): Name of the table to drop
            
        Returns:
            dict: Response from the server
        """
        return self._make_request("DELETE", f"/databases/{database}/tables/{name}")
    
    def insert(self, database: str, table: str, data: Dict[str, Any]) -> Dict:
        """
        Insert data into a table.
        
        Args:
            database (str): Name of the database
            table (str): Name of the table
            data (dict): Data to insert
            
        Returns:
            dict: Response from the server
        """
        return self._make_request("POST", f"/databases/{database}/tables/{table}", data)
    
    def select(self, database: str, table: str, 
              condition: Optional[str] = None) -> List[Dict]:
        """
        Select data from a table.
        
        Args:
            database (str): Name of the database
            table (str): Name of the table
            condition (str, optional): Condition for filtering data
            
        Returns:
            list: List of records matching the query
        """
        endpoint = f"/databases/{database}/tables/{table}"
        if condition:
            endpoint += f"?condition={condition}"
            
        response = self._make_request("GET", endpoint)
        # This would return a list of records in a real implementation
        return response.get("records", [])
    
    def update(self, database: str, table: str, data: Dict[str, Any],
              condition: Optional[str] = None) -> Dict:
        """
        Update data in a table.
        
        Args:
            database (str): Name of the database
            table (str): Name of the table
            data (dict): Data to update
            condition (str, optional): Condition for filtering data to update
            
        Returns:
            dict: Response from the server
        """
        payload: Dict[str, Any] = {"data": data}
        if condition:
            payload["condition"] = condition
            
        return self._make_request("PUT", f"/databases/{database}/tables/{table}", payload)
    
    def delete(self, database: str, table: str,
              condition: Optional[str] = None) -> Dict:
        """
        Delete data from a table.
        
        Args:
            database (str): Name of the database
            table (str): Name of the table
            condition (str, optional): Condition for filtering data to delete
            
        Returns:
            dict: Response from the server
        """
        endpoint = f"/databases/{database}/tables/{table}"
        if condition:
            endpoint += f"?condition={condition}"
            
        return self._make_request("DELETE", endpoint)
    
    def execute_query(self, database: str, query: str) -> Dict:
        """
        Execute a custom SQL query.
        
        Args:
            database (str): Name of the database
            query (str): SQL query to execute
            
        Returns:
            dict: Query result
        """
        data = {"query": query}
        return self._make_request("POST", f"/databases/{database}/query", data)
    
    def begin_transaction(self) -> str:
        """
        Begin a new transaction.
        
        Returns:
            str: Transaction ID
        """
        response = self._make_request("POST", "/transactions")
        return response.get("transaction_id", "")
    
    def commit_transaction(self, transaction_id: str) -> Dict:
        """
        Commit a transaction.
        
        Args:
            transaction_id (str): ID of the transaction to commit
            
        Returns:
            dict: Response from the server
        """
        return self._make_request("POST", f"/transactions/{transaction_id}/commit")
    
    def rollback_transaction(self, transaction_id: str) -> Dict:
        """
        Rollback a transaction.
        
        Args:
            transaction_id (str): ID of the transaction to rollback
            
        Returns:
            dict: Response from the server
        """
        return self._make_request("POST", f"/transactions/{transaction_id}/rollback")


# Convenience functions for common operations
def connect(base_url: str = "http://localhost:8080") -> PhantomDB:
    """
    Create a new PhantomDB client connection.
    
    Args:
        base_url (str): The base URL of the PhantomDB REST API server.
        
    Returns:
        PhantomDB: A new PhantomDB client instance
    """
    return PhantomDB(base_url)