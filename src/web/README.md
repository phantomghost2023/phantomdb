# PhantomDB Web Management Console

A web-based management console for administering PhantomDB instances.

## Features

- Dashboard with server health monitoring
- Database management (create, list, drop)
- Table management (create, list, drop)
- Data browsing and editing
- SQL query execution
- Responsive web interface

## Installation

```bash
pip install phantomdb-web
```

Or install directly from source:

```bash
pip install -e .
```

## Usage

### Running the Web Console

```bash
# Run the web console
python app.py
```

By default, the web console will be available at http://localhost:5000

### Specifying Database URL

You can specify a different PhantomDB server URL:

```bash
python app.py --db-url http://example.com:8080
```

## Web Interface

### Dashboard

The dashboard provides an overview of the PhantomDB server health and basic statistics.

### Database Management

- List all databases
- Create new databases
- Drop existing databases

### Table Management

- List tables in a database
- Create new tables with column definitions
- Drop existing tables

### Data Management

- Browse table data
- Insert new records
- Update existing records
- Delete records

### Query Interface

Execute custom SQL queries against any database.

## API Endpoints

The web console also provides REST API endpoints for programmatic access:

- `GET /api/databases` - List databases
- `POST /api/databases` - Create database
- `DELETE /api/databases` - Drop database
- `GET /api/database/<db_name>/tables` - List tables
- `POST /api/database/<db_name>/tables` - Create table
- `DELETE /api/database/<db_name>/tables` - Drop table
- `GET /api/database/<db_name>/table/<table_name>/data` - Get table data
- `POST /api/database/<db_name>/table/<table_name>/data` - Insert data
- `PUT /api/database/<db_name>/table/<table_name>/data` - Update data
- `DELETE /api/database/<db_name>/table/<table_name>/data` - Delete data
- `POST /api/query` - Execute custom query

## Requirements

- Python 3.7 or higher
- Flask web framework
- PhantomDB Python client

## License

MIT License