# CMakeLists.txt for PhantomDB Query Module

# Define the query library
add_library(query
    sql_parser.cpp
    query_planner.cpp
    enhanced_query_planner.cpp
    query_optimizer.cpp
    query_processor.cpp
    execution_engine.cpp
)

# Link dependencies
target_link_libraries(query PRIVATE core storage)

# Include directories
target_include_directories(query PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Install targets
install(TARGETS query
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

# Tests
add_executable(sql_parser_test parser_test.cpp)
target_link_libraries(sql_parser_test query)

add_executable(query_planner_test planner_test.cpp)
target_link_libraries(query_planner_test query)

add_executable(query_optimizer_test optimizer_test.cpp)
target_link_libraries(query_optimizer_test query)

add_executable(query_test query_test.cpp)
target_link_libraries(query_test query)

add_executable(execution_engine_test execution_engine_test.cpp)
target_link_libraries(execution_engine_test query)

add_executable(simple_parser_test simple_parser_test.cpp)
target_link_libraries(simple_parser_test query)

add_executable(simple_optimizer_test simple_optimizer_test.cpp)
target_link_libraries(simple_optimizer_test query)

add_executable(simple_query_test simple_query_test.cpp)
target_link_libraries(simple_query_test query)

add_executable(simple_execution_test simple_execution_test.cpp)
target_link_libraries(simple_execution_test query)

add_executable(insert_test insert_test.cpp)
target_link_libraries(insert_test query)

add_executable(insert_plan_test insert_plan_test.cpp)
target_link_libraries(insert_plan_test query)

add_executable(insert_execution_test insert_execution_test.cpp)
target_link_libraries(insert_execution_test query)

add_executable(update_test update_test.cpp)
target_link_libraries(update_test query)

add_executable(update_plan_test update_plan_test.cpp)
target_link_libraries(update_plan_test query)

add_executable(update_execution_test update_execution_test.cpp)
target_link_libraries(update_execution_test query)

add_executable(delete_test delete_test.cpp)
target_link_libraries(delete_test query)

add_executable(delete_plan_test delete_plan_test.cpp)
target_link_libraries(delete_plan_test query)

add_executable(delete_execution_test delete_execution_test.cpp)
target_link_libraries(delete_execution_test query)

add_executable(join_test join_test.cpp)
target_link_libraries(join_test query)

add_executable(join_plan_test join_plan_test.cpp)
target_link_libraries(join_plan_test query)

add_executable(join_execution_test join_execution_test.cpp)
target_link_libraries(join_execution_test query)

add_executable(join_comprehensive_test join_comprehensive_test.cpp)
target_link_libraries(join_comprehensive_test query)

add_executable(optimizer_enhancement_test optimizer_enhancement_test.cpp)
target_link_libraries(optimizer_enhancement_test query)

# Test for enhanced query planner
add_executable(test_enhanced_query_planner test_enhanced_query_planner.cpp)
target_link_libraries(test_enhanced_query_planner query)