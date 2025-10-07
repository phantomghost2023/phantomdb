import unittest
import json
import sys
import os
from unittest.mock import patch, MagicMock

# Add the current directory to the path
sys.path.insert(0, os.path.dirname(__file__))

# Import the module directly
import phantomdb_debugger


class TestQueryDebugger(unittest.TestCase):
    
    def setUp(self):
        self.debugger = phantomdb_debugger.QueryDebugger("http://localhost:8080")
    
    def test_init(self):
        self.assertEqual(self.debugger.base_url, "http://localhost:8080")
        self.assertEqual(self.debugger.query_history, [])
    
    @patch('phantomdb_debugger.requests.Session.request')
    def test_make_request_success(self, mock_request):
        # Mock successful response
        mock_response = MagicMock()
        mock_response.raise_for_status.return_value = None
        mock_response.content = b'{"result": "success"}'
        mock_response.json.return_value = {"result": "success"}
        mock_request.return_value = mock_response
        
        result = self.debugger._make_request("GET", "/test")
        self.assertIn("result", result)
        self.assertIn("execution_time_ms", result)
    
    @patch('phantomdb_debugger.requests.Session.request')
    def test_make_request_error(self, mock_request):
        # Mock request exception
        mock_request.side_effect = Exception("Connection error")
        
        result = self.debugger._make_request("GET", "/test")
        self.assertIn("error", result)
    
    @patch('phantomdb_debugger.requests.Session.request')
    def test_execute_query(self, mock_request):
        # Mock successful response
        mock_response = MagicMock()
        mock_response.raise_for_status.return_value = None
        mock_response.content = b'{"result": "query executed"}'
        mock_response.json.return_value = {"result": "query executed"}
        mock_request.return_value = mock_response
        
        result = self.debugger.execute_query("testdb", "SELECT * FROM users")
        self.assertIn("result", result)
    
    @patch('phantomdb_debugger.requests.Session.request')
    def test_profile_query(self, mock_request):
        # Mock successful response
        mock_response = MagicMock()
        mock_response.raise_for_status.return_value = None
        mock_response.content = b'{"result": "query executed"}'
        mock_response.json.return_value = {"result": "query executed"}
        mock_request.return_value = mock_response
        
        result = self.debugger.profile_query("testdb", "SELECT * FROM users", 3)
        self.assertIn("query", result)
        self.assertIn("iterations", result)
        self.assertEqual(result["iterations"], 3)
    
    def test_clear_history(self):
        # Add some history
        self.debugger.query_history.append({"query": "SELECT * FROM users"})
        self.assertEqual(len(self.debugger.query_history), 1)
        
        # Clear history
        self.debugger.clear_history()
        self.assertEqual(len(self.debugger.query_history), 0)


if __name__ == '__main__':
    unittest.main()