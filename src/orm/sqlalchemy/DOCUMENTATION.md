# SQLAlchemy Dialect for PhantomDB

This document provides comprehensive documentation for the SQLAlchemy dialect implementation for PhantomDB.

## Overview

The SQLAlchemy dialect for PhantomDB allows SQLAlchemy applications to use PhantomDB as their database backend through its REST API. This integration provides a seamless experience for SQLAlchemy developers while leveraging PhantomDB's advanced features.

## Architecture

The integration follows SQLAlchemy's dialect architecture:

```
SQLAlchemy ORM/Core
    ↓
PhantomDB Dialect
    ↓
PhantomDB Client (phantomdb)
    ↓
PhantomDB REST API
    ↓
PhantomDB Storage Engine
```

## Components

### 1. PhantomDBDialect

The main dialect class that implements SQLAlchemy's dialect interface.

#### Key Features:
- Connection management to PhantomDB
- Data type mapping from SQLAlchemy to PhantomDB
- Query compilation and translation
- Transaction management

#### Methods:
- `initialize()`: Initialize the dialect with a connection
- `do_execute()`: Execute SQL statements
- `has_table()`: Check if a table exists
- `get_columns()`: Get column information
- And many more SQLAlchemy dialect methods

### 2. PhantomDBCompiler

SQL compiler that translates SQLAlchemy expressions to SQL.

#### Key Features:
- Bind parameter handling
- Column reference handling
- SELECT statement compilation
- LIMIT/OFFSET support

### 3. PhantomDBTypeCompiler

Type compiler that maps SQLAlchemy types to PhantomDB types.

#### Key Features:
- Data type mapping
- Type-specific compilation

### 4. PhantomDBExecutionContext

Execution context that manages query execution.

#### Key Features:
- Cursor management
- Result handling
- Transaction context

## Data Type Mapping

The dialect automatically maps SQLAlchemy field types to PhantomDB types:

| SQLAlchemy Type | PhantomDB Type | Notes |
|----------------|----------------|-------|
| Integer | INTEGER | 32-bit signed integer |
| BigInteger | BIGINT | 64-bit signed integer |
| Boolean | BOOLEAN | True/False values |
| CHAR(n) | CHAR(n) | Fixed-length character string |
| VARCHAR(n) | VARCHAR(n) | Variable-length character string |
| Text | TEXT | Long text storage |
| Float | FLOAT | Single-precision floating point |
| Double | DOUBLE | Double-precision floating point |
| DECIMAL(p, s) | DECIMAL(p, s) | Fixed-point decimal |
| DateTime | TIMESTAMP | Date and time with timezone |
| Date | DATE | Date only |
| Time | TIME | Time only |
| JSON | JSON | JSON document storage |

## Query Translation

The dialect translates SQLAlchemy queries to PhantomDB REST API calls:

### SELECT Queries
```python
# SQLAlchemy
session.query(User).filter(User.age > 18).all()

# Translated to PhantomDB API call
GET /databases/myapp/tables/users?condition=age>18
```

### INSERT Queries
```python
# SQLAlchemy
user = User(first_name='John', last_name='Doe')
session.add(user)
session.commit()

# Translated to PhantomDB API call
POST /databases/myapp/tables/users
{
  "first_name": "John",
  "last_name": "Doe"
}
```

### UPDATE Queries
```python
# SQLAlchemy
session.query(User).filter(User.id == 1).update({User.first_name: 'Jane'})
session.commit()

# Translated to PhantomDB API call
PUT /databases/myapp/tables/users
{
  "data": {"first_name": "Jane"},
  "condition": "id=1"
}
```

### DELETE Queries
```python
# SQLAlchemy
session.query(User).filter(User.id == 1).delete()
session.commit()

# Translated to PhantomDB API call
DELETE /databases/myapp/tables/users?condition=id=1
```

## Transaction Support

The dialect supports SQLAlchemy's transaction management:

```python
from sqlalchemy.orm import sessionmaker
from sqlalchemy import create_engine

engine = create_engine('phantomdb://localhost:8080/myapp')
Session = sessionmaker(bind=engine)
session = Session()

try:
    # Perform operations
    user = User(first_name='John', last_name='Doe')
    session.add(user)
    
    profile = Profile(user_id=user.id, bio='Software Developer')
    session.add(profile)
    
    # Commit transaction
    session.commit()
except Exception as e:
    # Rollback on error
    session.rollback()
    raise
finally:
    session.close()
```

This translates to:
1. `POST /transactions` - Begin transaction
2. Execute queries with transaction context
3. `POST /transactions/{id}/commit` - Commit transaction
4. Or `POST /transactions/{id}/rollback` - Rollback on error

## Configuration

### Engine Creation

Create a SQLAlchemy engine with the PhantomDB dialect:

```python
from sqlalchemy import create_engine

# Create engine with PhantomDB
engine = create_engine('phantomdb://localhost:8080/myapp')

# Or with explicit URL
engine = create_engine('phantomdb+rest://localhost:8080/myapp')
```

### Connection Parameters

The dialect supports various connection parameters:

```python
# With authentication
engine = create_engine('phantomdb://user:password@localhost:8080/myapp')

# With connection options
engine = create_engine('phantomdb://localhost:8080/myapp?pool_size=10&timeout=30')
```

## Usage Examples

### Model Definition

```python
from sqlalchemy import Column, Integer, String, Boolean, DateTime
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker

Base = declarative_base()

class User(Base):
    __tablename__ = 'users'
    
    id = Column(Integer, primary_key=True)
    first_name = Column(String(100))
    last_name = Column(String(100))
    email = Column(String(255), unique=True)
    is_active = Column(Boolean, default=True)
    created_at = Column(DateTime, default=datetime.utcnow)
    
    def __repr__(self):
        return f"<User(name='{self.first_name} {self.last_name}', email='{self.email}')>"

class Profile(Base):
    __tablename__ = 'profiles'
    
    id = Column(Integer, primary_key=True)
    user_id = Column(Integer)
    bio = Column(String(500))
    website = Column(String(255))
    created_at = Column(DateTime, default=datetime.utcnow)
```

### Basic Operations

```python
from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker

# Create engine and session
engine = create_engine('phantomdb://localhost:8080/myapp')
Session = sessionmaker(bind=engine)
session = Session()

# Create tables
Base.metadata.create_all(engine)

# Create
user = User(first_name='John', last_name='Doe', email='john.doe@example.com')
session.add(user)
session.commit()

# Read
all_users = session.query(User).all()
active_users = session.query(User).filter(User.is_active == True).all()
user = session.query(User).filter(User.email == 'john.doe@example.com').first()

# Update
session.query(User).filter(User.id == 1).update({User.first_name: 'Jane'})
session.commit()

# Delete
session.query(User).filter(User.id == 1).delete()
session.commit()
```

### Advanced Queries

```python
from sqlalchemy import and_, or_, not_, func

# Complex filtering
users = session.query(User).filter(
    and_(
        User.age >= 18,
        User.age <= 65,
        User.email.like('%@example.com')
    )
).filter(
    not_(User.is_active == False)
).order_by(User.created_at.desc()).all()

# Aggregation
user_count = session.query(func.count(User.id)).scalar()
average_age = session.query(func.avg(User.age)).scalar()

# Joins
users_with_profiles = session.query(User, Profile).join(
    Profile, User.id == Profile.user_id
).all()
```

## Error Handling

The dialect provides proper error handling:

```python
from sqlalchemy.exc import IntegrityError, DatabaseError
from base import PhantomDBError

try:
    user = User(email='duplicate@example.com')
    session.add(user)
    session.commit()
except IntegrityError as e:
    # Handle duplicate key errors
    session.rollback()
    print(f"User already exists: {e}")
except DatabaseError as e:
    # Handle other database errors
    session.rollback()
    print(f"Database error: {e}")
except PhantomDBError as e:
    # Handle PhantomDB-specific errors
    session.rollback()
    print(f"PhantomDB error: {e}")
```

## Performance Considerations

### Connection Management

The dialect reuses connections efficiently:

```python
# Recommended: Use connection pooling
from sqlalchemy import create_engine

engine = create_engine(
    'phantomdb://localhost:8080/myapp',
    pool_size=20,
    max_overflow=0,
    pool_timeout=30,
    pool_recycle=3600
)
```

### Query Optimization

Use SQLAlchemy's query optimization features:

```python
# Efficient: Load only needed columns
users = session.query(User.first_name, User.last_name).all()

# Efficient: Use subqueries
subquery = session.query(User.id).filter(User.is_active == True).subquery()
active_profiles = session.query(Profile).filter(Profile.user_id.in_(subquery)).all()
```

## Testing

The dialect includes comprehensive tests:

```python
# test_dialect.py
import unittest
from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker

class TestPhantomDBDialect(unittest.TestCase):
    def setUp(self):
        self.engine = create_engine('phantomdb://localhost:8080/testdb')
        self.Session = sessionmaker(bind=self.engine)
        self.session = self.Session()
        
    def test_connection(self):
        # Test that we can connect to PhantomDB
        result = self.engine.execute("SELECT 1")
        self.assertEqual(result.fetchone()[0], 1)
        
    def test_crud_operations(self):
        # Test create, read, update, delete operations
        user = User(first_name='Test', last_name='User')
        self.session.add(user)
        self.session.commit()
        
        retrieved_user = self.session.query(User).filter(User.first_name == 'Test').first()
        self.assertIsNotNone(retrieved_user)
        
        self.session.delete(retrieved_user)
        self.session.commit()
```

## Migration Support

The dialect supports SQLAlchemy's migration tools:

```bash
# Create migration
alembic revision --autogenerate -m "Create users table"

# Apply migration
alembic upgrade head
```

Migrations are translated to PhantomDB schema operations:
- `CREATE TABLE` becomes PhantomDB table creation
- `ALTER TABLE` becomes PhantomDB schema modifications
- `DROP TABLE` becomes PhantomDB table deletion

## Security Considerations

### SQL Injection Prevention

The dialect uses parameterized queries to prevent SQL injection:

```python
# Safe: Parameters are properly escaped
user = session.query(User).filter(User.email == user_input).first()

# Unsafe: Direct string concatenation (avoid)
# session.execute(f"SELECT * FROM users WHERE email = '{user_input}'")
```

### Authentication

Configure authentication in the connection string:

```python
engine = create_engine('phantomdb://username:password@localhost:8080/myapp')
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
   DatabaseError: Failed to connect to PhantomDB server
   ```
   Solution: Check PhantomDB server status and connection settings

2. **Authentication Errors**
   ```
   DatabaseError: Unauthorized access
   ```
   Solution: Verify authentication credentials

3. **Schema Mismatch**
   ```
   DatabaseError: Table 'users' does not exist
   ```
   Solution: Run migrations to create tables

### Debugging

Enable debug logging:

```python
import logging

# Enable SQLAlchemy logging
logging.basicConfig()
logging.getLogger('sqlalchemy.engine').setLevel(logging.INFO)

# Enable PhantomDB dialect logging
logging.getLogger('phantomdb_sqlalchemy').setLevel(logging.DEBUG)
```

## API Reference

### PhantomDBDialect

```python
class PhantomDBDialect(DefaultDialect):
    def __init__(self, **kwargs):
        """Initialize the dialect."""
        pass
    
    def initialize(self, connection):
        """Initialize the dialect with a connection."""
        pass
    
    def do_execute(self, cursor, statement, parameters, context=None):
        """Execute a SQL statement."""
        pass
    
    def has_table(self, connection, table_name, schema=None):
        """Check if a table exists."""
        pass
```

### PhantomDBCompiler

```python
class PhantomDBCompiler(SQLCompiler):
    def visit_bindparam(self, bindparam, **kwargs):
        """Handle bind parameters."""
        pass
    
    def visit_column(self, column, add_to_result_map=None, **kwargs):
        """Handle column references."""
        pass
```

### PhantomDBTypeCompiler

```python
class PhantomDBTypeCompiler(GenericTypeCompiler):
    def visit_INTEGER(self, type_, **kw):
        """Compile INTEGER type."""
        pass
    
    def visit_VARCHAR(self, type_, **kw):
        """Compile VARCHAR type."""
        pass
```

## Contributing

### Development Setup

1. Install dependencies:
   ```bash
   pip install -r requirements.txt
   ```

2. Run tests:
   ```bash
   python -m pytest tests/
   ```

3. Run example:
   ```bash
   python example.py
   ```

### Code Structure

```
src/orm/sqlalchemy/
├── __init__.py
├── base.py          # Main implementation
├── package.json     # Package configuration
├── requirements.txt # Dependencies
├── setup.py         # Installation script
├── README.md        # Documentation
├── example.py       # Usage examples
└── test.py          # Tests
```

## License

MIT License

## Support

For issues and feature requests, please open an issue on the GitHub repository.