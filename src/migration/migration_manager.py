"""
PhantomDB Migration Manager

Handles database schema migrations and versioning.
"""

import os
import json
import hashlib
from datetime import datetime
from typing import List, Dict, Any


class MigrationManager:
    """Manages database schema migrations and versioning."""
    
    def __init__(self, migrations_dir: str = "migrations"):
        """Initialize the migration manager.
        
        Args:
            migrations_dir: Directory to store migration files
        """
        self.migrations_dir = migrations_dir
        self.migration_history_file = os.path.join(migrations_dir, "migration_history.json")
        
        # Create migrations directory if it doesn't exist
        if not os.path.exists(migrations_dir):
            os.makedirs(migrations_dir)
    
    def create_migration(self, name: str, description: str = "") -> str:
        """Create a new migration file.
        
        Args:
            name: Name of the migration
            description: Description of what the migration does
            
        Returns:
            Path to the created migration file
        """
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        filename = f"{timestamp}_{name}.py"
        filepath = os.path.join(self.migrations_dir, filename)
        
        migration_content = f'''"""
PhantomDB Migration: {name}
Created: {datetime.now().strftime("%Y-%m-%d %H:%M:%S")}
Description: {description}
"""

def upgrade(db):
    """Apply this migration.
    
    Args:
        db: PhantomDB instance
    """
    # TODO: Implement migration upgrade logic
    pass

def downgrade(db):
    """Revert this migration.
    
    Args:
        db: PhantomDB instance
    """
    # TODO: Implement migration downgrade logic
    pass
'''
        
        with open(filepath, 'w') as f:
            f.write(migration_content)
        
        print(f"Created migration: {filepath}")
        return filepath
    
    def get_migrations(self) -> List[Dict[str, Any]]:
        """Get all migrations in the migrations directory.
        
        Returns:
            List of migration information
        """
        migrations = []
        
        if not os.path.exists(self.migrations_dir):
            return migrations
        
        for filename in sorted(os.listdir(self.migrations_dir)):
            if filename.endswith('.py') and filename != '__init__.py':
                filepath = os.path.join(self.migrations_dir, filename)
                
                # Extract timestamp and name from filename
                parts = filename.split('_', 2)
                if len(parts) >= 3:
                    timestamp = parts[0] + "_" + parts[1]
                    name = parts[2].replace('.py', '')
                    
                    # Calculate file hash for integrity checking
                    with open(filepath, 'rb') as f:
                        file_hash = hashlib.md5(f.read()).hexdigest()
                    
                    migrations.append({
                        'filename': filename,
                        'filepath': filepath,
                        'timestamp': timestamp,
                        'name': name,
                        'hash': file_hash
                    })
        
        return migrations
    
    def get_applied_migrations(self) -> List[Dict[str, Any]]:
        """Get list of applied migrations from history.
        
        Returns:
            List of applied migrations
        """
        if not os.path.exists(self.migration_history_file):
            return []
        
        try:
            with open(self.migration_history_file, 'r') as f:
                return json.load(f)
        except (json.JSONDecodeError, FileNotFoundError):
            return []
    
    def record_migration(self, migration_info: Dict[str, Any], action: str = "applied"):
        """Record a migration in the history file.
        
        Args:
            migration_info: Migration information
            action: Action performed ("applied" or "reverted")
        """
        history = self.get_applied_migrations()
        
        record = {
            'filename': migration_info['filename'],
            'timestamp': migration_info['timestamp'],
            'name': migration_info['name'],
            'hash': migration_info['hash'],
            'applied_at': datetime.now().isoformat(),
            'action': action
        }
        
        history.append(record)
        
        # Save history
        with open(self.migration_history_file, 'w') as f:
            json.dump(history, f, indent=2)
    
    def apply_migration(self, migration_info: Dict[str, Any], db):
        """Apply a specific migration.
        
        Args:
            migration_info: Migration information
            db: PhantomDB instance
        """
        # Load migration module
        import importlib.util
        spec = importlib.util.spec_from_file_location(
            migration_info['name'], 
            migration_info['filepath']
        )
        
        if spec is None:
            raise ImportError(f"Could not load migration {migration_info['name']}")
        
        migration_module = importlib.util.module_from_spec(spec)
        if spec.loader is not None:
            spec.loader.exec_module(migration_module)
        
        # Apply migration
        if hasattr(migration_module, 'upgrade'):
            migration_module.upgrade(db)
            self.record_migration(migration_info, "applied")
            print(f"Applied migration: {migration_info['name']}")
        else:
            raise ValueError(f"Migration {migration_info['name']} missing upgrade function")
    
    def revert_migration(self, migration_info: Dict[str, Any], db):
        """Revert a specific migration.
        
        Args:
            migration_info: Migration information
            db: PhantomDB instance
        """
        # Load migration module
        import importlib.util
        spec = importlib.util.spec_from_file_location(
            migration_info['name'], 
            migration_info['filepath']
        )
        
        if spec is None:
            raise ImportError(f"Could not load migration {migration_info['name']}")
        
        migration_module = importlib.util.module_from_spec(spec)
        if spec.loader is not None:
            spec.loader.exec_module(migration_module)
        
        # Revert migration
        if hasattr(migration_module, 'downgrade'):
            migration_module.downgrade(db)
            self.record_migration(migration_info, "reverted")
            print(f"Reverted migration: {migration_info['name']}")
        else:
            raise ValueError(f"Migration {migration_info['name']} missing downgrade function")
    
    def migrate(self, db, target_version: str | None = None):
        """Apply all pending migrations.
        
        Args:
            db: PhantomDB instance
            target_version: Target version to migrate to (None for latest)
        """
        all_migrations = self.get_migrations()
        applied_migrations = self.get_applied_migrations()
        
        # Get filenames of applied migrations
        applied_filenames = {m['filename'] for m in applied_migrations if m['action'] == 'applied'}
        
        # Apply pending migrations
        for migration in all_migrations:
            if migration['filename'] not in applied_filenames:
                self.apply_migration(migration, db)
    
    def create_schema_file(self, db, filepath: str = "schema.json"):
        """Create a schema definition file from current database state.
        
        Args:
            db: PhantomDB instance
            filepath: Path to save schema file
        """
        try:
            # Get all databases
            databases = db.list_databases()
            
            schema = {
                "version": "1.0",
                "exported_at": datetime.now().isoformat(),
                "databases": {}
            }
            
            for db_name in databases:
                db.select_database(db_name)
                tables = db.list_tables()
                
                schema["databases"][db_name] = {
                    "tables": {}
                }
                
                for table_name in tables:
                    # Get table schema (this is a simplified example)
                    schema["databases"][db_name]["tables"][table_name] = {
                        "columns": [],  # Would need to implement actual schema extraction
                        "indexes": [],
                        "constraints": []
                    }
            
            # Save schema to file
            with open(filepath, 'w') as f:
                json.dump(schema, f, indent=2)
            
            print(f"Schema exported to: {filepath}")
            return filepath
        except Exception as e:
            print(f"Error exporting schema: {e}")
            raise


if __name__ == "__main__":
    # Example usage
    manager = MigrationManager()
    print("Migration Manager initialized")