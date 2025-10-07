"""
PhantomDB Testing Framework

A comprehensive testing framework for PhantomDB applications that provides
utilities for testing database operations, transactions, and integration
with popular testing frameworks.
"""

import unittest
import asyncio
from typing import Any, Dict, List, Optional, Callable
from dataclasses import dataclass
from contextlib import contextmanager
import warnings
import functools
import time

# Import client libraries (handle missing dependencies gracefully)
try:
    from phantomdb import PhantomDB as PythonClient
except ImportError:
    PythonClient = None
    warnings.warn("phantomdb-python not installed. Some features will be disabled.")

try:
    import pytest
except ImportError:
    pytest = None
    warnings.warn("pytest not installed. Pytest integration will be disabled.")

# Import client libraries from other languages (handle missing dependencies gracefully)
try:
    from ..clients.javascript.phantomdb import PhantomDB as JSClient
except ImportError:
    JSClient = None

try:
    from ..clients.go.phantomdb import PhantomDB as GoClient
except ImportError:
    GoClient = None

try:
    from ..clients.rust.phantomdb import PhantomDB as RustClient
except ImportError:
    RustClient = None


@dataclass
class TestResult:
    """Represents the result of a test execution."""
    name: str
    passed: bool
    duration: float
    error: Optional[str] = None
    details: Optional[Dict[str, Any]] = None


class PhantomDBTestCase(unittest.TestCase):
    """
    Base test case class for PhantomDB testing.
    
    Provides utilities for setting up test databases, managing transactions,
    and asserting database states.
    """
    
    def __init__(self, database_url: str = "http://localhost:8080"):
        """Initialize the test case with a database URL."""
        super().__init__()
        self.database_url = database_url
        self.client = None
        self.test_database = None
        self.cleanup_functions = []
    
    def setUp(self):
        """Set up the test case."""
        if PythonClient:
            self.client = PythonClient(self.database_url)
        self.test_database = f"test_db_{int(time.time() * 1000)}"
        self._create_test_database()
    
    def tearDown(self):
        """Clean up after the test case."""
        self._drop_test_database()
        # Execute cleanup functions in reverse order
        for cleanup_func in reversed(self.cleanup_functions):
            try:
                cleanup_func()
            except Exception as e:
                warnings.warn(f"Cleanup function failed: {e}")
    
    def _create_test_database(self):
        """Create a test database."""
        if self.client:
            try:
                self.client.create_database(self.test_database)
                self.cleanup_functions.append(lambda: self.client.drop_database(self.test_database))
            except Exception as e:
                warnings.warn(f"Failed to create test database: {e}")
    
    def _drop_test_database(self):
        """Drop the test database."""
        if self.client and self.test_database:
            try:
                self.client.drop_database(self.test_database)
            except Exception as e:
                warnings.warn(f"Failed to drop test database: {e}")
    
    def create_table(self, table_name: str, columns: List[Dict[str, str]]):
        """Create a table in the test database."""
        if self.client:
            try:
                self.client.create_table(self.test_database, table_name, columns)
                self.cleanup_functions.append(
                    lambda: self.client.drop_table(self.test_database, table_name)
                )
            except Exception as e:
                raise Exception(f"Failed to create table {table_name}: {e}")
    
    def insert_data(self, table_name: str, data: Dict[str, Any]):
        """Insert data into a table."""
        if self.client:
            try:
                return self.client.insert(self.test_database, table_name, data)
            except Exception as e:
                raise Exception(f"Failed to insert data into {table_name}: {e}")
    
    def select_data(self, table_name: str, condition: str = ""):
        """Select data from a table."""
        if self.client:
            try:
                return self.client.select(self.test_database, table_name, condition)
            except Exception as e:
                raise Exception(f"Failed to select data from {table_name}: {e}")
    
    def assert_table_exists(self, table_name: str):
        """Assert that a table exists in the test database."""
        if self.client:
            tables = self.client.list_tables(self.test_database)
            if table_name not in tables:
                raise AssertionError(f"Table {table_name} does not exist")
    
    def assert_row_count(self, table_name: str, expected_count: int):
        """Assert that a table has the expected number of rows."""
        data = self.select_data(table_name)
        actual_count = len(data) if isinstance(data, list) else 0
        if actual_count != expected_count:
            raise AssertionError(
                f"Expected {expected_count} rows in {table_name}, but found {actual_count}"
            )
    
    def assert_row_exists(self, table_name: str, condition: str):
        """Assert that at least one row matches the condition."""
        data = self.select_data(table_name, condition)
        if not data or (isinstance(data, list) and len(data) == 0):
            raise AssertionError(
                f"No rows found in {table_name} matching condition: {condition}"
            )


class PhantomDBTestRunner:
    """Test runner for PhantomDB tests."""
    
    def __init__(self, database_url: str = "http://localhost:8080"):
        self.database_url = database_url
        self.results = []
    
    def run_test_case(self, test_case_class, method_name: str) -> TestResult:
        """Run a single test method."""
        start_time = time.time()
        
        try:
            # Create test instance
            test_instance = test_case_class(self.database_url)
            
            # Run setup
            if hasattr(test_instance, 'setUp'):
                test_instance.setUp()
            
            # Run test method
            test_method = getattr(test_instance, method_name)
            test_method()
            
            # Run teardown
            if hasattr(test_instance, 'tearDown'):
                test_instance.tearDown()
            
            duration = time.time() - start_time
            return TestResult(
                name=f"{test_case_class.__name__}.{method_name}",
                passed=True,
                duration=duration
            )
        
        except Exception as e:
            duration = time.time() - start_time
            return TestResult(
                name=f"{test_case_class.__name__}.{method_name}",
                passed=False,
                duration=duration,
                error=str(e)
            )
    
    def run_test_suite(self, test_case_class) -> List[TestResult]:
        """Run all test methods in a test case class."""
        results = []
        
        # Find all test methods
        test_methods = [
            method for method in dir(test_case_class)
            if method.startswith('test_') and callable(getattr(test_case_class, method))
        ]
        
        # Run each test method
        for method_name in test_methods:
            result = self.run_test_case(test_case_class, method_name)
            results.append(result)
            self.results.append(result)
        
        return results


def phantomdb_test(database_url: str = "http://localhost:8080"):
    """
    Decorator for PhantomDB test functions.
    
    Automatically sets up and tears down a test database for the test function.
    """
    def decorator(func: Callable) -> Callable:
        @functools.wraps(func)
        def wrapper(*args, **kwargs):
            # Create test database
            client = PythonClient(database_url) if PythonClient else None
            test_database = f"test_db_{int(time.time() * 1000)}"
            
            if client:
                try:
                    client.create_database(test_database)
                except Exception as e:
                    warnings.warn(f"Failed to create test database: {e}")
                    raise
            
            try:
                # Call the test function with the test database name
                return func(test_database, *args, **kwargs)
            finally:
                # Clean up test database
                if client:
                    try:
                        client.drop_database(test_database)
                    except Exception as e:
                        warnings.warn(f"Failed to drop test database: {e}")
        
        return wrapper
    return decorator


@contextmanager
def phantomdb_transaction(database_url: str = "http://localhost:8080"):
    """
    Context manager for PhantomDB transactions in tests.
    
    Automatically begins a transaction at the start and rolls it back at the end.
    """
    client = PythonClient(database_url) if PythonClient else None
    transaction_id = None
    
    try:
        if client:
            transaction_id = client.begin_transaction()
        yield transaction_id
    finally:
        if client and transaction_id:
            try:
                client.rollback_transaction(transaction_id)
            except Exception as e:
                warnings.warn(f"Failed to rollback transaction: {e}")


# Async version for async tests
class AsyncPhantomDBTestCase:
    """
    Base test case class for async PhantomDB testing.
    """
    
    def __init__(self, database_url: str = "http://localhost:8080"):
        self.database_url = database_url
        self.client = None
        self.test_database = None
        self.cleanup_functions = []
    
    async def asyncSetUp(self):
        """Async setup for the test case."""
        if PythonClient:
            self.client = PythonClient(self.database_url)
        self.test_database = f"test_db_{int(time.time() * 1000)}"
        await self._create_test_database()
    
    async def asyncTearDown(self):
        """Async cleanup after the test case."""
        await self._drop_test_database()
        # Execute cleanup functions in reverse order
        for cleanup_func in reversed(self.cleanup_functions):
            try:
                if asyncio.iscoroutinefunction(cleanup_func):
                    await cleanup_func()
                else:
                    cleanup_func()
            except Exception as e:
                warnings.warn(f"Cleanup function failed: {e}")
    
    async def _create_test_database(self):
        """Create a test database."""
        if self.client:
            try:
                await asyncio.get_event_loop().run_in_executor(
                    None, lambda: self.client.create_database(self.test_database)
                )
                self.cleanup_functions.append(
                    lambda: asyncio.get_event_loop().run_in_executor(
                        None, lambda: self.client.drop_database(self.test_database)
                    )
                )
            except Exception as e:
                warnings.warn(f"Failed to create test database: {e}")
    
    async def _drop_test_database(self):
        """Drop the test database."""
        if self.client and self.test_database:
            try:
                await asyncio.get_event_loop().run_in_executor(
                    None, lambda: self.client.drop_database(self.test_database)
                )
            except Exception as e:
                warnings.warn(f"Failed to drop test database: {e}")


def async_phantomdb_test(database_url: str = "http://localhost:8080"):
    """
    Decorator for async PhantomDB test functions.
    """
    def decorator(func: Callable) -> Callable:
        @functools.wraps(func)
        async def wrapper(*args, **kwargs):
            # Create test database
            client = PythonClient(database_url) if PythonClient else None
            test_database = f"test_db_{int(time.time() * 1000)}"
            
            if client:
                try:
                    await asyncio.get_event_loop().run_in_executor(
                        None, lambda: client.create_database(test_database)
                    )
                except Exception as e:
                    warnings.warn(f"Failed to create test database: {e}")
                    raise
            
            try:
                # Call the test function with the test database name
                return await func(test_database, *args, **kwargs)
            finally:
                # Clean up test database
                if client:
                    try:
                        await asyncio.get_event_loop().run_in_executor(
                            None, lambda: client.drop_database(test_database)
                        )
                    except Exception as e:
                        warnings.warn(f"Failed to drop test database: {e}")
        
        return wrapper
    return decorator