# Django ORM Integration for PhantomDB

This document provides comprehensive documentation for the Django ORM integration with PhantomDB.

## Overview

The Django ORM integration allows Django applications to use PhantomDB as their database backend through its REST API. This integration provides a seamless experience for Django developers while leveraging PhantomDB's advanced features.

## Architecture

The integration follows Django's database backend architecture:

```
Django ORM
    ↓
DatabaseWrapper (phantomdb_django.base)
    ↓
PhantomDB Client (phantomdb)
    ↓
PhantomDB REST API
    ↓
PhantomDB Storage Engine
```

## Components

### 1. DatabaseWrapper

The main entry point that implements Django's database backend interface.

#### Key Features:
- Connection management to PhantomDB
- Data type mapping from Django to PhantomDB
- Query operator translation
- Transaction management

#### Methods:
- `get_connection_params()`: Extract connection parameters from Django settings
- `get_new_connection()`: Create a new PhantomDB client connection
- `create_cursor()`: Create a cursor for executing queries
- `is_usable()`: Check if the connection is healthy

### 2. PhantomDBCursor

Implements Django's cursor interface for executing queries.

#### Key Features:
- SQL query execution
- Result set management
- Row fetching methods
- Parameter substitution

#### Methods:
- `execute()`: Execute a single SQL query
- `executemany()`: Execute a query multiple times
- `fetchone()`: Fetch a single row
- `fetchmany()`: Fetch multiple rows
- `fetchall()`: Fetch all remaining rows

## Data Type Mapping

The integration automatically maps Django field types to PhantomDB types:

| Django Field | PhantomDB Type | Notes |
|--------------|----------------|-------|
| AutoField | INTEGER PRIMARY KEY | Auto-incrementing integer |
| BigAutoField | BIGINT PRIMARY KEY | Large auto-incrementing integer |
| BinaryField | BLOB | Binary data storage |
| BooleanField | BOOLEAN | True/False values |
| CharField | VARCHAR(n) | Variable character string |
| DateField | DATE | Date values |
| DateTimeField | TIMESTAMP | Date and time values |
| DecimalField | DECIMAL(p, s) | Fixed-point decimal numbers |
| DurationField | BIGINT | Time duration in microseconds |
| FileField | VARCHAR(n) | File path storage |
| FilePathField | VARCHAR(n) | File path validation |
| FloatField | DOUBLE | Floating-point numbers |
| IntegerField | INTEGER | 32-bit integers |
| BigIntegerField | BIGINT | 64-bit integers |
| IPAddressField | VARCHAR(15) | IP address storage |
| GenericIPAddressField | VARCHAR(39) | IPv4/IPv6 address storage |
| JSONField | JSON | JSON document storage |
| NullBooleanField | BOOLEAN | Boolean with NULL support |
| OneToOneField | INTEGER | Foreign key reference |
| PositiveBigIntegerField | BIGINT | Positive 64-bit integer |
| PositiveIntegerField | INTEGER | Positive 32-bit integer |
| PositiveSmallIntegerField | SMALLINT | Positive 16-bit integer |
| SlugField | VARCHAR(n) | URL-friendly string |
| SmallAutoField | SMALLINT PRIMARY KEY | Small auto-incrementing integer |
| SmallIntegerField | SMALLINT | 16-bit integers |
| TextField | TEXT | Long text storage |
| TimeField | TIME | Time values |
| UUIDField | VARCHAR(32) | UUID storage |

## Query Translation

The integration translates Django ORM queries to PhantomDB REST API calls:

### SELECT Queries
```python
# Django ORM
User.objects.filter(age__gt=18)

# Translated to PhantomDB API call
GET /databases/myapp/tables/users?condition=age>18
```

### INSERT Queries
```python
# Django ORM
User.objects.create(first_name='John', last_name='Doe')

# Translated to PhantomDB API call
POST /databases/myapp/tables/users
{
  "first_name": "John",
  "last_name": "Doe"
}
```

### UPDATE Queries
```python
# Django ORM
User.objects.filter(id=1).update(first_name='Jane')

# Translated to PhantomDB API call
PUT /databases/myapp/tables/users
{
  "data": {"first_name": "Jane"},
  "condition": "id=1"
}
```

### DELETE Queries
```python
# Django ORM
User.objects.filter(id=1).delete()

# Translated to PhantomDB API call
DELETE /databases/myapp/tables/users?condition=id=1
```

## Operators Support

The integration supports Django's query operators:

| Operator | Django Syntax | PhantomDB Translation |
|----------|---------------|----------------------|
| exact | `name='John'` | `name='John'` |
| iexact | `name__iexact='john'` | `ILIKE 'john'` |
| contains | `name__contains='oh'` | `LIKE '%oh%'` |
| icontains | `name__icontains='oh'` | `ILIKE '%oh%'` |
| gt | `age__gt=18` | `age>18` |
| gte | `age__gte=18` | `age>=18` |
| lt | `age__lt=65` | `age<65` |
| lte | `age__lte=65` | `age<=65` |
| startswith | `name__startswith='J'` | `LIKE 'J%'` |
| endswith | `name__endswith='n'` | `LIKE '%n'` |

## Transaction Support

The integration supports Django's transaction management:

```python
from django.db import transaction

with transaction.atomic():
    user = User.objects.create(first_name='John', last_name='Doe')
    profile = Profile.objects.create(user=user, bio='Developer')
```

This translates to:
1. `POST /transactions` - Begin transaction
2. Execute queries with transaction context
3. `POST /transactions/{id}/commit` - Commit transaction
4. Or `POST /transactions/{id}/rollback` - Rollback on error

## Configuration

### Django Settings

Configure the database backend in your Django settings:

```python
# settings.py
DATABASES = {
    'default': {
        'ENGINE': 'phantomdb_django',
        'NAME': 'myapp',
        'HOST': 'http://localhost:8080',  # PhantomDB server URL
        'PORT': '',  # Not used
        'USER': '',  # Not used
        'PASSWORD': '',  # Not used
    }
}
```

### Environment Variables

The integration can also use environment variables:

```python
# settings.py
import os

DATABASES = {
    'default': {
        'ENGINE': 'phantomdb_django',
        'NAME': os.environ.get('PHANTOMDB_NAME', 'myapp'),
        'HOST': os.environ.get('PHANTOMDB_HOST', 'http://localhost:8080'),
    }
}
```

## Usage Examples

### Model Definition

```python
# models.py
from django.db import models

class User(models.Model):
    first_name = models.CharField(max_length=100)
    last_name = models.CharField(max_length=100)
    email = models.EmailField(unique=True)
    age = models.IntegerField(default=0)
    is_active = models.BooleanField(default=True)
    created_at = models.DateTimeField(auto_now_add=True)
    
    class Meta:
        db_table = 'users'

class Profile(models.Model):
    user = models.OneToOneField(User, on_delete=models.CASCADE)
    bio = models.TextField(blank=True)
    website = models.URLField(blank=True)
    created_at = models.DateTimeField(auto_now_add=True)
    
    class Meta:
        db_table = 'profiles'
```

### Basic Operations

```python
# Create
user = User.objects.create(
    first_name='John',
    last_name='Doe',
    email='john.doe@example.com',
    age=30
)

# Read
all_users = User.objects.all()
active_users = User.objects.filter(is_active=True)
user = User.objects.get(email='john.doe@example.com')

# Update
User.objects.filter(age__lt=18).update(is_active=False)
User.objects.filter(pk=1).update(first_name='Jane')

# Delete
User.objects.filter(is_active=False).delete()
```

### Advanced Queries

```python
# Complex filtering
users = User.objects.filter(
    age__gte=18,
    age__lte=65,
    email__icontains='@example.com'
).exclude(
    is_active=False
).order_by('-created_at')

# Aggregation (if supported)
from django.db.models import Count, Avg
User.objects.aggregate(
    total=Count('id'),
    average_age=Avg('age')
)

# Joins
users_with_profiles = User.objects.select_related('profile').all()
```

## Error Handling

The integration provides proper error handling:

```python
from django.db import IntegrityError, DatabaseError
from phantomdb import PhantomDBError

try:
    user = User.objects.create(email='duplicate@example.com')
except IntegrityError as e:
    # Handle duplicate key errors
    print(f"User already exists: {e}")
except DatabaseError as e:
    # Handle other database errors
    print(f"Database error: {e}")
except PhantomDBError as e:
    # Handle PhantomDB-specific errors
    print(f"PhantomDB error: {e}")
```

## Performance Considerations

### Connection Management

The integration reuses connections efficiently:

```python
# Recommended: Use Django's connection pooling
DATABASES = {
    'default': {
        'ENGINE': 'phantomdb_django',
        'NAME': 'myapp',
        'HOST': 'http://localhost:8080',
        'CONN_MAX_AGE': 600,  # Connection lifetime in seconds
    }
}
```

### Query Optimization

Use Django's select_related and prefetch_related for efficient queries:

```python
# Efficient: Single query with join
users = User.objects.select_related('profile').all()

# Efficient: Prefetch related objects
users = User.objects.prefetch_related('posts').all()
```

## Testing

The integration includes comprehensive tests:

```python
# test_models.py
from django.test import TestCase
from myapp.models import User

class UserModelTest(TestCase):
    def test_create_user(self):
        user = User.objects.create(
            first_name='John',
            last_name='Doe',
            email='john.doe@example.com'
        )
        self.assertEqual(user.first_name, 'John')
        self.assertTrue(user.is_active)
```

## Migration Support

The integration supports Django migrations:

```bash
# Create migrations
python manage.py makemigrations

# Apply migrations
python manage.py migrate
```

Migrations are translated to PhantomDB schema operations:
- `CREATE TABLE` becomes PhantomDB table creation
- `ALTER TABLE` becomes PhantomDB schema modifications
- `DROP TABLE` becomes PhantomDB table deletion

## Security Considerations

### SQL Injection Prevention

The integration uses parameterized queries to prevent SQL injection:

```python
# Safe: Parameters are properly escaped
User.objects.filter(email=user_input)

# Unsafe: Direct string concatenation (avoid)
# User.objects.extra(where=[f"email = '{user_input}'"])
```

### Authentication

Configure authentication in Django settings:

```python
DATABASES = {
    'default': {
        'ENGINE': 'phantomdb_django',
        'NAME': 'myapp',
        'HOST': 'https://phantomdb.example.com',
        'USER': os.environ.get('PHANTOMDB_USER'),
        'PASSWORD': os.environ.get('PHANTOMDB_PASSWORD'),
    }
}
```

## Limitations

### Current Limitations

1. **Raw SQL Support**: Limited support for complex raw SQL queries
2. **Aggregation Functions**: Some advanced aggregation functions may not be supported
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
   PhantomDBError: Failed to connect to PhantomDB server
   ```
   Solution: Check PhantomDB server status and connection settings

2. **Authentication Errors**
   ```
   PhantomDBError: Unauthorized access
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
# settings.py
LOGGING = {
    'version': 1,
    'disable_existing_loggers': False,
    'handlers': {
        'console': {
            'class': 'logging.StreamHandler',
        },
    },
    'loggers': {
        'phantomdb_django': {
            'handlers': ['console'],
            'level': 'DEBUG',
        },
    },
}
```

## API Reference

### DatabaseWrapper

```python
class DatabaseWrapper(BaseDatabaseWrapper):
    def __init__(self, settings_dict, alias='default'):
        """Initialize the database wrapper."""
        pass
    
    def get_connection_params(self):
        """Get connection parameters from settings."""
        pass
    
    def get_new_connection(self, conn_params):
        """Create a new PhantomDB connection."""
        pass
    
    def create_cursor(self, name=None):
        """Create a new cursor."""
        pass
```

### PhantomDBCursor

```python
class PhantomDBCursor(CursorWrapper):
    def __init__(self, connection, db):
        """Initialize the cursor."""
        pass
    
    def execute(self, sql, params=None):
        """Execute a SQL query."""
        pass
    
    def fetchone(self):
        """Fetch a single row."""
        pass
    
    def fetchall(self):
        """Fetch all rows."""
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
src/orm/django/
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