"""
Example usage of PhantomDB Migration and Schema Management Utilities
"""

import os
import sys

# Add parent directory to path to import modules
sys.path.append(os.path.dirname(__file__))

from migration_manager import MigrationManager
from schema_manager import SchemaManager


def main():
    """Demonstrate migration and schema management utilities."""
    print("PhantomDB Migration and Schema Management Example")
    print("=" * 50)
    
    # Initialize managers
    migration_manager = MigrationManager("migrations")
    schema_manager = SchemaManager()
    
    # Show available migrations
    print("\n1. Available Migrations:")
    migrations = migration_manager.get_migrations()
    if migrations:
        for migration in migrations:
            print(f"   - {migration['name']} ({migration['timestamp']})")
    else:
        print("   No migrations found")
    
    # Show migration status
    print("\n2. Migration Status:")
    applied_migrations = migration_manager.get_applied_migrations()
    if applied_migrations:
        print("   Applied migrations:")
        for migration in applied_migrations:
            print(f"   - {migration['name']} ({migration['applied_at']})")
    else:
        print("   No migrations applied")
    
    # Create a new migration
    print("\n3. Creating new migration:")
    migration_file = migration_manager.create_migration(
        "add_products_table", 
        "Add products table for inventory management"
    )
    print(f"   Created: {os.path.basename(migration_file)}")
    
    # Show updated migrations
    print("\n4. Updated Migrations:")
    migrations = migration_manager.get_migrations()
    for migration in migrations:
        print(f"   - {migration['name']} ({migration['timestamp']})")


if __name__ == "__main__":
    main()