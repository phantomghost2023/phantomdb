"""
PhantomDB Connection Pool for Python

This module provides connection pooling functionality for PhantomDB clients.
"""

import threading
import time
from queue import Queue, Empty
from typing import Optional, Dict, Any
from .phantomdb import PhantomDB, PhantomDBError

class PhantomDBConnectionPool:
    """
    A connection pool for PhantomDB clients.
    
    This class manages a pool of PhantomDB connections to improve performance
    by reusing connections instead of creating new ones for each operation.
    """
    
    def __init__(self, base_url: str = "http://localhost:8080", 
                 min_connections: int = 2, max_connections: int = 10,
                 acquire_timeout: int = 30, idle_timeout: int = 60):
        """
        Initialize the connection pool.
        
        Args:
            base_url: The base URL of the PhantomDB REST API server
            min_connections: Minimum number of connections in the pool
            max_connections: Maximum number of connections in the pool
            acquire_timeout: Timeout for acquiring a connection (seconds)
            idle_timeout: Timeout for idle connections (seconds)
        """
        self.base_url = base_url
        self.min_connections = min_connections
        self.max_connections = max_connections
        self.acquire_timeout = acquire_timeout
        self.idle_timeout = idle_timeout
        
        # Pool state
        self._pool = Queue()
        self._used = set()
        self._lock = threading.Lock()
        self._closed = False
        
        # Initialize the pool
        self._initialize_pool()
    
    def _initialize_pool(self) -> None:
        """Initialize the connection pool with minimum connections."""
        for _ in range(self.min_connections):
            self._create_connection()
    
    def _create_connection(self) -> Optional[PhantomDB]:
        """
        Create a new connection and add it to the pool.
        
        Returns:
            PhantomDB client instance or None if at max capacity
        """
        with self._lock:
            if self._pool.qsize() + len(self._used) >= self.max_connections:
                return None
            
            connection = PhantomDB(self.base_url)
            self._pool.put({
                'client': connection,
                'last_used': time.time(),
                'idle': True
            })
            
            return connection
    
    def acquire(self) -> PhantomDB:
        """
        Acquire a connection from the pool.
        
        Returns:
            PhantomDB client instance
            
        Raises:
            PhantomDBError: If unable to acquire a connection within the timeout
        """
        if self._closed:
            raise PhantomDBError("Connection pool is closed")
        
        start_time = time.time()
        
        while time.time() - start_time < self.acquire_timeout:
            try:
                # Try to get an available connection
                connection_info = self._pool.get_nowait()
                
                # Check if connection is still valid
                if time.time() - connection_info['last_used'] < self.idle_timeout:
                    connection_info['idle'] = False
                    with self._lock:
                        self._used.add(id(connection_info['client']))
                    return connection_info['client']
                # Connection is too old, discard it and try again
            except Empty:
                # No available connections, check if we can create a new one
                with self._lock:
                    if len(self._used) < self.max_connections:
                        connection = self._create_connection()
                        if connection:
                            connection_info = {
                                'client': connection,
                                'last_used': time.time(),
                                'idle': False
                            }
                            with self._lock:
                                self._used.add(id(connection_info['client']))
                            return connection
                
                # Wait a bit before trying again
                time.sleep(0.1)
        
        raise PhantomDBError("Timeout acquiring connection from pool")
    
    def release(self, connection: PhantomDB) -> None:
        """
        Release a connection back to the pool.
        
        Args:
            connection: The PhantomDB client instance to release
        """
        if self._closed:
            return
        
        with self._lock:
            connection_id = id(connection)
            if connection_id in self._used:
                self._used.remove(connection_id)
                
                # Add back to pool
                self._pool.put({
                    'client': connection,
                    'last_used': time.time(),
                    'idle': True
                })
    
    def close(self) -> None:
        """Close the connection pool and all connections."""
        self._closed = True
        
        # Clear the pool
        while not self._pool.empty():
            try:
                self._pool.get_nowait()
            except Empty:
                break
        
        with self._lock:
            self._used.clear()
    
    def get_status(self) -> Dict[str, Any]:
        """
        Get the current pool status.
        
        Returns:
            Dictionary with pool status information
        """
        with self._lock:
            return {
                'available': self._pool.qsize(),
                'used': len(self._used),
                'min': self.min_connections,
                'max': self.max_connections
            }

def create_pool(base_url: str = "http://localhost:8080", 
                min_connections: int = 2, max_connections: int = 10,
                acquire_timeout: int = 30, idle_timeout: int = 60) -> PhantomDBConnectionPool:
    """
    Create a new PhantomDB connection pool.
    
    Args:
        base_url: The base URL of the PhantomDB REST API server
        min_connections: Minimum number of connections in the pool
        max_connections: Maximum number of connections in the pool
        acquire_timeout: Timeout for acquiring a connection (seconds)
        idle_timeout: Timeout for idle connections (seconds)
        
    Returns:
        A new connection pool instance
    """
    return PhantomDBConnectionPool(base_url, min_connections, max_connections, 
                                 acquire_timeout, idle_timeout)