"""
PhantomDB Migration and Schema Management CLI

Command-line interface for managing database migrations and schema operations.
"""

import argparse
import sys
import os
import json

# Add parent directory to path to import phantomdb
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'clients', 'python'))

try:
    from phantomdb import PhantomDB
except ImportError:
    # Create a mock PhantomDB for development
    class PhantomDB:
        def __init__(self, url="http://localhost:8080"):
            self.url = url
        
        def list_databases(self):
            return ["test_db"]
        
        def create_database(self, name):
            print(f"Created database: {name}")
        
        def select_database(self, name):
            print(f"Selected database: {name}")
        
        def list_tables(self):
            return ["test_table"]
        
        def create_table(self, name, schema):
            print(f"Created table: {name} with schema {schema}")

from .migration_manager import MigrationManager
from .schema_manager import SchemaManager


def main():
    """Main entry point for the CLI."""
    parser = argparse.ArgumentParser(
        description="PhantomDB Migration and Schema Management Tool",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Commands:
  create     Create a new migration
  migrate    Apply pending migrations
  rollback   Revert the last migration
  status     Show migration status
  schema     Manage database schema
  export     Export current database schema
        """
    )
    
    parser.add_argument(
        "--db-url",
        default="http://localhost:8080",
        help="PhantomDB server URL (default: http://localhost:8080)"
    )
    
    parser.add_argument(
        "--migrations-dir",
        default="migrations",
        help="Migrations directory (default: migrations)"
    )
    
    subparsers = parser.add_subparsers(dest="command", help="Available commands")
    
    # Create migration command
    create_parser = subparsers.add_parser("create", help="Create a new migration")
    create_parser.add_argument("name", help="Migration name")
    create_parser.add_argument(
        "--description", 
        default="", 
        help="Migration description"
    )
    
    # Migrate command
    migrate_parser = subparsers.add_parser("migrate", help="Apply pending migrations")
    migrate_parser.add_argument(
        "--target", 
        help="Target version to migrate to"
    )
    
    # Rollback command
    rollback_parser = subparsers.add_parser("rollback", help="Revert the last migration")
    
    # Status command
    status_parser = subparsers.add_parser("status", help="Show migration status")
    
    # Schema command
    schema_parser = subparsers.add_parser("schema", help="Manage database schema")
    schema_parser.add_argument(
        "action", 
        choices=["apply", "validate"], 
        help="Schema action"
    )
    schema_parser.add_argument(
        "schema_file", 
        help="Path to schema file"
    )
    
    # Export command
    export_parser = subparsers.add_parser("export", help="Export current database schema")
    export_parser.add_argument(
        "--output", 
        default="schema.json", 
        help="Output file path (default: schema.json)"
    )
    
    # Parse arguments
    args = parser.parse_args()
    
    if not args.command:
        parser.print_help()
        return
    
    # Initialize managers
    db = PhantomDB(args.db_url)
    migration_manager = MigrationManager(args.migrations_dir)
    schema_manager = SchemaManager()
    
    # Execute command
    try:
        if args.command == "create":
            migration_manager.create_migration(args.name, args.description)
        
        elif args.command == "migrate":
            migration_manager.migrate(db, args.target)
            print("All pending migrations applied successfully")
        
        elif args.command == "rollback":
            applied_migrations = migration_manager.get_applied_migrations()
            if applied_migrations:
                last_migration = applied_migrations[-1]
                migration_manager.revert_migration(last_migration, db)
                print(f"Reverted migration: {last_migration['name']}")
            else:
                print("No migrations to rollback")
        
        elif args.command == "status":
            all_migrations = migration_manager.get_migrations()
            applied_migrations = migration_manager.get_applied_migrations()
            
            print("Migration Status:")
            print("=" * 50)
            
            if not all_migrations:
                print("No migrations found")
                return
            
            # Get applied migration filenames
            applied_filenames = {m['filename'] for m in applied_migrations if m['action'] == 'applied'}
            
            for migration in all_migrations:
                status = "APPLIED" if migration['filename'] in applied_filenames else "PENDING"
                print(f"{status:8} {migration['name']}")
        
        elif args.command == "schema":
            if args.action == "apply":
                schema_manager.apply_schema(db, args.schema_file)
            elif args.action == "validate":
                with open(args.schema_file, 'r') as f:
                    schema = json.load(f)
                if schema_manager.validate_schema(schema):
                    print("Schema is valid")
                else:
                    print("Schema is invalid")
        
        elif args.command == "export":
            schema_manager.export_schema(db, args.output)
            print(f"Schema exported to: {args.output}")
            
    except Exception as e:
        print(f"Error executing command: {e}")
        sys.exit(1)


if __name__ == "__main__":
    main()