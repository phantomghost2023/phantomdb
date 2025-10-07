"""
Unit tests for PhantomDB Schema Manager
"""

import unittest
import json
from schema_manager import SchemaManager


class TestSchemaManager(unittest.TestCase):
    """Test cases for SchemaManager class."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.schema_manager = SchemaManager()
    
    def test_init(self):
        """Test SchemaManager initialization."""
        self.assertIsInstance(self.schema_manager, SchemaManager)
    
    def test_validate_schema_valid(self):
        """Test validating a valid schema."""
        valid_schema = {
            "version": "1.0",
            "databases": {
                "test_db": {
                    "tables": {
                        "users": {
                            "columns": [
                                {"name": "id", "type": "integer"},
                                {"name": "name", "type": "string"}
                            ]
                        }
                    }
                }
            }
        }
        
        result = self.schema_manager.validate_schema(valid_schema)
        self.assertTrue(result)
    
    def test_validate_schema_missing_version(self):
        """Test validating a schema missing version."""
        invalid_schema = {
            "databases": {
                "test_db": {
                    "tables": {}
                }
            }
        }
        
        result = self.schema_manager.validate_schema(invalid_schema)
        self.assertFalse(result)
    
    def test_validate_schema_missing_databases(self):
        """Test validating a schema missing databases."""
        invalid_schema = {
            "version": "1.0"
        }
        
        result = self.schema_manager.validate_schema(invalid_schema)
        self.assertFalse(result)
    
    def test_get_current_timestamp(self):
        """Test getting current timestamp."""
        timestamp = self.schema_manager._get_current_timestamp()
        self.assertIsInstance(timestamp, str)
        self.assertIn("T", timestamp)  # ISO format contains T
    
    def test_get_table_columns(self):
        """Test getting table columns."""
        # This is a simplified test since the method returns mock data
        columns = self.schema_manager._get_table_columns(None, "test_table")
        self.assertIsInstance(columns, list)
        self.assertGreater(len(columns), 0)
    
    def test_get_table_indexes(self):
        """Test getting table indexes."""
        # This is a simplified test since the method returns mock data
        indexes = self.schema_manager._get_table_indexes(None, "test_table")
        self.assertIsInstance(indexes, list)
        self.assertGreater(len(indexes), 0)
    
    def test_get_table_constraints(self):
        """Test getting table constraints."""
        # This is a simplified test since the method returns mock data
        constraints = self.schema_manager._get_table_constraints(None, "test_table")
        self.assertIsInstance(constraints, list)
        self.assertGreater(len(constraints), 0)


if __name__ == '__main__':
    unittest.main()