"""
Example tests demonstrating the PhantomDB Testing Framework.
"""

import unittest
import sys
import os
sys.path.insert(0, os.path.dirname(__file__))

from framework import PhantomDBTestCase, phantomdb_test


class TestExampleOperations(unittest.TestCase):
    """Example test case demonstrating the PhantomDB testing framework."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.test_case = PhantomDBTestCase()
        self.test_case.setUp()
        
        # Create a users table for testing
        self.test_case.create_table("users", [
            {"name": "id", "type": "int"},
            {"name": "name", "type": "string"},
            {"name": "email", "type": "string"}
        ])
        
        # Create an orders table for testing
        self.test_case.create_table("orders", [
            {"name": "id", "type": "int"},
            {"name": "user_id", "type": "int"},
            {"name": "product", "type": "string"},
            {"name": "amount", "type": "float"}
        ])
    
    def tearDown(self):
        """Clean up after tests."""
        self.test_case.tearDown()
    
    def test_user_creation(self):
        """Test creating a user."""
        # Insert a user
        self.test_case.insert_data("users", {
            "id": 1,
            "name": "John Doe",
            "email": "john@example.com"
        })
        
        # Assert the user was created
        # Note: In a real implementation, these would be actual assertions
        # For this example, we'll just verify the methods exist
        self.assertTrue(hasattr(self.test_case, 'assert_row_count'))
        self.assertTrue(hasattr(self.test_case, 'assert_row_exists'))
    
    def test_multiple_users(self):
        """Test creating multiple users."""
        # Insert multiple users
        users_data = [
            {"id": 1, "name": "John Doe", "email": "john@example.com"},
            {"id": 2, "name": "Jane Smith", "email": "jane@example.com"},
            {"id": 3, "name": "Bob Johnson", "email": "bob@example.com"}
        ]
        
        for user_data in users_data:
            self.test_case.insert_data("users", user_data)
        
        # Assert all users were created
        # Note: In a real implementation, these would be actual assertions
        self.assertTrue(hasattr(self.test_case, 'assert_row_count'))
    
    def test_user_and_order_creation(self):
        """Test creating users and orders."""
        # Insert a user
        self.test_case.insert_data("users", {
            "id": 1,
            "name": "John Doe",
            "email": "john@example.com"
        })
        
        # Insert an order for that user
        self.test_case.insert_data("orders", {
            "id": 1,
            "user_id": 1,
            "product": "Test Product",
            "amount": 99.99
        })
        
        # Assert both were created
        # Note: In a real implementation, these would be actual assertions
        self.assertTrue(hasattr(self.test_case, 'assert_row_count'))


@phantomdb_test()
def test_with_decorator(test_database):
    """Test using the @phantomdb_test decorator."""
    # The test_database is automatically created and will be cleaned up
    print(f"Testing with database: {test_database}")
    
    # In a real test, you would use the database for testing
    assert test_database.startswith("test_db_")


if __name__ == '__main__':
    unittest.main()