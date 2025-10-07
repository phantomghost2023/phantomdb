"""
PhantomDB Web Management Console

A web-based interface for administering PhantomDB instances.
"""

import os
import json
from flask import Flask, render_template, request, jsonify, redirect, url_for
from phantomdb import PhantomDB  # Using our existing Python client
# Import the visualization tools
from visualizations.visualizer import DataVisualizer


class PhantomDBWebConsole:
    """Web-based management console for PhantomDB."""
    
    def __init__(self, db_url="http://localhost:8080"):
        """Initialize the web console with database connection."""
        self.app = Flask(__name__)
        self.db = PhantomDB(db_url)
        self.visualizer = DataVisualizer()
        self.setup_routes()
    
    def setup_routes(self):
        """Set up URL routes for the web application."""
        self.app.add_url_rule('/', 'index', self.index)
        self.app.add_url_rule('/databases', 'databases', self.databases)
        self.app.add_url_rule('/database/<db_name>', 'database', self.database)
        self.app.add_url_rule('/database/<db_name>/tables', 'tables', self.tables)
        self.app.add_url_rule('/database/<db_name>/table/<table_name>', 'table', self.table)
        self.app.add_url_rule('/query', 'query', self.query, methods=['GET', 'POST'])
        self.app.add_url_rule('/health', 'health', self.health)
        self.app.add_url_rule('/visualizations', 'visualizations', self.visualizations)
        self.app.add_url_rule('/visualizations/schema', 'schema_visualization', self.schema_visualization)
        self.app.add_url_rule('/visualizations/data', 'data_visualization', self.data_visualization, methods=['GET', 'POST'])
        self.app.add_url_rule('/api/databases', 'api_databases', self.api_databases, methods=['GET', 'POST', 'DELETE'])
        self.app.add_url_rule('/api/database/<db_name>/tables', 'api_tables', self.api_tables, methods=['GET', 'POST', 'DELETE'])
        self.app.add_url_rule('/api/database/<db_name>/table/<table_name>/data', 'api_table_data', self.api_table_data, methods=['GET', 'POST', 'PUT', 'DELETE'])
        self.app.add_url_rule('/api/query', 'api_query', self.api_query, methods=['POST'])
        self.app.add_url_rule('/api/visualizations/chart', 'api_chart_data', self.api_chart_data, methods=['POST'])
        self.app.add_url_rule('/api/visualizations/stats', 'api_stats_data', self.api_stats_data, methods=['POST'])
        self.app.add_url_rule('/api/visualizations/distribution', 'api_distribution_data', self.api_distribution_data, methods=['POST'])
    
    def index(self):
        """Render the main dashboard page."""
        try:
            health = self.db.health_check()
            return render_template('index.html', health=health)
        except Exception as e:
            return render_template('index.html', health={"error": str(e)})
    
    def databases(self):
        """Render the databases page."""
        try:
            databases = self.db.list_databases()
            return render_template('databases.html', databases=databases)
        except Exception as e:
            return render_template('databases.html', databases=[], error=str(e))
    
    def database(self, db_name):
        """Render a specific database page."""
        try:
            # Get database info
            return render_template('database.html', db_name=db_name)
        except Exception as e:
            return render_template('database.html', db_name=db_name, error=str(e))
    
    def tables(self, db_name):
        """Render the tables page for a database."""
        try:
            tables = self.db.list_tables(db_name)
            return render_template('tables.html', db_name=db_name, tables=tables)
        except Exception as e:
            return render_template('tables.html', db_name=db_name, tables=[], error=str(e))
    
    def table(self, db_name, table_name):
        """Render a specific table page."""
        try:
            # Get table data (limited for performance)
            data = self.db.select(db_name, table_name)
            # Limit to first 100 rows for display
            limited_data = data[:100] if isinstance(data, list) else []
            return render_template('table.html', db_name=db_name, table_name=table_name, data=limited_data)
        except Exception as e:
            return render_template('table.html', db_name=db_name, table_name=table_name, data=[], error=str(e))
    
    def query(self):
        """Render the query page."""
        if request.method == 'POST':
            try:
                db_name = request.form.get('database')
                query_text = request.form.get('query')
                if db_name and query_text:
                    result = self.db.execute_query(db_name, query_text)
                    return render_template('query.html', result=result, database=db_name, query=query_text)
                else:
                    return render_template('query.html', error="Database and query are required")
            except Exception as e:
                return render_template('query.html', error=str(e))
        else:
            # GET request - show the query form
            try:
                databases = self.db.list_databases()
                return render_template('query.html', databases=databases)
            except Exception as e:
                return render_template('query.html', databases=[], error=str(e))
    
    def health(self):
        """Render the health check page."""
        try:
            health = self.db.health_check()
            return render_template('health.html', health=health)
        except Exception as e:
            return render_template('health.html', health={"error": str(e)})
    
    def visualizations(self):
        """Render the visualizations dashboard page."""
        try:
            databases = self.db.list_databases()
            return render_template('visualizations.html', databases=databases)
        except Exception as e:
            return render_template('visualizations.html', databases=[], error=str(e))
    
    def schema_visualization(self):
        """Render the schema visualization page."""
        try:
            databases = self.db.list_databases()
            # Get tables for each database
            db_tables = {}
            for db in databases:
                try:
                    tables = self.db.list_tables(db)
                    db_tables[db] = tables
                except:
                    db_tables[db] = []
            return render_template('schema_visualization.html', databases=databases, db_tables=db_tables)
        except Exception as e:
            return render_template('schema_visualization.html', databases=[], db_tables={}, error=str(e))
    
    def data_visualization(self):
        """Render the data visualization page."""
        if request.method == 'POST':
            try:
                db_name = request.form.get('database')
                table_name = request.form.get('table')
                chart_type = request.form.get('chart_type', 'bar')
                x_column = request.form.get('x_column')
                y_column = request.form.get('y_column')
                
                if db_name and table_name and x_column and y_column:
                    # Get table data
                    data = self.db.select(db_name, table_name)
                    # Create chart data
                    chart_data = self.visualizer.create_chart_data(
                        data=data,
                        chart_type=chart_type,
                        x_column=x_column,
                        y_column=y_column,
                        title=f'{table_name} - {y_column} by {x_column}'
                    )
                    return render_template('data_visualization.html', 
                                         chart_data=json.dumps(chart_data),
                                         db_name=db_name,
                                         table_name=table_name,
                                         chart_type=chart_type,
                                         x_column=x_column,
                                         y_column=y_column)
                else:
                    return render_template('data_visualization.html', error="All fields are required")
            except Exception as e:
                return render_template('data_visualization.html', error=str(e))
        else:
            # GET request - show the visualization form
            try:
                databases = self.db.list_databases()
                return render_template('data_visualization.html', databases=databases)
            except Exception as e:
                return render_template('data_visualization.html', databases=[], error=str(e))
    
    # API endpoints for AJAX requests
    def api_databases(self):
        """API endpoint for database operations."""
        if request.method == 'GET':
            try:
                databases = self.db.list_databases()
                return jsonify({"databases": databases})
            except Exception as e:
                return jsonify({"error": str(e)}), 500
        elif request.method == 'POST':
            try:
                data = request.get_json()
                db_name = data.get('name')
                if db_name:
                    result = self.db.create_database(db_name)
                    return jsonify(result)
                else:
                    return jsonify({"error": "Database name is required"}), 400
            except Exception as e:
                return jsonify({"error": str(e)}), 500
        elif request.method == 'DELETE':
            try:
                data = request.get_json()
                db_name = data.get('name')
                if db_name:
                    result = self.db.drop_database(db_name)
                    return jsonify(result)
                else:
                    return jsonify({"error": "Database name is required"}), 400
            except Exception as e:
                return jsonify({"error": str(e)}), 500
    
    def api_tables(self, db_name):
        """API endpoint for table operations."""
        if request.method == 'GET':
            try:
                tables = self.db.list_tables(db_name)
                return jsonify({"tables": tables})
            except Exception as e:
                return jsonify({"error": str(e)}), 500
        elif request.method == 'POST':
            try:
                data = request.get_json()
                table_name = data.get('name')
                columns = data.get('columns', [])
                if table_name:
                    result = self.db.create_table(db_name, table_name, columns)
                    return jsonify(result)
                else:
                    return jsonify({"error": "Table name is required"}), 400
            except Exception as e:
                return jsonify({"error": str(e)}), 500
        elif request.method == 'DELETE':
            try:
                data = request.get_json()
                table_name = data.get('name')
                if table_name:
                    result = self.db.drop_table(db_name, table_name)
                    return jsonify(result)
                else:
                    return jsonify({"error": "Table name is required"}), 400
            except Exception as e:
                return jsonify({"error": str(e)}), 500
    
    def api_table_data(self, db_name, table_name):
        """API endpoint for table data operations."""
        if request.method == 'GET':
            try:
                # Get query parameters for filtering
                condition = request.args.get('condition')
                data = self.db.select(db_name, table_name, condition)
                return jsonify({"data": data})
            except Exception as e:
                return jsonify({"error": str(e)}), 500
        elif request.method == 'POST':
            try:
                data = request.get_json()
                if data:
                    result = self.db.insert(db_name, table_name, data)
                    return jsonify(result)
                else:
                    return jsonify({"error": "Data is required"}), 400
            except Exception as e:
                return jsonify({"error": str(e)}), 500
        elif request.method == 'PUT':
            try:
                data = request.get_json()
                update_data = data.get('data', {})
                condition = data.get('condition')
                if update_data:
                    result = self.db.update(db_name, table_name, update_data, condition)
                    return jsonify(result)
                else:
                    return jsonify({"error": "Update data is required"}), 400
            except Exception as e:
                return jsonify({"error": str(e)}), 500
        elif request.method == 'DELETE':
            try:
                data = request.get_json()
                condition = data.get('condition')
                result = self.db.delete(db_name, table_name, condition)
                return jsonify(result)
            except Exception as e:
                return jsonify({"error": str(e)}), 500
    
    def api_query(self):
        """API endpoint for executing queries."""
        try:
            data = request.get_json()
            db_name = data.get('database')
            query_text = data.get('query')
            if db_name and query_text:
                result = self.db.execute_query(db_name, query_text)
                return jsonify(result)
            else:
                return jsonify({"error": "Database and query are required"}), 400
        except Exception as e:
            return jsonify({"error": str(e)}), 500
    
    def api_chart_data(self):
        """API endpoint for creating chart data."""
        try:
            data = request.get_json()
            chart_data = self.visualizer.create_chart_data(
                data=data.get('data', []),
                chart_type=data.get('chart_type', 'bar'),
                x_column=data.get('x_column', ''),
                y_column=data.get('y_column', ''),
                title=data.get('title', 'Chart')
            )
            return jsonify(chart_data)
        except Exception as e:
            return jsonify({"error": str(e)}), 500
    
    def api_stats_data(self):
        """API endpoint for creating summary statistics."""
        try:
            data = request.get_json()
            stats_data = self.visualizer.create_summary_statistics(
                data=data.get('data', []),
                columns=data.get('columns', [])
            )
            return jsonify(stats_data)
        except Exception as e:
            return jsonify({"error": str(e)}), 500
    
    def api_distribution_data(self):
        """API endpoint for creating data distribution."""
        try:
            data = request.get_json()
            distribution_data = self.visualizer.create_data_distribution(
                data=data.get('data', []),
                column=data.get('column', ''),
                bins=data.get('bins', 10)
            )
            return jsonify(distribution_data)
        except Exception as e:
            return jsonify({"error": str(e)}), 500
    
    def run(self, host='localhost', port=5000, debug=False):
        """Run the web application."""
        self.app.run(host=host, port=port, debug=debug)


# Create the Flask app instance
def create_app():
    """Application factory function."""
    console = PhantomDBWebConsole()
    return console.app


if __name__ == '__main__':
    # Run the web console
    console = PhantomDBWebConsole()
    console.run(debug=True)