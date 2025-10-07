# PhantomDB Visual Schema Explorer

A web-based UI to browse databases, tables, and relationships in PhantomDB.

## Features

- Visual representation of database schemas
- Interactive diagram showing table relationships
- Detailed table information including columns and constraints
- Database and table navigation

## Usage

To use the Visual Schema Explorer:

1. Open `index.html` in a web browser
2. Select a database from the list
3. Browse tables in the selected database
4. Click on a table to view its details and see it highlighted in the schema diagram

## Components

- `index.html` - Main HTML structure
- `styles.css` - Styling for the UI
- `script.js` - JavaScript functionality for rendering and interaction

## API Integration

In a production environment, this UI would connect to the PhantomDB REST API to retrieve real schema information. The current implementation uses sample data for demonstration purposes.

## Example API Endpoints (Planned)

- `GET /api/databases` - List all databases
- `GET /api/databases/{id}/tables` - List tables in a database
- `GET /api/tables/{id}` - Get detailed information about a table
- `GET /api/schema` - Get complete schema information for visualization

## Development

To modify the Visual Schema Explorer:

1. Edit the HTML structure in `index.html`
2. Update styling in `styles.css`
3. Modify functionality in `script.js`

## Future Enhancements

- Real-time schema updates
- Export schema diagrams as images
- Advanced filtering and search
- Collaboration features for team environments