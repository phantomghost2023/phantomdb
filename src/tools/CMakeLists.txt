# Tools module CMakeLists.txt

# This is a Python module, so we don't need to build anything with CMake
# But we can add installation rules for the Python files

# Install Python tools files
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION share/phantomdb/tools
    FILES_MATCHING PATTERN "*.py"
)

# Install requirements file
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt
    DESTINATION share/phantomdb/tools
)

# Install setup.py
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/setup.py
    DESTINATION share/phantomdb/tools
)

# Install README.md
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/README.md
    DESTINATION share/phantomdb/tools
)

# Install debugger directory
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/debugger/
    DESTINATION share/phantomdb/tools/debugger
    FILES_MATCHING PATTERN "*.py"
)

# Install debugger requirements
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/debugger/requirements.txt
    DESTINATION share/phantomdb/tools/debugger
)

# Install debugger setup.py
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/debugger/setup.py
    DESTINATION share/phantomdb/tools/debugger
)

# Install debugger README.md
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/debugger/README.md
    DESTINATION share/phantomdb/tools/debugger
)