"""
Example usage of PhantomDB SQLAlchemy dialect.
"""

def main():
    """Demonstrate PhantomDB SQLAlchemy integration features."""
    print("PhantomDB SQLAlchemy Dialect Example")
    print("=" * 40)
    print("\nThis example shows how the integration would work with SQLAlchemy.")
    
    print("\nConfiguration:")
    print("""
from sqlalchemy import create_engine

# Create engine with PhantomDB
engine = create_engine('phantomdb://localhost:8080/myapp')
""")
    
    print("\nModel definition:")
    print("""
from sqlalchemy import Column, Integer, String
from sqlalchemy.ext.declarative import declarative_base

Base = declarative_base()

class User(Base):
    __tablename__ = 'users'
    
    id = Column(Integer, primary_key=True)
    first_name = Column(String(100))
    last_name = Column(String(100))
    email = Column(String(255), unique=True)
""")
    
    print("\nUsage examples:")
    print("""
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

# Transactions
try:
    user = User(first_name='Bob', last_name='Johnson')
    session.add(user)
    profile = Profile(user_id=user.id, bio='Designer')
    session.add(profile)
    session.commit()
except Exception as e:
    session.rollback()
    raise
""")
    
    print("\nExample completed successfully!")
    print("\nNote: This is a conceptual example. To run it, you would need to:")
    print("1. Install SQLAlchemy and phantomdb-client")
    print("2. Configure the PhantomDB SQLAlchemy dialect")
    print("3. Run the example with a PhantomDB server running")

if __name__ == '__main__':
    main()