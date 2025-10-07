# Phantom-DB Scripts

This directory contains utility scripts for managing the Phantom-DB project.

## Available Scripts

### update-progress.py
A Python script to update task progress in the [PROJECT_TRACKING.md](file:///D:/PhantomGhost/Storage/Media/Media/Projects/MyProjects/PhantomDB/docs/PROJECT_TRACKING.md) file.

**Usage:**
```bash
python update-progress.py "section" "task" "status"
```

**Example:**
```bash
python update-progress.py "Research & Design" "Study failure points in existing databases (SQL/NoSQL)" complete
```

### update-progress.bat
A Windows batch script wrapper for the update-progress functionality.

**Usage:**
```cmd
update-progress.bat "section" "task" "status"
```

### build_rest_api.bat
A Windows batch script to build the PhantomDB REST API server.

**Usage:**
```cmd
build_rest_api.bat
```

### run_rest_api.bat
A Windows batch script to run the PhantomDB REST API server.

**Usage:**
```cmd
run_rest_api.bat
```

### rest_api_example.sh
A bash script demonstrating how to use the PhantomDB REST API with curl commands.

**Usage:**
```bash
./rest_api_example.sh
```

### rest_api_example.bat
A Windows batch script demonstrating how to use the PhantomDB REST API with curl commands.

**Usage:**
```cmd
rest_api_example.bat
```

## Adding New Scripts

When adding new scripts to this directory:

1. Ensure they follow the project's coding standards
2. Add documentation to this README
3. Make them executable where appropriate
4. Test on all supported platforms