"""
PhantomDB Data Visualization Tools

This module provides data visualization capabilities for PhantomDB,
including charting, graphing, and dashboard visualization features.
"""

import json
import pandas as pd
import numpy as np
from typing import Dict, List, Any, Optional
from datetime import datetime


class DataVisualizer:
    """Data visualization tools for PhantomDB."""
    
    def __init__(self):
        """Initialize the data visualizer."""
        pass
    
    def create_chart_data(self, data: List[Dict], chart_type: str, 
                         x_column: str, y_column: str, 
                         title: str = "Chart") -> Dict[str, Any]:
        """
        Create chart data from database results.
        
        Args:
            data: List of dictionaries representing database rows
            chart_type: Type of chart ('bar', 'line', 'pie', 'scatter')
            x_column: Column name for x-axis
            y_column: Column name for y-axis
            title: Chart title
            
        Returns:
            Dictionary with chart configuration and data
        """
        if not data:
            return {"error": "No data provided"}
        
        # Extract x and y values
        x_values = []
        y_values = []
        
        for row in data:
            if x_column in row and y_column in row:
                x_values.append(row[x_column])
                y_values.append(row[y_column])
        
        if not x_values or not y_values:
            return {"error": f"Columns {x_column} or {y_column} not found in data"}
        
        # Create chart configuration
        chart_config = {
            "type": chart_type,
            "title": title,
            "data": {
                "labels": x_values,
                "datasets": [{
                    "label": y_column,
                    "data": y_values,
                    "backgroundColor": self._get_background_colors(chart_type, len(y_values)),
                    "borderColor": self._get_border_colors(chart_type, len(y_values)),
                    "borderWidth": 1
                }]
            },
            "options": {
                "responsive": True,
                "maintainAspectRatio": False,
                "scales": {},
                "plugins": {
                    "title": {
                        "display": True,
                        "text": title
                    }
                }
            }
        }
        
        # Add scales for bar and line charts
        if chart_type in ['bar', 'line']:
            chart_config["options"]["scales"] = {
                "y": {
                    "beginAtZero": True
                }
            }
        
        return chart_config
    
    def create_summary_statistics(self, data: List[Dict], columns: List[str]) -> Dict[str, Any]:
        """
        Create summary statistics for numerical columns.
        
        Args:
            data: List of dictionaries representing database rows
            columns: List of column names to analyze
            
        Returns:
            Dictionary with summary statistics
        """
        if not data:
            return {"error": "No data provided"}
        
        # Convert to DataFrame for easier analysis
        df = pd.DataFrame(data)
        
        # Filter to only requested columns
        if columns:
            df = df[columns]
        
        # Calculate statistics for numerical columns
        stats = {}
        for column in df.columns:
            if df[column].dtype in ['int64', 'float64']:
                stats[column] = {
                    "count": int(df[column].count()),
                    "mean": float(df[column].mean()),
                    "std": float(df[column].std()),
                    "min": float(df[column].min()),
                    "max": float(df[column].max()),
                    "median": float(df[column].median()),
                    "quartiles": {
                        "q1": float(df[column].quantile(0.25)),
                        "q2": float(df[column].quantile(0.50)),
                        "q3": float(df[column].quantile(0.75))
                    }
                }
            else:
                # For non-numerical columns, provide count and unique values
                stats[column] = {
                    "count": int(df[column].count()),
                    "unique": int(df[column].nunique()),
                    "most_frequent": str(df[column].mode().iloc[0]) if not df[column].mode().empty else "N/A"
                }
        
        return {
            "columns": stats,
            "total_rows": len(data),
            "generated_at": datetime.now().isoformat()
        }
    
    def create_data_distribution(self, data: List[Dict], column: str, 
                               bins: int = 10) -> Dict[str, Any]:
        """
        Create data distribution information for a column.
        
        Args:
            data: List of dictionaries representing database rows
            column: Column name to analyze
            bins: Number of bins for histogram
            
        Returns:
            Dictionary with distribution data
        """
        if not data:
            return {"error": "No data provided"}
        
        # Extract column values
        values = [row[column] for row in data if column in row]
        
        if not values:
            return {"error": f"Column {column} not found in data"}
        
        # Convert to numeric if possible
        try:
            numeric_values = [float(v) for v in values]
            # Create histogram data
            hist, bin_edges = np.histogram(numeric_values, bins=bins)
            
            return {
                "column": column,
                "type": "numeric",
                "count": len(numeric_values),
                "min": float(min(numeric_values)),
                "max": float(max(numeric_values)),
                "mean": float(np.mean(numeric_values)),
                "std": float(np.std(numeric_values)),
                "histogram": {
                    "bins": [float(edge) for edge in bin_edges],
                    "frequencies": [int(freq) for freq in hist]
                }
            }
        except ValueError:
            # Non-numeric data - create frequency distribution
            value_counts = {}
            for value in values:
                value_counts[value] = value_counts.get(value, 0) + 1
            
            # Sort by frequency
            sorted_counts = sorted(value_counts.items(), key=lambda x: x[1], reverse=True)
            
            return {
                "column": column,
                "type": "categorical",
                "count": len(values),
                "unique_values": len(value_counts),
                "top_values": [{"value": str(item[0]), "count": item[1]} for item in sorted_counts[:10]]
            }
    
    def create_schema_visualization(self, databases: List[str], 
                                  tables: Dict[str, List[str]]) -> Dict[str, Any]:
        """
        Create a schema visualization showing database and table relationships.
        
        Args:
            databases: List of database names
            tables: Dictionary mapping database names to lists of table names
            
        Returns:
            Dictionary with schema visualization data
        """
        nodes = []
        links = []
        
        # Add databases as nodes
        for i, db in enumerate(databases):
            nodes.append({
                "id": f"db_{i}",
                "name": db,
                "type": "database",
                "size": 20
            })
        
        # Add tables as nodes and create links
        table_index = len(databases)
        for db_index, (db, db_tables) in enumerate(tables.items()):
            db_node_id = f"db_{db_index}"
            for table in db_tables:
                nodes.append({
                    "id": f"table_{table_index}",
                    "name": table,
                    "type": "table",
                    "size": 10
                })
                # Link database to table
                links.append({
                    "source": db_node_id,
                    "target": f"table_{table_index}",
                    "type": "contains"
                })
                table_index += 1
        
        return {
            "nodes": nodes,
            "links": links,
            "generated_at": datetime.now().isoformat()
        }
    
    def _get_background_colors(self, chart_type: str, count: int) -> List[str]:
        """Get background colors for chart elements."""
        if chart_type == 'pie':
            # Use different colors for pie charts
            colors = [
                'rgba(255, 99, 132, 0.6)',
                'rgba(54, 162, 235, 0.6)',
                'rgba(255, 205, 86, 0.6)',
                'rgba(75, 192, 192, 0.6)',
                'rgba(153, 102, 255, 0.6)',
                'rgba(255, 159, 64, 0.6)',
                'rgba(199, 199, 199, 0.6)',
                'rgba(83, 102, 255, 0.6)',
                'rgba(255, 99, 255, 0.6)',
                'rgba(99, 255, 132, 0.6)'
            ]
            # Repeat colors if needed
            return (colors * (count // len(colors) + 1))[:count]
        else:
            # Use single color for other charts
            return ['rgba(54, 162, 235, 0.6)'] * count
    
    def _get_border_colors(self, chart_type: str, count: int) -> List[str]:
        """Get border colors for chart elements."""
        if chart_type == 'pie':
            # Use different border colors for pie charts
            colors = [
                'rgba(255, 99, 132, 1)',
                'rgba(54, 162, 235, 1)',
                'rgba(255, 205, 86, 1)',
                'rgba(75, 192, 192, 1)',
                'rgba(153, 102, 255, 1)',
                'rgba(255, 159, 64, 1)',
                'rgba(199, 199, 199, 1)',
                'rgba(83, 102, 255, 1)',
                'rgba(255, 99, 255, 1)',
                'rgba(99, 255, 132, 1)'
            ]
            # Repeat colors if needed
            return (colors * (count // len(colors) + 1))[:count]
        else:
            # Use single border color for other charts
            return ['rgba(54, 162, 235, 1)'] * count