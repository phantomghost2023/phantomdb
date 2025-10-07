"""
Example usage of PhantomDB Django ORM integration.
"""

# This is a simplified example that demonstrates the concept
# In a real Django project, you would configure the database backend in settings.py

def main():
    """Demonstrate PhantomDB Django integration features."""
    print("PhantomDB Django ORM Integration Example")
    print("=" * 40)
    print("\nThis example shows how the integration would work in a Django project.")
    print("\nConfiguration in settings.py:")
    print("""
DATABASES = {
    'default': {
        'ENGINE': 'phantomdb_django',
        'NAME': 'myapp',
        'HOST': 'http://localhost:8080',  # PhantomDB server URL
    }
}
""")
    
    print("\nModel definition:")
    print("""
class User(models.Model):
    first_name = models.CharField(max_length=100)
    last_name = models.CharField(max_length=100)
    email = models.EmailField(unique=True)
    age = models.IntegerField(default=0)
    is_active = models.BooleanField(default=True)
    created_at = models.DateTimeField(auto_now_add=True)
    
    class Meta:
        db_table = 'users'
""")
    
    print("\nUsage examples:")
    print("""
# Create a user
user = User.objects.create(
    first_name='John',
    last_name='Doe',
    email='john.doe@example.com',
    age=30
)

# Query users
all_users = User.objects.all()
active_users = User.objects.filter(is_active=True)
young_users = User.objects.filter(age__lt=30)

# Update users
User.objects.filter(age__lt=30).update(is_active=False)

# Delete users
User.objects.filter(email__icontains='jane').delete()

# Transactions
with transaction.atomic():
    user = User.objects.create(first_name='Bob', last_name='Johnson')
    profile = Profile.objects.create(user=user, bio='Designer')

# Raw queries
with connection.cursor() as cursor:
    cursor.execute("SELECT * FROM users WHERE age > %s", [25])
    rows = cursor.fetchall()
""")
    
    print("\nExample completed successfully!")
    print("\nNote: This is a conceptual example. To run it, you would need to:")
    print("1. Install Django and phantomdb-client")
    print("2. Configure the PhantomDB Django backend in your Django settings")
    print("3. Run the example in a proper Django project context")

if __name__ == '__main__':
    main()
