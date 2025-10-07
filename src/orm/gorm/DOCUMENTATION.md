# GORM Dialect for PhantomDB

This document provides comprehensive documentation for the GORM dialect implementation for PhantomDB.

## Overview

The GORM dialect for PhantomDB allows GORM applications to use PhantomDB as their database backend through its REST API. This integration provides a seamless experience for GORM developers while leveraging PhantomDB's advanced features.

## Architecture

The integration follows GORM's dialect architecture:

```
GORM ORM
    ↓
PhantomDB Dialect
    ↓
PhantomDB Client (phantomdb-go)
    ↓
PhantomDB REST API
    ↓
PhantomDB Storage Engine
```

## Components

### 1. PhantomDBDialect

The main dialect struct that implements GORM's dialect interface.

#### Key Features:
- Connection management to PhantomDB
- Data type mapping from GORM to PhantomDB
- Query compilation and translation
- Transaction management

#### Methods:
- `Name()`: Returns the dialect name
- `Initialize()`: Initializes the dialect
- `DataTypeOf()`: Returns the data type for a field
- And many other GORM dialect methods

### 2. phantomDBDriver

SQL driver that implements Go's `driver.Driver` interface.

#### Key Features:
- Connection creation to PhantomDB
- DSN parsing
- Client initialization

### 3. phantomDBConn

Connection that implements Go's `driver.Conn` interface.

#### Key Features:
- Statement preparation
- Transaction management
- Connection closing

### 4. phantomDBStmt

Prepared statement that implements Go's `driver.Stmt` interface.

#### Key Features:
- Query execution
- Parameter binding
- Statement closing

### 5. phantomDBTx

Transaction that implements Go's `driver.Tx` interface.

#### Key Features:
- Transaction commit
- Transaction rollback

## Data Type Mapping

The dialect automatically maps GORM field types to PhantomDB types:

| GORM Type | PhantomDB Type | Notes |
|-----------|----------------|-------|
| bool | BOOLEAN | True/False values |
| int | INTEGER | 32-bit signed integer |
| int8 | TINYINT | 8-bit signed integer |
| int16 | SMALLINT | 16-bit signed integer |
| int32 | INTEGER | 32-bit signed integer |
| int64 | BIGINT | 64-bit signed integer |
| uint | INTEGER | 32-bit unsigned integer |
| uint8 | TINYINT | 8-bit unsigned integer |
| uint16 | SMALLINT | 16-bit unsigned integer |
| uint32 | INTEGER | 32-bit unsigned integer |
| uint64 | BIGINT | 64-bit unsigned integer |
| float32 | FLOAT | Single-precision floating point |
| float64 | DOUBLE | Double-precision floating point |
| string | VARCHAR(n) | Variable-length character string |
| time.Time | TIMESTAMP | Date and time with timezone |
| []byte | BLOB | Binary large object |

## Query Translation

The dialect translates GORM queries to PhantomDB REST API calls:

### SELECT Queries
```go
// GORM
var users []User
db.Where("age > ?", 18).Find(&users)

// Translated to PhantomDB API call
GET /databases/myapp/tables/users?condition=age>18
```

### INSERT Queries
```go
// GORM
user := User{FirstName: "John", LastName: "Doe"}
db.Create(&user)

// Translated to PhantomDB API call
POST /databases/myapp/tables/users
{
  "FirstName": "John",
  "LastName": "Doe"
}
```

### UPDATE Queries
```go
// GORM
db.Model(&user).Where("id = ?", 1).Update("FirstName", "Jane")

// Translated to PhantomDB API call
PUT /databases/myapp/tables/users
{
  "data": {"FirstName": "Jane"},
  "condition": "id=1"
}
```

### DELETE Queries
```go
// GORM
db.Where("id = ?", 1).Delete(&User{})

// Translated to PhantomDB API call
DELETE /databases/myapp/tables/users?condition=id=1
```

## Transaction Support

The dialect supports GORM's transaction management:

```go
func TransferMoney(db *gorm.DB, fromID, toID int, amount float64) error {
    return db.Transaction(func(tx *gorm.DB) error {
        // Find and update sender
        var sender Account
        if err := tx.First(&sender, fromID).Error; err != nil {
            return err
        }
        
        if sender.Balance < amount {
            return errors.New("insufficient balance")
        }
        
        tx.Model(&sender).Update("Balance", sender.Balance-amount)
        
        // Find and update receiver
        var receiver Account
        if err := tx.First(&receiver, toID).Error; err != nil {
            return err
        }
        
        tx.Model(&receiver).Update("Balance", receiver.Balance+amount)
        
        return nil
    })
}
```

This translates to:
1. `POST /transactions` - Begin transaction
2. Execute queries with transaction context
3. `POST /transactions/{id}/commit` - Commit transaction
4. Or `POST /transactions/{id}/rollback` - Rollback on error

## Configuration

### Database Connection

Create a GORM database connection with the PhantomDB dialect:

```go
import (
    "gorm.io/gorm"
    "phantomdb-gorm"
)

// Open database connection with PhantomDB
db, err := gorm.Open(phantomdb.Open("http://localhost:8080/myapp"), &gorm.Config{})
if err != nil {
    panic("failed to connect database")
}
```

### Connection Parameters

The dialect supports various connection parameters:

```go
// With authentication
db, err := gorm.Open(phantomdb.Open("http://user:password@localhost:8080/myapp"), &gorm.Config{})

// With connection options
db, err := gorm.Open(phantomdb.Open("http://localhost:8080/myapp?timeout=30"), &gorm.Config{})
```

## Usage Examples

### Model Definition

```go
import (
    "gorm.io/gorm"
    "phantomdb-gorm"
)

type User struct {
    ID        uint   `gorm:"primaryKey"`
    FirstName string
    LastName  string
    Email     string `gorm:"unique"`
    Age       int
    Active    bool   `gorm:"default:true"`
    CreatedAt time.Time
}

type Profile struct {
    ID        uint   `gorm:"primaryKey"`
    UserID    uint
    Bio       string
    Website   string
    CreatedAt time.Time
}

// Define table names
func (User) TableName() string {
    return "users"
}

func (Profile) TableName() string {
    return "profiles"
}
```

### Basic Operations

```go
import (
    "gorm.io/gorm"
    "phantomdb-gorm"
)

// Open database connection
db, err := gorm.Open(phantomdb.Open("http://localhost:8080/myapp"), &gorm.Config{})
if err != nil {
    panic("failed to connect database")
}

// Migrate the schema
db.AutoMigrate(&User{}, &Profile{})

// Create
user := User{FirstName: "John", LastName: "Doe", Email: "john.doe@example.com", Age: 30}
result := db.Create(&user) // pass pointer of data to Create

if result.Error != nil {
    log.Fatal("Failed to create user:", result.Error)
}

// Read
var user User
db.First(&user, 1) // find user with integer primary key
db.First(&user, "email = ?", "john.doe@example.com") // find user with email

// Update
db.Model(&user).Where("active = ?", true).Update("Age", 35)
// Update multiple fields
db.Model(&user).Updates(User{Age: 35, Active: false}) // non-zero fields
db.Model(&user).Updates(map[string]interface{}{"Age": 35, "Active": false})

// Delete
db.Delete(&user, 1)
```

### Advanced Queries

```go
// Complex filtering
var users []User
db.Where("age >= ? AND age <= ? AND email LIKE ?", 18, 65, "%@example.com").
   Not("active = ?", false).
   Order("created_at desc").
   Find(&users)

// Joins
var usersWithProfiles []struct {
    User
    Profile
}
db.Joins("Profile").Find(&usersWithProfiles)

// Preloading
var users []User
db.Preload("Profile").Find(&users)

// Aggregation
var count int64
db.Model(&User{}).Where("active = ?", true).Count(&count)

var averageAge float64
db.Model(&User{}).Select("AVG(age)").Scan(&averageAge)
```

## Error Handling

The dialect provides proper error handling:

```go
import (
    "errors"
    "gorm.io/gorm"
)

func CreateUser(db *gorm.DB, user *User) error {
    result := db.Create(user)
    if result.Error != nil {
        // Handle PhantomDB-specific errors
        if errors.Is(result.Error, gorm.ErrDuplicatedKey) {
            return errors.New("user already exists")
        }
        return fmt.Errorf("failed to create user: %w", result.Error)
    }
    return nil
}
```

## Performance Considerations

### Connection Management

The dialect reuses connections efficiently:

```go
// Recommended: Configure connection pool
db, err := gorm.Open(phantomdb.Open("http://localhost:8080/myapp"), &gorm.Config{})
if err != nil {
    panic("failed to connect database")
}

// Get the underlying SQL DB to configure connection pool
sqlDB, err := db.DB()
if err != nil {
    panic("failed to get SQL DB")
}

// SetMaxIdleConns sets the maximum number of connections in the idle connection pool.
sqlDB.SetMaxIdleConns(10)

// SetMaxOpenConns sets the maximum number of open connections to the database.
sqlDB.SetMaxOpenConns(100)

// SetConnMaxLifetime sets the maximum amount of time a connection may be reused.
sqlDB.SetConnMaxLifetime(time.Hour)
```

### Query Optimization

Use GORM's query optimization features:

```go
// Efficient: Select only needed fields
var names []string
db.Model(&User{}).Pluck("first_name", &names)

// Efficient: Batch operations
users := []User{
    {FirstName: "John", LastName: "Doe"},
    {FirstName: "Jane", LastName: "Smith"},
}
db.CreateInBatches(users, 100)
```

## Testing

The dialect includes comprehensive tests:

```go
// gorm_test.go
package gorm_test

import (
    "testing"
    "gorm.io/gorm"
    "phantomdb-gorm"
)

func TestUserCRUD(t *testing.T) {
    db, err := gorm.Open(phantomdb.Open("http://localhost:8080/testdb"), &gorm.Config{})
    if err != nil {
        t.Fatalf("failed to connect database: %v", err)
    }
    
    // Test create
    user := User{FirstName: "Test", LastName: "User"}
    result := db.Create(&user)
    if result.Error != nil {
        t.Fatalf("failed to create user: %v", result.Error)
    }
    
    // Test read
    var retrievedUser User
    result = db.First(&retrievedUser, user.ID)
    if result.Error != nil {
        t.Fatalf("failed to find user: %v", result.Error)
    }
    
    // Test update
    result = db.Model(&retrievedUser).Update("FirstName", "Updated")
    if result.Error != nil {
        t.Fatalf("failed to update user: %v", result.Error)
    }
    
    // Test delete
    result = db.Delete(&retrievedUser)
    if result.Error != nil {
        t.Fatalf("failed to delete user: %v", result.Error)
    }
}
```

## Migration Support

The dialect supports GORM's migration tools:

```go
// Auto migrate the schema
db.AutoMigrate(&User{}, &Profile{})

// Migrate specific models
db.Migrator().CreateTable(&User{})
db.Migrator().DropTable(&User{})
db.Migrator().HasTable(&User{})
db.Migrator().RenameTable(&User{}, &NewUser{})
```

Migrations are translated to PhantomDB schema operations:
- `CreateTable` becomes PhantomDB table creation
- `DropTable` becomes PhantomDB table deletion
- `AddColumn` becomes PhantomDB column addition
- `DropColumn` becomes PhantomDB column removal

## Security Considerations

### SQL Injection Prevention

The dialect uses parameterized queries to prevent SQL injection:

```go
// Safe: Parameters are properly escaped
var user User
db.First(&user, "email = ?", userInput)

// Unsafe: Direct string concatenation (avoid)
// db.Raw("SELECT * FROM users WHERE email = '" + userInput + "'").Scan(&user)
```

### Authentication

Configure authentication in the connection string:

```go
db, err := gorm.Open(phantomdb.Open("http://username:password@localhost:8080/myapp"), &gorm.Config{})
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
   failed to connect database: Failed to connect to PhantomDB server
   ```
   Solution: Check PhantomDB server status and connection settings

2. **Authentication Errors**
   ```
   failed to connect database: Unauthorized access
   ```
   Solution: Verify authentication credentials

3. **Schema Mismatch**
   ```
   Error 1146: Table 'users' doesn't exist
   ```
   Solution: Run migrations to create tables

### Debugging

Enable debug logging:

```go
import (
    "gorm.io/gorm/logger"
)

// Enable GORM logging
db, err := gorm.Open(phantomdb.Open("http://localhost:8080/myapp"), &gorm.Config{
    Logger: logger.Default.LogMode(logger.Info),
})
```

## API Reference

### PhantomDBDialect

```go
type PhantomDBDialect struct {
    // Contains filtered or unexported fields
}

func (PhantomDBDialect) Name() string
func (PhantomDBDialect) Initialize(db *gorm.DB) error
func (PhantomDBDialect) DataTypeOf(field *schema.Field) string
```

### phantomDBDriver

```go
type phantomDBDriver struct {
    // Contains filtered or unexported fields
}

func (d *phantomDBDriver) Open(dsn string) (driver.Conn, error)
```

### phantomDBConn

```go
type phantomDBConn struct {
    // Contains filtered or unexported fields
}

func (c *phantomDBConn) Prepare(query string) (driver.Stmt, error)
func (c *phantomDBConn) Close() error
func (c *phantomDBConn) Begin() (driver.Tx, error)
```

## Contributing

### Development Setup

1. Install dependencies:
   ```bash
   go mod tidy
   ```

2. Run tests:
   ```bash
   go test ./...
   ```

3. Run example:
   ```bash
   go run example.go
   ```

### Code Structure

```
src/orm/gorm/
├── go.mod          # Module definition
├── gorm.go         # Main implementation
├── README.md       # Documentation
├── example.go      # Usage examples
└── gorm_test.go    # Tests
```

## License

MIT License

## Support

For issues and feature requests, please open an issue on the GitHub repository.