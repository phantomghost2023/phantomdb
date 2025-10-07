"""
PhantomDB Migration: create_example_table
Created: 2023-01-01 12:00:00
Description: Create an example table for demonstration
"""

def upgrade(db):
    """Apply this migration.
    
    Args:
        db: PhantomDB instance
    """
    # Create an example table
    db.create_table("example_table", {
        "id": "integer",
        "name": "string",
        "created_at": "timestamp"
    })
    
    # Create an index on the name column
    db.create_index("example_table", "idx_name", ["name"])

def downgrade(db):
    """Revert this migration.
    
    Args:
        db: PhantomDB instance
    """
    # Drop the example table
    db.drop_table("example_table")