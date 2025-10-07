# Diesel ORM Integration for PhantomDB

This document provides comprehensive documentation for the Diesel ORM integration implementation for PhantomDB.

## Overview

The Diesel ORM integration for PhantomDB allows Rust applications to use PhantomDB as their database backend through its REST API. This integration provides a seamless experience for Rust developers while leveraging PhantomDB's advanced features.

## Architecture

The integration follows Diesel's architecture:

```
Rust Application
    ↓
Diesel ORM
    ↓
PhantomDB Diesel Integration
    ↓
PhantomDB Client (phantomdb-rust)
    ↓
PhantomDB REST API
    ↓
PhantomDB Storage Engine
```

## Components

### 1. PhantomDBConnection

The main connection struct that implements Diesel's connection traits.

#### Key Features:
- Connection management to PhantomDB
- Query execution
- Transaction management

#### Methods:
- `establish()`: Establishes a connection to PhantomDB
- `execute()`: Executes a query
- Transaction methods

### 2. PhantomDBBackend

The backend struct that implements Diesel's backend traits.

#### Key Features:
- Backend-specific functionality
- Query building
- Result processing

### 3. QueryBuilder

Query builder for constructing SQL queries.

#### Key Features:
- SELECT query building
- WHERE clause construction
- Query composition

### 4. Type Mapping

Data type mapping from Rust types to PhantomDB types.

#### Key Features:
- Automatic type conversion
- Schema generation
- Type validation

## Data Type Mapping

The integration automatically maps Rust field types to PhantomDB types:

| Rust Type | PhantomDB Type | Notes |
|-----------|----------------|-------|
| i8 | TINYINT | 8-bit signed integer |
| i16 | SMALLINT | 16-bit signed integer |
| i32 | INTEGER | 32-bit signed integer |
| i64 | BIGINT | 64-bit signed integer |
| f32 | FLOAT | Single-precision floating point |
| f64 | DOUBLE | Double-precision floating point |
| bool | BOOLEAN | True/False values |
| String | VARCHAR(n) | Variable-length character string |
| chrono::NaiveDateTime | TIMESTAMP | Date and time without timezone |
| Vec<u8> | BLOB | Binary large object |

## Query Translation

The integration translates Diesel queries to PhantomDB REST API calls:

### SELECT Queries
```rust
// Diesel
let users = users::table
    .filter(users::age.gt(18))
    .load::<User>(&mut connection)?;

// Translated to PhantomDB API call
GET /databases/myapp/tables/users?condition=age>18
```

### INSERT Queries
```rust
// Diesel
let new_user = NewUser {
    first_name: "John".to_string(),
    last_name: "Doe".to_string(),
};
diesel::insert_into(users::table)
    .values(&new_user)
    .execute(&mut connection)?;

// Translated to PhantomDB API call
POST /databases/myapp/tables/users
{
  "first_name": "John",
  "last_name": "Doe"
}
```

### UPDATE Queries
```rust
// Diesel
diesel::update(users::table.filter(users::id.eq(1)))
    .set(users::first_name.eq("Jane"))
    .execute(&mut connection)?;

// Translated to PhantomDB API call
PUT /databases/myapp/tables/users
{
  "data": {"first_name": "Jane"},
  "condition": "id=1"
}
```

### DELETE Queries
```rust
// Diesel
diesel::delete(users::table.filter(users::id.eq(1)))
    .execute(&mut connection)?;

// Translated to PhantomDB API call
DELETE /databases/myapp/tables/users?condition=id=1
```

## Transaction Support

The integration supports Diesel's transaction management:

```rust
use diesel::Connection;

connection.transaction::<_, diesel::result::Error, _>(|conn| {
    // Create user
    diesel::insert_into(users::table)
        .values(&new_user)
        .execute(conn)?;
        
    // Create profile
    diesel::insert_into(profiles::table)
        .values(&new_profile)
        .execute(conn)?;
        
    Ok(())
})?;
```

This translates to:
1. `POST /transactions` - Begin transaction
2. Execute queries with transaction context
3. `POST /transactions/{id}/commit` - Commit transaction
4. Or `POST /transactions/{id}/rollback` - Rollback on error

## Configuration

### Database Connection

Establish a database connection with PhantomDB:

```rust
use phantomdb_diesel::PhantomDBConnection;
use diesel::prelude::*;

// Establish connection with PhantomDB
let connection = PhantomDBConnection::establish("http://localhost:8080/myapp")?;
```

### Connection Parameters

The integration supports various connection parameters:

```rust
// With authentication
let connection = PhantomDBConnection::establish("http://user:password@localhost:8080/myapp")?;

// With connection options
let connection = PhantomDBConnection::establish("http://localhost:8080/myapp?timeout=30")?;
```

## Usage Examples

### Model Definition

```rust
use diesel::prelude::*;

// Schema definition
table! {
    users (id) {
        id -> Integer,
        first_name -> Varchar,
        last_name -> Varchar,
        email -> Varchar,
        age -> Integer,
        active -> Bool,
        created_at -> Timestamp,
    }
}

// User model
#[derive(Queryable, Selectable)]
#[diesel(table_name = users)]
#[diesel(check_for_backend(diesel::pg::Pg))]
pub struct User {
    pub id: i32,
    pub first_name: String,
    pub last_name: String,
    pub email: String,
    pub age: i32,
    pub active: bool,
    pub created_at: chrono::NaiveDateTime,
}

// New user model for inserts
#[derive(Insertable)]
#[diesel(table_name = users)]
pub struct NewUser {
    pub first_name: String,
    pub last_name: String,
    pub email: String,
    pub age: i32,
    pub active: bool,
}
```

### Basic Operations

```rust
use diesel::prelude::*;
use phantomdb_diesel::PhantomDBConnection;

// Establish connection
let mut connection = PhantomDBConnection::establish("http://localhost:8080/myapp")?;

// Create
let new_user = NewUser {
    first_name: "John".to_string(),
    last_name: "Doe".to_string(),
    email: "john.doe@example.com".to_string(),
    age: 30,
    active: true,
};

diesel::insert_into(users::table)
    .values(&new_user)
    .execute(&mut connection)?;

// Read
let users = users::table
    .filter(users::age.gt(18))
    .load::<User>(&mut connection)?;

let user = users::table
    .find(1)
    .first::<User>(&mut connection)?;

// Update
diesel::update(users::table.filter(users::id.eq(1)))
    .set(users::age.eq(35))
    .execute(&mut connection)?;

// Delete
diesel::delete(users::table.filter(users::id.eq(1)))
    .execute(&mut connection)?;
```

### Advanced Queries

```rust
use diesel::prelude::*;

// Complex filtering
let users = users::table
    .filter(users::age.between(18, 65))
    .filter(users::email.like("%@example.com"))
    .filter(users::active.eq(true))
    .order(users::created_at.desc())
    .limit(10)
    .load::<User>(&mut connection)?;

// Joins
let users_with_profiles = users::table
    .inner_join(profiles::table)
    .load::<(User, Profile)>(&mut connection)?;

// Aggregations
let user_count: i64 = users::table
    .filter(users::active.eq(true))
    .count()
    .get_result(&mut connection)?;

let average_age: f64 = users::table
    .select(diesel::dsl::avg(users::age))
    .get_result(&mut connection)?
    .unwrap_or(0.0);
```

## Error Handling

The integration provides proper error handling:

```rust
use diesel::result::Error;
use phantomdb_diesel::errors::PhantomDBError;

fn create_user(connection: &mut PhantomDBConnection, user: &NewUser) -> Result<(), Box<dyn std::error::Error>> {
    match diesel::insert_into(users::table)
        .values(user)
        .execute(connection) {
        Ok(_) => Ok(()),
        Err(Error::DatabaseError(_, _)) => Err(Box::new(PhantomDBError::QueryError("Duplicate user".to_string()))),
        Err(e) => Err(Box::new(e)),
    }
}
```

## Performance Considerations

### Connection Management

The integration reuses connections efficiently:

```rust
use diesel::r2d2::{ConnectionManager, Pool};
use phantomdb_diesel::PhantomDBConnection;

// Create connection pool
let manager = ConnectionManager::<PhantomDBConnection>::new("http://localhost:8080/myapp");
let pool = Pool::builder()
    .max_size(15)
    .build(manager)?;

// Get connection from pool
let connection = pool.get()?;
```

### Query Optimization

Use Diesel's query optimization features:

```rust
// Efficient: Select only needed fields
let names: Vec<String> = users::table
    .select(users::first_name)
    .load(&mut connection)?;

// Efficient: Batch operations
let new_users = vec![new_user1, new_user2, new_user3];
diesel::insert_into(users::table)
    .values(&new_users)
    .execute(&mut connection)?;
```

## Testing

The integration includes comprehensive tests:

```rust
// lib.rs
#[cfg(test)]
mod tests {
    use super::*;
    
    #[test]
    fn test_user_crud() -> Result<(), Box<dyn std::error::Error>> {
        let mut connection = PhantomDBConnection::establish("http://localhost:8080/testdb")?;
        
        // Test create
        let new_user = NewUser {
            first_name: "Test".to_string(),
            last_name: "User".to_string(),
            email: "test@example.com".to_string(),
            age: 25,
            active: true,
        };
        
        let result = diesel::insert_into(users::table)
            .values(&new_user)
            .execute(&mut connection);
        assert!(result.is_ok());
        
        // Test read
        let user = users::table
            .filter(users::email.eq("test@example.com"))
            .first::<User>(&mut connection);
        assert!(user.is_ok());
        
        // Test update
        let updated_rows = diesel::update(users::table.filter(users::id.eq(user?.id)))
            .set(users::age.eq(30))
            .execute(&mut connection)?;
        assert_eq!(updated_rows, 1);
        
        // Test delete
        let deleted_rows = diesel::delete(users::table.filter(users::id.eq(user?.id)))
            .execute(&mut connection)?;
        assert_eq!(deleted_rows, 1);
        
        Ok(())
    }
}
```

## Migration Support

The integration supports Diesel's migration tools:

```bash
# Generate migration
diesel migration generate create_users_table

# Run migrations
diesel migration run

# Revert migration
diesel migration revert
```

Migrations are translated to PhantomDB schema operations:
- `CREATE TABLE` becomes PhantomDB table creation
- `ALTER TABLE` becomes PhantomDB schema modifications
- `DROP TABLE` becomes PhantomDB table deletion

## Security Considerations

### SQL Injection Prevention

The integration uses parameterized queries to prevent SQL injection:

```rust
// Safe: Parameters are properly escaped
let user = users::table
    .filter(users::email.eq(user_input))
    .first::<User>(&mut connection)?;

// Unsafe: Direct string concatenation (avoid)
// diesel::sql_query(format!("SELECT * FROM users WHERE email = '{}'", user_input))
```

### Authentication

Configure authentication in the connection string:

```rust
let connection = PhantomDBConnection::establish("http://username:password@localhost:8080/myapp")?;
```

## Limitations

### Current Limitations

1. **Raw SQL Support**: Limited support for complex raw SQL queries
2. **Advanced Aggregations**: Some advanced aggregation functions may not be supported
3. **Complex Joins**: Advanced JOIN operations may have limitations
4. **Stored Procedures**: PhantomDB doesn't support stored procedures

### Planned Enhancements

1. **Connection Pooling**: Implement connection pooling for better performance
2. **Caching Layer**: Add query result caching
3. **Advanced Indexing**: Support for advanced indexing strategies
4. **Streaming Results**: Support for streaming large result sets

## Troubleshooting

### Common Issues

1. **Connection Errors**
   ```
   Connection error: Failed to connect to PhantomDB server
   ```
   Solution: Check PhantomDB server status and connection settings

2. **Authentication Errors**
   ```
   Connection error: Unauthorized access
   ```
   Solution: Verify authentication credentials

3. **Schema Mismatch**
   ```
   Error: Table 'users' doesn't exist
   ```
   Solution: Run migrations to create tables

### Debugging

Enable debug logging:

```rust
use std::env;

// Enable logging
env::set_var("RUST_LOG", "phantomdb_diesel=debug");
env_logger::init();
```

## API Reference

### PhantomDBConnection

```rust
pub struct PhantomDBConnection {
    // Contains filtered or unexported fields
}

impl PhantomDBConnection {
    pub fn establish(database_url: &str) -> Result<Self, Box<dyn std::error::Error>>
    pub fn execute(&self, query: &str) -> Result<usize, Box<dyn std::error::Error>>
}
```

### PhantomDBBackend

```rust
pub struct PhantomDBBackend;

impl PhantomDBBackend {
    pub fn new() -> Self
}
```

### QueryBuilder

```rust
pub struct PhantomDBQueryBuilder {
    // Contains filtered or unexported fields
}

impl QueryBuilder for PhantomDBQueryBuilder {
    fn select(&mut self, table: &str) -> &mut Self
    fn filter(&mut self, condition: &str) -> &mut Self
    fn build(&self) -> String
}
```

## Contributing

### Development Setup

1. Install dependencies:
   ```bash
   cargo build
   ```

2. Run tests:
   ```bash
   cargo test
   ```

3. Run example:
   ```bash
   cargo run --example example
   ```

### Code Structure

```
src/orm/diesel/
├── Cargo.toml      # Package definition
├── lib.rs          # Main implementation
├── README.md       # Documentation
├── example.rs      # Usage examples
└── DOCUMENTATION.md # Comprehensive documentation
```

## License

MIT License

## Support

For issues and feature requests, please open an issue on the GitHub repository.