# PhantomDB SQLAlchemy Dialect

This package provides a SQLAlchemy dialect that allows SQLAlchemy applications to use PhantomDB as their database backend through its REST API.

## Overview

The PhantomDB SQLAlchemy dialect enables SQLAlchemy developers to leverage PhantomDB's advanced features while maintaining the familiar SQLAlchemy interface. This dialect translates SQLAlchemy operations into PhantomDB REST API calls.

## Features

- Full SQLAlchemy compatibility
- Automatic schema translation
- Query translation from SQLAlchemy to PhantomDB
- Transaction support
- Connection pooling (planned)

## Installation

```bash
pip install phantomdb-sqlalchemy
```

## Configuration

To use PhantomDB with SQLAlchemy, configure your engine:

```python
from sqlalchemy import create_engine

# Create engine with PhantomDB
engine = create_engine('phantomdb://localhost:8080/myapp')

# Or with explicit URL
engine = create_engine('phantomdb+rest://localhost:8080/myapp')
```

## Usage

Once configured, you can use SQLAlchemy as usual:

```python
from sqlalchemy import Column, Integer, String, create_engine
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker

Base = declarative_base()

class User(Base):
    __tablename__ = 'users'
    
    id = Column(Integer, primary_key=True)
    first_name = Column(String(100))
    last_name = Column(String(100))
    email = Column(String(255), unique=True)

# Create engine and session
engine = create_engine('phantomdb://localhost:8080/myapp')
Session = sessionmaker(bind=engine)
session = Session()

# Create tables
Base.metadata.create_all(engine)

# Create a user
user = User(first_name='John', last_name='Doe', email='john.doe@example.com')
session.add(user)
session.commit()

# Query users
users = session.query(User).filter(User.email.like('%@example.com')).all()

# Update users
session.query(User).filter(User.id == 1).update({User.first_name: 'Jane'})

# Delete users
session.query(User).filter(User.id == 1).delete()
session.commit()
```

## Data Type Mapping

The dialect automatically maps SQLAlchemy field types to PhantomDB types:

| SQLAlchemy Type | PhantomDB Type |
|----------------|----------------|
| Integer | INTEGER |
| BigInteger | BIGINT |
| Boolean | BOOLEAN |
| CHAR(n) | CHAR(n) |
| VARCHAR(n) | VARCHAR(n) |
| Text | TEXT |
| Float | FLOAT |
| DECIMAL(p, s) | DECIMAL(p, s) |
| DateTime | TIMESTAMP |
| Date | DATE |
| Time | TIME |
| JSON | JSON |

## Advanced Features

### Transactions

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

### Raw SQL

```python
from sqlalchemy import text

# Execute raw SQL
result = engine.execute(text("SELECT * FROM users WHERE age > :age"), age=25)
rows = result.fetchall()
```

## Requirements

- SQLAlchemy >= 1.4.0
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