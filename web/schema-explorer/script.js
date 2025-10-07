// PhantomDB Schema Explorer JavaScript

// Sample data for demonstration
const sampleData = {
    databases: [
        { id: 1, name: "testdb", tables: [1, 2, 3] },
        { id: 2, name: "production", tables: [4, 5] },
        { id: 3, name: "analytics", tables: [6] }
    ],
    tables: [
        { 
            id: 1, 
            name: "users", 
            databaseId: 1,
            columns: [
                { name: "id", type: "INT", primaryKey: true, nullable: false },
                { name: "name", type: "VARCHAR(50)", primaryKey: false, nullable: false },
                { name: "email", type: "VARCHAR(100)", primaryKey: false, nullable: true },
                { name: "created_at", type: "TIMESTAMP", primaryKey: false, nullable: false }
            ],
            relationships: [
                { type: "one-to-many", targetTable: 2, sourceColumn: "id", targetColumn: "user_id" }
            ]
        },
        { 
            id: 2, 
            name: "orders", 
            databaseId: 1,
            columns: [
                { name: "id", type: "INT", primaryKey: true, nullable: false },
                { name: "user_id", type: "INT", primaryKey: false, nullable: false },
                { name: "product_name", type: "VARCHAR(100)", primaryKey: false, nullable: false },
                { name: "quantity", type: "INT", primaryKey: false, nullable: false },
                { name: "order_date", type: "TIMESTAMP", primaryKey: false, nullable: false }
            ],
            relationships: [
                { type: "many-to-one", targetTable: 1, sourceColumn: "user_id", targetColumn: "id" }
            ]
        },
        { 
            id: 3, 
            name: "products", 
            databaseId: 1,
            columns: [
                { name: "id", type: "INT", primaryKey: true, nullable: false },
                { name: "name", type: "VARCHAR(100)", primaryKey: false, nullable: false },
                { name: "price", type: "DECIMAL(10,2)", primaryKey: false, nullable: false },
                { name: "category", type: "VARCHAR(50)", primaryKey: false, nullable: true }
            ],
            relationships: []
        },
        { 
            id: 4, 
            name: "customers", 
            databaseId: 2,
            columns: [
                { name: "id", type: "INT", primaryKey: true, nullable: false },
                { name: "company_name", type: "VARCHAR(100)", primaryKey: false, nullable: false },
                { name: "contact_person", type: "VARCHAR(100)", primaryKey: false, nullable: true },
                { name: "email", type: "VARCHAR(100)", primaryKey: false, nullable: false }
            ],
            relationships: []
        },
        { 
            id: 5, 
            name: "invoices", 
            databaseId: 2,
            columns: [
                { name: "id", type: "INT", primaryKey: true, nullable: false },
                { name: "customer_id", type: "INT", primaryKey: false, nullable: false },
                { name: "amount", type: "DECIMAL(10,2)", primaryKey: false, nullable: false },
                { name: "invoice_date", type: "TIMESTAMP", primaryKey: false, nullable: false }
            ],
            relationships: [
                { type: "many-to-one", targetTable: 4, sourceColumn: "customer_id", targetColumn: "id" }
            ]
        },
        { 
            id: 6, 
            name: "user_analytics", 
            databaseId: 3,
            columns: [
                { name: "id", type: "INT", primaryKey: true, nullable: false },
                { name: "user_id", type: "INT", primaryKey: false, nullable: false },
                { name: "page_views", type: "INT", primaryKey: false, nullable: false },
                { name: "session_duration", type: "INT", primaryKey: false, nullable: false },
                { name: "last_visit", type: "TIMESTAMP", primaryKey: false, nullable: false }
            ],
            relationships: [
                { type: "many-to-one", targetTable: 1, sourceColumn: "user_id", targetColumn: "id" }
            ]
        }
    ]
};

// Current state
let currentDatabase = null;
let currentTable = null;

// Initialize the application
function init() {
    renderDatabaseList();
    setupEventListeners();
}

// Render the database list
function renderDatabaseList() {
    const databaseList = document.getElementById('database-list');
    databaseList.innerHTML = '';
    
    sampleData.databases.forEach(db => {
        const li = document.createElement('li');
        li.textContent = db.name;
        li.dataset.id = db.id;
        li.addEventListener('click', () => selectDatabase(db));
        databaseList.appendChild(li);
    });
}

// Select a database
function selectDatabase(database) {
    currentDatabase = database;
    
    // Update UI
    document.querySelectorAll('#database-list li').forEach(li => {
        li.classList.remove('selected');
    });
    event.target.classList.add('selected');
    
    // Render tables for this database
    renderTableList();
    
    // Clear diagram and table details
    clearDiagram();
    clearTableDetails();
}

// Render the table list for the current database
function renderTableList() {
    const tableList = document.getElementById('table-list');
    tableList.innerHTML = '';
    
    if (!currentDatabase) return;
    
    const tables = sampleData.tables.filter(table => 
        currentDatabase.tables.includes(table.id)
    );
    
    tables.forEach(table => {
        const li = document.createElement('li');
        li.textContent = table.name;
        li.dataset.id = table.id;
        li.addEventListener('click', () => selectTable(table));
        tableList.appendChild(li);
    });
}

// Select a table
function selectTable(table) {
    currentTable = table;
    
    // Update UI
    document.querySelectorAll('#table-list li').forEach(li => {
        li.classList.remove('selected');
    });
    event.target.classList.add('selected');
    
    // Render table details
    renderTableDetails();
    
    // Render schema diagram
    renderSchemaDiagram();
}

// Render table details
function renderTableDetails() {
    const detailsContainer = document.getElementById('table-details-content');
    
    if (!currentTable) {
        detailsContainer.innerHTML = '<p>Select a table to view details</p>';
        return;
    }
    
    let html = `<h3>${currentTable.name}</h3>`;
    html += '<table>';
    html += '<thead><tr><th>Column</th><th>Type</th><th>Primary Key</th><th>Nullable</th></tr></thead>';
    html += '<tbody>';
    
    currentTable.columns.forEach(column => {
        html += '<tr>';
        html += `<td>${column.name}</td>`;
        html += `<td>${column.type}</td>`;
        html += `<td>${column.primaryKey ? 'Yes' : 'No'}</td>`;
        html += `<td>${column.nullable ? 'Yes' : 'No'}</td>`;
        html += '</tr>';
    });
    
    html += '</tbody></table>';
    
    // Add relationships if any
    if (currentTable.relationships && currentTable.relationships.length > 0) {
        html += '<h4>Relationships</h4>';
        html += '<ul>';
        currentTable.relationships.forEach(rel => {
            const targetTable = sampleData.tables.find(t => t.id === rel.targetTable);
            if (targetTable) {
                html += `<li>${rel.type}: ${currentTable.name}.${rel.sourceColumn} → ${targetTable.name}.${rel.targetColumn}</li>`;
            }
        });
        html += '</ul>';
    }
    
    detailsContainer.innerHTML = html;
}

// Render schema diagram
function renderSchemaDiagram() {
    const canvas = document.getElementById('schema-diagram');
    const ctx = canvas.getContext('2d');
    
    // Clear canvas
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    
    if (!currentDatabase) return;
    
    // Get tables for this database
    const tables = sampleData.tables.filter(table => 
        currentDatabase.tables.includes(table.id)
    );
    
    // Draw tables
    const tableWidth = 200;
    const tableHeight = 100;
    const padding = 50;
    
    tables.forEach((table, index) => {
        const x = padding + (index % 3) * (tableWidth + padding);
        const y = padding + Math.floor(index / 3) * (tableHeight + padding);
        
        // Draw table rectangle
        ctx.fillStyle = '#667eea';
        ctx.fillRect(x, y, tableWidth, 30);
        ctx.fillStyle = '#f8f9fa';
        ctx.fillRect(x, y + 30, tableWidth, tableHeight - 30);
        
        // Draw table name
        ctx.fillStyle = 'white';
        ctx.font = 'bold 14px Arial';
        ctx.textAlign = 'center';
        ctx.fillText(table.name, x + tableWidth / 2, y + 20);
        
        // Draw column count
        ctx.fillStyle = 'black';
        ctx.font = '12px Arial';
        ctx.textAlign = 'left';
        ctx.fillText(`${table.columns.length} columns`, x + 10, y + 50);
        
        // Highlight selected table
        if (table === currentTable) {
            ctx.strokeStyle = '#ff6b6b';
            ctx.lineWidth = 3;
            ctx.strokeRect(x - 2, y - 2, tableWidth + 4, tableHeight + 4);
        }
    });
    
    // Draw relationships
    tables.forEach(table => {
        if (table.relationships) {
            table.relationships.forEach(rel => {
                const sourceTable = table;
                const targetTable = sampleData.tables.find(t => t.id === rel.targetTable);
                
                if (sourceTable && targetTable) {
                    // Find positions
                    const sourceIndex = tables.indexOf(sourceTable);
                    const targetIndex = tables.indexOf(targetTable);
                    
                    if (sourceIndex !== -1 && targetIndex !== -1) {
                        const sourceX = padding + (sourceIndex % 3) * (tableWidth + padding) + tableWidth / 2;
                        const sourceY = padding + Math.floor(sourceIndex / 3) * (tableHeight + padding) + tableHeight;
                        const targetX = padding + (targetIndex % 3) * (tableWidth + padding) + tableWidth / 2;
                        const targetY = padding + Math.floor(targetIndex / 3) * (tableHeight + padding);
                        
                        // Draw line
                        ctx.beginPath();
                        ctx.moveTo(sourceX, sourceY);
                        ctx.lineTo(targetX, targetY);
                        ctx.strokeStyle = '#667eea';
                        ctx.lineWidth = 2;
                        ctx.stroke();
                        
                        // Draw arrowhead
                        drawArrowhead(ctx, sourceX, sourceY, targetX, targetY);
                    }
                }
            });
        }
    });
}

// Draw arrowhead for relationships
function drawArrowhead(ctx, fromX, fromY, toX, toY) {
    const angle = Math.atan2(toY - fromY, toX - fromX);
    const arrowLength = 10;
    
    ctx.beginPath();
    ctx.moveTo(toX, toY);
    ctx.lineTo(
        toX - arrowLength * Math.cos(angle - Math.PI / 6),
        toY - arrowLength * Math.sin(angle - Math.PI / 6)
    );
    ctx.lineTo(
        toX - arrowLength * Math.cos(angle + Math.PI / 6),
        toY - arrowLength * Math.sin(angle + Math.PI / 6)
    );
    ctx.closePath();
    ctx.fillStyle = '#667eea';
    ctx.fill();
}

// Clear diagram
function clearDiagram() {
    const canvas = document.getElementById('schema-diagram');
    const ctx = canvas.getContext('2d');
    ctx.clearRect(0, 0, canvas.width, canvas.height);
}

// Clear table details
function clearTableDetails() {
    const detailsContainer = document.getElementById('table-details-content');
    detailsContainer.innerHTML = '<p>Select a table to view details</p>';
}

// Setup event listeners
function setupEventListeners() {
    // Add any additional event listeners here
}

// Initialize when the page loads
document.addEventListener('DOMContentLoaded', init);