#!/usr/bin/env python3
"""
Unit tests for PhantomDB import/export functionality
"""

import unittest
import sys
import os
from io import StringIO
from unittest.mock import patch, MagicMock
import requests

# Add the src/tools directory to the path so we can import the module
sys.path.insert(0, os.path.join(os.path.dirname(__file__)))

import phantomdb_cli


class TestPhantomDBImportExport(unittest.TestCase):
    """Test cases for PhantomDB import/export functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.cli = phantomdb_cli.PhantomDBCLI("http://localhost:8080")
    
    @patch('phantomdb_cli.requests.Session')
    def test_import_data_success(self, mock_session):
        """Test successful data import."""
        # Mock the response
        mock_response = MagicMock()
        mock_response.content = b'{"rows_imported": 10, "rows_failed": 0}'
        mock_response.json.return_value = {"rows_imported": 10, "rows_failed": 0}
        mock_response.raise_for_status.return_value = None
        
        # Mock the session request
        mock_session_instance = mock_session.return_value
        mock_session_instance.request.return_value = mock_response
        
        # Capture stdout
        captured_output = StringIO()
        sys.stdout = captured_output
        
        # Test the import
        self.cli.import_data("testdb", "users", "/path/to/data.csv", "csv")
        
        # Restore stdout
        sys.stdout = sys.__stdout__
        
        # Check that import was successful
        output = captured_output.getvalue()
        self.assertIn("Data import completed. Rows imported: 10, Rows failed: 0", output)
    
    @patch('phantomdb_cli.requests.Session')
    def test_import_data_error(self, mock_session):
        """Test data import with error."""
        # Mock the response to raise an exception
        mock_session_instance = mock_session.return_value
        mock_session_instance.request.side_effect = requests.exceptions.RequestException("Connection error")
        
        # Capture stdout
        captured_output = StringIO()
        sys.stdout = captured_output
        
        # Test the import
        self.cli.import_data("testdb", "users", "/path/to/data.csv", "csv")
        
        # Restore stdout
        sys.stdout = sys.__stdout__
        
        # Check that error was printed
        output = captured_output.getvalue()
        self.assertIn("Failed to import data: Connection error", output)
    
    @patch('phantomdb_cli.requests.Session')
    def test_export_data_success(self, mock_session):
        """Test successful data export."""
        # Mock the response
        mock_response = MagicMock()
        mock_response.content = b'{"rows_exported": 10}'
        mock_response.json.return_value = {"rows_exported": 10}
        mock_response.raise_for_status.return_value = None
        
        # Mock the session request
        mock_session_instance = mock_session.return_value
        mock_session_instance.request.return_value = mock_response
        
        # Capture stdout
        captured_output = StringIO()
        sys.stdout = captured_output
        
        # Test the export
        self.cli.export_data("testdb", "users", "/path/to/data.json", "json")
        
        # Restore stdout
        sys.stdout = sys.__stdout__
        
        # Check that export was successful
        output = captured_output.getvalue()
        self.assertIn("Data export completed. Rows exported: 10", output)
    
    @patch('phantomdb_cli.requests.Session')
    def test_export_data_error(self, mock_session):
        """Test data export with error."""
        # Mock the response to raise an exception
        mock_session_instance = mock_session.return_value
        mock_session_instance.request.side_effect = requests.exceptions.RequestException("Connection error")
        
        # Capture stdout
        captured_output = StringIO()
        sys.stdout = captured_output
        
        # Test the export
        self.cli.export_data("testdb", "users", "/path/to/data.json", "json")
        
        # Restore stdout
        sys.stdout = sys.__stdout__
        
        # Check that error was printed
        output = captured_output.getvalue()
        self.assertIn("Failed to export data: Connection error", output)


if __name__ == '__main__':
    unittest.main()