"""
PhantomDB Schema Manager

Handles database schema operations and management.
"""

import json
from typing import Dict, List, Any


class SchemaManager:
    """Manages database schema operations and validation."""
    
    def __init__(self):
        """Initialize the schema manager."""
        pass
    
    def validate_schema(self, schema: Dict[str, Any]) -> bool:
        """Validate a schema definition.
        
        Args:
            schema: Schema definition to validate
            
        Returns:
            True if schema is valid, False otherwise
        """
        try:
            # Check required fields
            if "version" not in schema:
                raise ValueError("Schema missing 'version' field")
            
            if "databases" not in schema:
                raise ValueError("Schema missing 'databases' field")
            
            # Validate databases structure
            for db_name, db_info in schema.get("databases", {}).items():
                if not isinstance(db_info, dict):
                    raise ValueError(f"Database '{db_name}' info must be a dictionary")
                
                if "tables" not in db_info:
                    raise ValueError(f"Database '{db_name}' missing 'tables' field")
                
                # Validate tables structure
                for table_name, table_info in db_info.get("tables", {}).items():
                    if not isinstance(table_info, dict):
                        raise ValueError(f"Table '{table_name}' info must be a dictionary")
            
            return True
        except Exception as e:
            print(f"Schema validation error: {e}")
            return False
    
    def apply_schema(self, db, schema_file: str):
        """Apply a schema definition to the database.
        
        Args:
            db: PhantomDB instance
            schema_file: Path to schema definition file
        """
        try:
            # Load schema from file
            with open(schema_file, 'r') as f:
                schema = json.load(f)
            
            # Validate schema
            if not self.validate_schema(schema):
                raise ValueError("Invalid schema definition")
            
            # Apply schema to database
            for db_name, db_info in schema.get("databases", {}).items():
                # Create database if it doesn't exist
                try:
                    db.create_database(db_name)
                except Exception:
                    # Database might already exist, continue
                    pass
                
                db.select_database(db_name)
                
                # Create tables
                for table_name, table_info in db_info.get("tables", {}).items():
                    # Create table with columns (simplified)
                    columns = table_info.get("columns", [])
                    if columns:
                        # Convert columns to schema format
                        schema_def = {}
                        for col in columns:
                            if isinstance(col, dict) and "name" in col and "type" in col:
                                schema_def[col["name"]] = col["type"]
                            elif isinstance(col, str):
                                # Simple column name, assume string type
                                schema_def[col] = "string"
                        
                        try:
                            db.create_table(table_name, schema_def)
                        except Exception as e:
                            print(f"Warning: Could not create table '{table_name}': {e}")
                    
                    # Create indexes
                    indexes = table_info.get("indexes", [])
                    for index in indexes:
                        try:
                            if isinstance(index, dict) and "name" in index and "columns" in index:
                                db.create_index(table_name, index["name"], index["columns"])
                            elif isinstance(index, str):
                                # Simple index name, assume single column
                                db.create_index(table_name, index, [index])
                        except Exception as e:
                            print(f"Warning: Could not create index '{index}': {e}")
            
            print(f"Schema applied successfully from: {schema_file}")
        except Exception as e:
            print(f"Error applying schema: {e}")
            raise
    
    def export_schema(self, db, filepath: str = "schema.json"):
        """Export current database schema to a file.
        
        Args:
            db: PhantomDB instance
            filepath: Path to save schema file
        """
        try:
            # Get all databases
            databases = db.list_databases()
            
            schema = {
                "version": "1.0",
                "exported_at": self._get_current_timestamp(),
                "databases": {}
            }
            
            for db_name in databases:
                db.select_database(db_name)
                tables = db.list_tables()
                
                schema["databases"][db_name] = {
                    "tables": {}
                }
                
                for table_name in tables:
                    # Get table information (this is a simplified example)
                    schema["databases"][db_name]["tables"][table_name] = {
                        "columns": self._get_table_columns(db, table_name),
                        "indexes": self._get_table_indexes(db, table_name),
                        "constraints": self._get_table_constraints(db, table_name)
                    }
            
            # Save schema to file
            with open(filepath, 'w') as f:
                json.dump(schema, f, indent=2)
            
            print(f"Schema exported to: {filepath}")
            return filepath
        except Exception as e:
            print(f"Error exporting schema: {e}")
            raise
    
    def _get_current_timestamp(self) -> str:
        """Get current timestamp as ISO string.
        
        Returns:
            Current timestamp
        """
        from datetime import datetime
        return datetime.now().isoformat()
    
    def _get_table_columns(self, db, table_name: str) -> List[Dict[str, Any]]:
        """Get table column information.
        
        Args:
            db: PhantomDB instance
            table_name: Name of the table
            
        Returns:
            List of column information
        """
        # This is a simplified implementation
        # In a real implementation, this would query the database metadata
        return [
            {"name": "id", "type": "integer", "nullable": False},
            {"name": "created_at", "type": "timestamp", "nullable": False}
        ]
    
    def _get_table_indexes(self, db, table_name: str) -> List[Dict[str, Any]]:
        """Get table index information.
        
        Args:
            db: PhantomDB instance
            table_name: Name of the table
            
        Returns:
            List of index information
        """
        # This is a simplified implementation
        return [
            {"name": "primary_key", "columns": ["id"], "unique": True}
        ]
    
    def _get_table_constraints(self, db, table_name: str) -> List[Dict[str, Any]]:
        """Get table constraint information.
        
        Args:
            db: PhantomDB instance
            table_name: Name of the table
            
        Returns:
            List of constraint information
        """
        # This is a simplified implementation
        return [
            {"name": "pk_constraint", "type": "primary_key", "columns": ["id"]}
        ]


if __name__ == "__main__":
    # Example usage
    manager = SchemaManager()
    print("Schema Manager initialized")