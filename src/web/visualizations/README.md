# PhantomDB Data Visualization Tools

Data visualization tools for PhantomDB web management console.

## Features

1. **Chart Generation**: Create various types of charts from database query results
2. **Summary Statistics**: Generate statistical summaries of numerical data
3. **Data Distribution**: Analyze data distribution patterns
4. **Schema Visualization**: Visualize database and table relationships

## Installation

```bash
pip install -e .
```

## Usage

```python
from visualizations.visualizer import DataVisualizer

# Create a visualizer instance
visualizer = DataVisualizer()

# Create chart data
chart_data = visualizer.create_chart_data(
    data=query_results,
    chart_type='bar',
    x_column='name',
    y_column='value',
    title='Sample Chart'
)

# Create summary statistics
stats = visualizer.create_summary_statistics(data, columns=['age', 'salary'])

# Create data distribution
distribution = visualizer.create_data_distribution(data, column='age', bins=10)

# Create schema visualization
schema = visualizer.create_schema_visualization(databases, tables)
```

## Requirements

- Python 3.7 or higher
- Flask 2.0+
- Pandas 1.3+
- NumPy 1.21+

## License

MIT License