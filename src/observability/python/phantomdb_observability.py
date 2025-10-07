"""
PhantomDB Observability Client

A Python client for interacting with PhantomDB's observability and monitoring features.
"""

import requests
import json
from typing import Dict, Any, Optional
from datetime import datetime


class PhantomDBObservabilityClient:
    """Client for PhantomDB observability and monitoring features."""
    
    def __init__(self, base_url: str = "http://localhost:8080"):
        """Initialize the observability client."""
        self.base_url = base_url.rstrip('/')
        self.session = requests.Session()
    
    def _make_request(self, method: str, endpoint: str, 
                     data: Optional[Dict[str, Any]] = None) -> Dict[str, Any]:
        """Make an HTTP request to the PhantomDB REST API."""
        url = f"{self.base_url}{endpoint}"
        
        try:
            if data is not None:
                response = self.session.request(
                    method, url, 
                    json=data,
                    headers={'Content-Type': 'application/json'}
                )
            else:
                response = self.session.request(method, url)
                
            response.raise_for_status()
            if response.headers.get('Content-Type', '').startswith('application/json'):
                return response.json() if response.content else {}
            else:
                return {"content": response.text}
            
        except requests.exceptions.RequestException as e:
            print(f"Error: {e}")
            return {"error": str(e)}
        except json.JSONDecodeError as e:
            print(f"Error decoding JSON response: {e}")
            return {"error": str(e)}
    
    def get_health(self) -> Dict[str, Any]:
        """Get the health status of the PhantomDB server."""
        return self._make_request("GET", "/health")
    
    def get_stats(self) -> Dict[str, Any]:
        """Get database statistics."""
        return self._make_request("GET", "/stats")
    
    def get_metrics(self) -> str:
        """Get Prometheus metrics."""
        result = self._make_request("GET", "/metrics")
        if "content" in result:
            return result["content"]
        elif "error" in result:
            return f"# Error: {result['error']}"
        else:
            return "# No metrics available"
    
    def get_metrics_as_dict(self) -> Dict[str, Any]:
        """Get metrics parsed as a dictionary."""
        metrics_text = self.get_metrics()
        metrics = {}
        
        for line in metrics_text.split('\n'):
            if line.startswith('#') or not line.strip():
                continue
                
            if ' ' in line:
                key, value = line.split(' ', 1)
                try:
                    metrics[key] = float(value)
                except ValueError:
                    metrics[key] = value
                    
        return metrics
    
    def record_query(self, query_type: str, duration_ms: float) -> None:
        """Record a query execution (would be implemented server-side)."""
        print(f"Recording query: {query_type} took {duration_ms}ms")
        # In a real implementation, this would send data to the server


def parse_prometheus_metrics(metrics_text: str) -> Dict[str, Any]:
    """Parse Prometheus metrics text into a dictionary."""
    metrics = {}
    
    for line in metrics_text.split('\n'):
        # Skip comments and empty lines
        if line.startswith('#') or not line.strip():
            continue
            
        # Parse metric lines
        if ' ' in line:
            key, value = line.split(' ', 1)
            try:
                metrics[key] = float(value)
            except ValueError:
                metrics[key] = value
                
    return metrics


def main():
    """Example usage of the PhantomDB Observability Client."""
    print("PhantomDB Observability Client")
    print("=" * 40)
    
    # Create client
    client = PhantomDBObservabilityClient("http://localhost:8080")
    
    # Get health status
    print("\n1. Health Status:")
    health = client.get_health()
    print(json.dumps(health, indent=2))
    
    # Get stats
    print("\n2. Database Statistics:")
    stats = client.get_stats()
    print(json.dumps(stats, indent=2))
    
    # Get metrics
    print("\n3. Prometheus Metrics:")
    metrics = client.get_metrics()
    print(metrics[:500] + "..." if len(metrics) > 500 else metrics)
    
    # Parse metrics
    print("\n4. Parsed Metrics:")
    parsed_metrics = client.get_metrics_as_dict()
    print(f"Found {len(parsed_metrics)} metrics")
    for key, value in list(parsed_metrics.items())[:5]:
        print(f"  {key}: {value}")


if __name__ == "__main__":
    main()