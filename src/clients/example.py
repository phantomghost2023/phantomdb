"""
Example usage of the PhantomDB Python client.
"""

from phantomdb import PhantomDB, PhantomDBError


def main():
    """Demonstrate PhantomDB Python client usage."""
    # Connect to PhantomDB server
    db = PhantomDB("http://localhost:8080")
    
    try:
        # Check server health
        print("Checking server health...")
        health = db.health_check()
        print(f"Server health: {health}")
        
        # Create a database
        print("\nCreating database 'example_db'...")
        result = db.create_database("example_db")
        print(f"Create database result: {result}")
        
        # List databases
        print("\nListing databases...")
        databases = db.list_databases()
        print(f"Databases: {databases}")
        
        # Create a table
        print("\nCreating table 'users'...")
        columns = [
            {"name": "id", "type": "int"},
            {"name": "name", "type": "string"},
            {"name": "email", "type": "string"}
        ]
        result = db.create_table("example_db", "users", columns)
        print(f"Create table result: {result}")
        
        # List tables
        print("\nListing tables...")
        tables = db.list_tables("example_db")
        print(f"Tables: {tables}")
        
        # Insert data
        print("\nInserting data...")
        user_data = {
            "id": "1",
            "name": "John Doe",
            "email": "john@example.com"
        }
        result = db.insert("example_db", "users", user_data)
        print(f"Insert result: {result}")
        
        # Insert another record
        user_data2 = {
            "id": "2",
            "name": "Jane Smith",
            "email": "jane@example.com"
        }
        result = db.insert("example_db", "users", user_data2)
        print(f"Insert result: {result}")
        
        # Query data
        print("\nQuerying data...")
        users = db.select("example_db", "users")
        print(f"Users: {users}")
        
        # Update data
        print("\nUpdating data...")
        result = db.update("example_db", "users", {"name": "Jane Doe"}, "id = 2")
        print(f"Update result: {result}")
        
        # Query updated data
        print("\nQuerying updated data...")
        users = db.select("example_db", "users")
        print(f"Users: {users}")
        
        # Delete data
        print("\nDeleting data...")
        result = db.delete("example_db", "users", "id = 1")
        print(f"Delete result: {result}")
        
        # Query remaining data
        print("\nQuerying remaining data...")
        users = db.select("example_db", "users")
        print(f"Users: {users}")
        
        # Demonstrate transactions
        print("\nDemonstrating transactions...")
        txn_id = db.begin_transaction()
        print(f"Transaction ID: {txn_id}")
        
        try:
            # Insert data within transaction
            user_data3 = {
                "id": "3",
                "name": "Bob Johnson",
                "email": "bob@example.com"
            }
            result = db.insert("example_db", "users", user_data3)
            print(f"Insert in transaction result: {result}")
            
            # Commit the transaction
            result = db.commit_transaction(txn_id)
            print(f"Commit result: {result}")
        except Exception as e:
            # Rollback on error
            print(f"Error occurred, rolling back: {e}")
            db.rollback_transaction(txn_id)
        
        # Final query
        print("\nFinal data query...")
        users = db.select("example_db", "users")
        print(f"Users: {users}")
        
        # Drop table
        print("\nDropping table 'users'...")
        result = db.drop_table("example_db", "users")
        print(f"Drop table result: {result}")
        
        # Drop database
        print("\nDropping database 'example_db'...")
        result = db.drop_database("example_db")
        print(f"Drop database result: {result}")
        
    except PhantomDBError as e:
        print(f"PhantomDB error: {e}")
    except Exception as e:
        print(f"Unexpected error: {e}")


if __name__ == "__main__":
    main()