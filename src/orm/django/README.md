# PhantomDB Django ORM Integration

This package provides a Django database backend that allows Django applications to use PhantomDB as their database through its REST API.

## Overview

The PhantomDB Django integration enables Django developers to leverage PhantomDB's advanced features while maintaining the familiar Django ORM interface. This backend translates Django ORM operations into PhantomDB REST API calls.

## Features

- Full Django ORM compatibility
- Automatic schema translation
- Query translation from Django ORM to PhantomDB
- Transaction support
- Migration support
- Connection pooling (planned)

## Installation

```bash
pip install phantomdb-django
```

## Configuration

To use PhantomDB with Django, configure your `settings.py`:

```python
DATABASES = {
    'default': {
        'ENGINE': 'phantomdb_django',
        'NAME': 'myapp',
        'HOST': 'http://localhost:8080',  # PhantomDB server URL
    }
}
```

## Usage

Once configured, you can use Django models as usual:

```python
from django.db import models

class User(models.Model):
    first_name = models.CharField(max_length=100)
    last_name = models.CharField(max_length=100)
    email = models.EmailField(unique=True)
    created_at = models.DateTimeField(auto_now_add=True)
    
    class Meta:
        db_table = 'users'

# Create a user
user = User.objects.create(
    first_name='John',
    last_name='Doe',
    email='john.doe@example.com'
)

# Query users
active_users = User.objects.filter(is_active=True)

# Update users
User.objects.filter(email__endswith='@example.com').update(is_active=True)

# Delete users
User.objects.filter(created_at__lt=timezone.now() - timedelta(days=30)).delete()
```

## Data Type Mapping

The integration automatically maps Django field types to PhantomDB types:

| Django Field | PhantomDB Type |
|--------------|----------------|
| AutoField | INTEGER PRIMARY KEY |
| CharField | VARCHAR(n) |
| TextField | TEXT |
| IntegerField | INTEGER |
| BigIntegerField | BIGINT |
| BooleanField | BOOLEAN |
| DateTimeField | TIMESTAMP |
| DateField | DATE |
| FloatField | DOUBLE |
| DecimalField | DECIMAL(p, s) |
| JSONField | JSON |

## Advanced Features

### Transactions

```python
from django.db import transaction

with transaction.atomic():
    user = User.objects.create(first_name='John', last_name='Doe')
    profile = Profile.objects.create(user=user, bio='Software Developer')
```

### Raw Queries

```python
from django.db import connection

with connection.cursor() as cursor:
    cursor.execute("SELECT * FROM users WHERE age > %s", [25])
    rows = cursor.fetchall()
```

## Requirements

- Django >= 3.2
- phantomdb-client >= 1.0.0
- Python >= 3.7

## Development

To run tests:

```bash
python -m pytest tests/
```

## License

MIT License

## Contributing

Contributions are welcome! Please read our contributing guidelines before submitting pull requests.