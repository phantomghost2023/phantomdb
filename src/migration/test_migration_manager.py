"""
Unit tests for PhantomDB Migration Manager
"""

import unittest
import os
import tempfile
import shutil
from migration_manager import MigrationManager


class TestMigrationManager(unittest.TestCase):
    """Test cases for MigrationManager class."""
    
    def setUp(self):
        """Set up test fixtures."""
        # Create a temporary directory for test migrations
        self.test_dir = tempfile.mkdtemp()
        self.migration_manager = MigrationManager(self.test_dir)
    
    def tearDown(self):
        """Clean up test fixtures."""
        # Remove temporary directory
        shutil.rmtree(self.test_dir)
    
    def test_init(self):
        """Test MigrationManager initialization."""
        self.assertEqual(self.migration_manager.migrations_dir, self.test_dir)
        self.assertTrue(os.path.exists(self.test_dir))
    
    def test_create_migration(self):
        """Test creating a new migration."""
        # Create a migration
        filepath = self.migration_manager.create_migration(
            "test_migration", 
            "Test migration description"
        )
        
        # Check that file was created
        self.assertTrue(os.path.exists(filepath))
        
        # Check filename format
        filename = os.path.basename(filepath)
        self.assertTrue(filename.endswith("_test_migration.py"))
        
        # Check file content
        with open(filepath, 'r') as f:
            content = f.read()
        
        self.assertIn("Test migration description", content)
        self.assertIn("def upgrade(db):", content)
        self.assertIn("def downgrade(db):", content)
    
    def test_get_migrations(self):
        """Test getting migrations."""
        # Create a few migrations
        self.migration_manager.create_migration("first_migration")
        self.migration_manager.create_migration("second_migration")
        
        # Get migrations
        migrations = self.migration_manager.get_migrations()
        
        # Check results
        self.assertEqual(len(migrations), 2)
        self.assertEqual(migrations[0]['name'], 'first_migration')
        self.assertEqual(migrations[1]['name'], 'second_migration')
    
    def test_get_applied_migrations(self):
        """Test getting applied migrations."""
        # Initially no applied migrations
        applied = self.migration_manager.get_applied_migrations()
        self.assertEqual(applied, [])
    
    def test_record_migration(self):
        """Test recording a migration."""
        # Create a mock migration info
        migration_info = {
            'filename': '20230101_120000_test.py',
            'timestamp': '20230101_120000',
            'name': 'test',
            'hash': 'abc123'
        }
        
        # Record migration
        self.migration_manager.record_migration(migration_info, "applied")
        
        # Check that it was recorded
        applied = self.migration_manager.get_applied_migrations()
        self.assertEqual(len(applied), 1)
        self.assertEqual(applied[0]['filename'], '20230101_120000_test.py')
        self.assertEqual(applied[0]['action'], 'applied')


if __name__ == '__main__':
    unittest.main()