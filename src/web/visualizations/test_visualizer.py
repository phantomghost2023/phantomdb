#!/usr/bin/env python3
"""
Test script for PhantomDB Data Visualization Tools
"""

import sys
import os

# Add the current directory to the path
sys.path.insert(0, os.path.dirname(__file__))

from visualizer import DataVisualizer
import json


def test_visualizer():
    """Test the data visualizer functionality."""
    print("Testing PhantomDB Data Visualization Tools...")
    
    # Create a visualizer instance
    visualizer = DataVisualizer()
    
    # Test data
    sample_data = [
        {"name": "Alice", "age": 25, "salary": 50000},
        {"name": "Bob", "age": 30, "salary": 60000},
        {"name": "Charlie", "age": 35, "salary": 70000},
        {"name": "Diana", "age": 28, "salary": 55000},
        {"name": "Eve", "age": 32, "salary": 65000}
    ]
    
    # Test chart data creation
    print("\n1. Testing chart data creation...")
    chart_data = visualizer.create_chart_data(
        data=sample_data,
        chart_type='bar',
        x_column='name',
        y_column='salary',
        title='Salary by Name'
    )
    print("Chart data created successfully")
    print(f"Chart type: {chart_data.get('type')}")
    print(f"Chart title: {chart_data.get('options', {}).get('plugins', {}).get('title', {}).get('text')}")
    
    # Test summary statistics
    print("\n2. Testing summary statistics...")
    stats = visualizer.create_summary_statistics(sample_data, columns=['age', 'salary'])
    print("Summary statistics created successfully")
    print(f"Total rows: {stats.get('total_rows')}")
    print(f"Age mean: {stats.get('columns', {}).get('age', {}).get('mean')}")
    print(f"Salary mean: {stats.get('columns', {}).get('salary', {}).get('mean')}")
    
    # Test data distribution
    print("\n3. Testing data distribution...")
    distribution = visualizer.create_data_distribution(sample_data, column='age', bins=5)
    print("Data distribution created successfully")
    print(f"Column: {distribution.get('column')}")
    print(f"Type: {distribution.get('type')}")
    if distribution.get('type') == 'numeric':
        print(f"Min: {distribution.get('min')}")
        print(f"Max: {distribution.get('max')}")
        print(f"Mean: {distribution.get('mean')}")
    
    # Test schema visualization
    print("\n4. Testing schema visualization...")
    databases = ["production", "staging", "development"]
    tables = {
        "production": ["users", "orders", "products"],
        "staging": ["users", "orders"],
        "development": ["users", "test_data"]
    }
    schema = visualizer.create_schema_visualization(databases, tables)
    print("Schema visualization created successfully")
    print(f"Number of nodes: {len(schema.get('nodes', []))}")
    print(f"Number of links: {len(schema.get('links', []))}")
    
    print("\nAll tests completed successfully!")


if __name__ == "__main__":
    test_visualizer()