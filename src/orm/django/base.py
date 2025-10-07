"""
PhantomDB backend for Django ORM.

This module provides a Django database backend that connects to PhantomDB
through its REST API, allowing Django applications to use PhantomDB as
their database backend.
"""

import logging
from typing import Any, Dict, List, Optional, Tuple
from django.db.backends.base.base import BaseDatabaseWrapper
from django.db.backends.base.client import BaseDatabaseClient
from django.db.backends.base.creation import BaseDatabaseCreation
from django.db.backends.base.introspection import BaseDatabaseIntrospection
from django.db.backends.base.operations import BaseDatabaseOperations
from django.db.backends.base.schema import BaseDatabaseSchemaEditor
from django.db.backends.base.validation import BaseDatabaseValidation
from django.db.backends.utils import CursorWrapper

# Import our PhantomDB client
from phantomdb import PhantomDB, PhantomDBError

logger = logging.getLogger(__name__)


class DatabaseWrapper(BaseDatabaseWrapper):
    """
    Django database backend for PhantomDB.
    
    This class implements the Django database backend interface to connect
    to PhantomDB through its REST API.
    """
    
    vendor = 'phantomdb'
    display_name = 'PhantomDB'
    
    # Data types mapping from Django to PhantomDB
    data_types = {
        'AutoField': 'INTEGER',
        'BigAutoField': 'BIGINT',
        'BinaryField': 'BLOB',
        'BooleanField': 'BOOLEAN',
        'CharField': 'VARCHAR(%(max_length)s)',
        'DateField': 'DATE',
        'DateTimeField': 'TIMESTAMP',
        'DecimalField': 'DECIMAL(%(max_digits)s, %(decimal_places)s)',
        'DurationField': 'BIGINT',
        'FileField': 'VARCHAR(%(max_length)s)',
        'FilePathField': 'VARCHAR(%(max_length)s)',
        'FloatField': 'DOUBLE',
        'IntegerField': 'INTEGER',
        'BigIntegerField': 'BIGINT',
        'IPAddressField': 'VARCHAR(15)',
        'GenericIPAddressField': 'VARCHAR(39)',
        'JSONField': 'JSON',
        'NullBooleanField': 'BOOLEAN',
        'OneToOneField': 'INTEGER',
        'PositiveBigIntegerField': 'BIGINT',
        'PositiveIntegerField': 'INTEGER',
        'PositiveSmallIntegerField': 'SMALLINT',
        'SlugField': 'VARCHAR(%(max_length)s)',
        'SmallAutoField': 'SMALLINT',
        'SmallIntegerField': 'SMALLINT',
        'TextField': 'TEXT',
        'TimeField': 'TIME',
        'UUIDField': 'VARCHAR(32)',
    }
    
    data_types_suffix = {
        'AutoField': 'PRIMARY KEY',
        'BigAutoField': 'PRIMARY KEY',
        'SmallAutoField': 'PRIMARY KEY',
    }
    
    operators = {
        'exact': '= %s',
        'iexact': "ILIKE %s",
        'contains': "LIKE %s",
        'icontains': "ILIKE %s",
        'regex': "~ %s",
        'iregex': "~* %s",
        'gt': '> %s',
        'gte': '>= %s',
        'lt': '< %s',
        'lte': '<= %s',
        'startswith': "LIKE %s",
        'endswith': "LIKE %s",
        'istartswith': "ILIKE %s",
        'iendswith': "ILIKE %s",
    }
    
    pattern_esc = r"REPLACE(REPLACE(REPLACE({}, '\', '\\'), '%%', '\%%'), '_', '\_')"
    pattern_ops = {
        'contains': "LIKE '%%' || {} || '%%'",
        'icontains': "ILIKE '%%' || {} || '%%'",
        'startswith': "LIKE {} || '%%'",
        'istartswith': "ILIKE {} || '%%'",
        'endswith': "LIKE '%%' || {}",
        'iendswith': "ILIKE '%%' || {}",
    }

    Database = PhantomDB
    client_class = BaseDatabaseClient
    creation_class = BaseDatabaseCreation
    introspection_class = BaseDatabaseIntrospection
    ops_class = BaseDatabaseOperations
    validation_class = BaseDatabaseValidation

    def __init__(self, settings_dict: Dict[str, Any], alias: str = 'default') -> None:
        """
        Initialize the PhantomDB database backend.
        
        Args:
            settings_dict: Django database settings dictionary
            alias: Database alias name
        """
        super().__init__(settings_dict, alias)
        self.connection = None
        self.queries_log: List[Dict] = []
        
    def get_connection_params(self) -> Dict[str, Any]:
        """
        Get connection parameters from Django settings.
        
        Returns:
            Dictionary of connection parameters
        """
        settings_dict = self.settings_dict
        return {
            'base_url': settings_dict.get('HOST', 'http://localhost:8080'),
        }
    
    def get_new_connection(self, conn_params: Dict[str, Any]) -> PhantomDB:
        """
        Create a new connection to PhantomDB.
        
        Args:
            conn_params: Connection parameters
            
        Returns:
            PhantomDB client instance
        """
        return PhantomDB(base_url=conn_params['base_url'])
    
    def init_connection_state(self) -> None:
        """
        Initialize the connection state after establishing a new connection.
        """
        pass  # No specific initialization needed for PhantomDB
    
    def create_cursor(self, name=None):
        """
        Create a cursor object for executing queries.
        
        Args:
            name: Cursor name (optional)
            
        Returns:
            PhantomDBCursor instance
        """
        return PhantomDBCursor(self.connection, self)
    
    def _set_autocommit(self, autocommit: bool) -> None:
        """
        Set autocommit mode.
        
        Args:
            autocommit: Whether to enable autocommit
        """
        # PhantomDB handles transactions explicitly, so we don't need to do anything here
        pass
    
    def is_usable(self) -> bool:
        """
        Check if the database connection is usable.
        
        Returns:
            True if the connection is usable, False otherwise
        """
        if self.connection is None:
            return False
        try:
            self.connection.health_check()
            return True
        except PhantomDBError:
            return False


class PhantomDBCursor(CursorWrapper):
    """
    Cursor implementation for PhantomDB.
    
    This class implements the Django cursor interface for executing
    queries against PhantomDB through its REST API.
    """
    
    def __init__(self, connection: PhantomDB, db: DatabaseWrapper) -> None:
        """
        Initialize the PhantomDB cursor.
        
        Args:
            connection: PhantomDB client connection
            db: Database wrapper instance
        """
        self.connection = connection
        self.db = db
        self.closed = False
        self.description = None
        self.rowcount = -1
        self.results = []
        self.rownumber = 0
        
    def execute(self, sql: str, params: Optional[Tuple] = None) -> 'PhantomDBCursor':
        """
        Execute a SQL query.
        
        Args:
            sql: SQL query to execute
            params: Query parameters
            
        Returns:
            Self for method chaining
        """
        if self.closed:
            raise PhantomDBError("Cursor is closed")
            
        # Format the SQL with parameters if provided
        if params:
            # Simple parameter substitution for demonstration
            # In a real implementation, this would be more sophisticated
            formatted_sql = sql
            for param in params:
                if isinstance(param, str):
                    formatted_sql = formatted_sql.replace('?', f"'{param}'", 1)
                else:
                    formatted_sql = formatted_sql.replace('?', str(param), 1)
            sql = formatted_sql
            
        logger.debug(f"Executing SQL: {sql}")
        
        try:
            # Parse the SQL to determine the operation
            sql_upper = sql.upper().strip()
            
            if sql_upper.startswith('SELECT'):
                # Handle SELECT queries
                result = self._execute_select(sql)
                self.results = result
                self.rowcount = len(result)
                self.rownumber = 0
                
            elif sql_upper.startswith('INSERT'):
                # Handle INSERT queries
                result = self.connection.execute_query('default', sql)
                self.results = [result]
                self.rowcount = 1
                
            elif sql_upper.startswith('UPDATE'):
                # Handle UPDATE queries
                result = self.connection.execute_query('default', sql)
                self.results = [result]
                self.rowcount = result.get('affected_rows', 0)
                
            elif sql_upper.startswith('DELETE'):
                # Handle DELETE queries
                result = self.connection.execute_query('default', sql)
                self.results = [result]
                self.rowcount = result.get('affected_rows', 0)
                
            else:
                # Handle other queries
                result = self.connection.execute_query('default', sql)
                self.results = [result] if result else []
                self.rowcount = 1 if result else 0
                
            # Log the query for debugging
            self.db.queries_log.append({
                'sql': sql,
                'params': params,
                'duration': 0,  # Would be measured in a real implementation
            })
            
            return self
            
        except Exception as e:
            logger.error(f"Error executing SQL: {sql}, Error: {str(e)}")
            raise PhantomDBError(f"Error executing query: {str(e)}")
    
    def executemany(self, sql: str, param_list: List[Tuple]) -> 'PhantomDBCursor':
        """
        Execute a SQL query multiple times with different parameters.
        
        Args:
            sql: SQL query to execute
            param_list: List of parameter tuples
            
        Returns:
            Self for method chaining
        """
        for params in param_list:
            self.execute(sql, params)
        return self
    
    def fetchone(self):
        """
        Fetch the next row of a query result set.
        
        Returns:
            A single row from the result set, or None if no more rows
        """
        if self.rownumber >= len(self.results):
            return None
            
        row = self.results[self.rownumber]
        self.rownumber += 1
        return row
    
    def fetchmany(self, size: int = None):
        """
        Fetch the next set of rows of a query result set.
        
        Args:
            size: Number of rows to fetch
            
        Returns:
            A list of rows from the result set
        """
        if size is None:
            size = len(self.results) - self.rownumber
            
        end = min(self.rownumber + size, len(self.results))
        rows = self.results[self.rownumber:end]
        self.rownumber = end
        return rows
    
    def fetchall(self):
        """
        Fetch all (remaining) rows of a query result set.
        
        Returns:
            A list of all remaining rows from the result set
        """
        rows = self.results[self.rownumber:]
        self.rownumber = len(self.results)
        return rows
    
    def close(self) -> None:
        """
        Close the cursor.
        """
        self.closed = True
        self.results = []
        self.description = None
        self.rowcount = -1
        self.rownumber = 0
    
    def _execute_select(self, sql: str) -> List[Dict]:
        """
        Execute a SELECT query and return results.
        
        Args:
            sql: SELECT SQL query
            
        Returns:
            List of result records
        """
        # In a real implementation, this would parse the SQL and
        # translate it to PhantomDB API calls
        # For now, we'll just execute it directly
        result = self.connection.execute_query('default', sql)
        return result.get('records', [])
    
    def __enter__(self):
        return self
    
    def __exit__(self, exc_type, exc_value, traceback):
        self.close()