# Clients module CMakeLists.txt

# This is a Python module, so we don't need to build anything with CMake
# But we can add installation rules for the Python files

# Install Python client files
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION share/phantomdb/clients
    FILES_MATCHING PATTERN "*.py"
)

# Install requirements file
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt
    DESTINATION share/phantomdb/clients
)

# Install setup.py
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/setup.py
    DESTINATION share/phantomdb/clients
)

# Install README.md
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/README.md
    DESTINATION share/phantomdb/clients
)