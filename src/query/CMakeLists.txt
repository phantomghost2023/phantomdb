# Query module CMakeLists.txt
set(QUERY_SOURCES
    query_processor.cpp
    sql_parser.cpp
    query_planner.cpp
    query_optimizer.cpp
    execution_engine.cpp
)

set(QUERY_HEADERS
    query_processor.h
    sql_parser.h
    query_planner.h
    query_optimizer.h
    execution_engine.h
)

add_library(query ${QUERY_SOURCES} ${QUERY_HEADERS})
target_link_libraries(query core)

# Query test executable
add_executable(query_test query_test.cpp)
target_link_libraries(query_test query core)

# Parser test executable
add_executable(parser_test parser_test.cpp)
target_link_libraries(parser_test query core)

# Planner test executable
add_executable(planner_test planner_test.cpp)
target_link_libraries(planner_test query core)

# Optimizer test executable
add_executable(optimizer_test optimizer_test.cpp)
target_link_libraries(optimizer_test query core)

# Execution engine test executable
add_executable(execution_engine_test execution_engine_test.cpp)
target_link_libraries(execution_engine_test query core transaction)

# Simple QueryProcessor execution test
add_executable(simple_execution_test simple_execution_test.cpp)
target_link_libraries(simple_execution_test query core transaction)

# Simple QueryProcessor compilation test
add_executable(simple_query_test simple_query_test.cpp)
target_link_libraries(simple_query_test query core)

# Simple parser and planner compilation test
add_executable(simple_parser_test simple_parser_test.cpp)
target_link_libraries(simple_parser_test query core)

# Simple optimizer compilation test
add_executable(simple_optimizer_test simple_optimizer_test.cpp)
target_link_libraries(simple_optimizer_test query core)

# INSERT statement test
add_executable(insert_test insert_test.cpp)
target_link_libraries(insert_test query core)

# INSERT plan test
add_executable(insert_plan_test insert_plan_test.cpp)
target_link_libraries(insert_plan_test query core)

# INSERT execution test
add_executable(insert_execution_test insert_execution_test.cpp)
target_link_libraries(insert_execution_test query core transaction)

# UPDATE statement test
add_executable(update_test update_test.cpp)
target_link_libraries(update_test query core)

# UPDATE plan test
add_executable(update_plan_test update_plan_test.cpp)
target_link_libraries(update_plan_test query core)

# UPDATE execution test
add_executable(update_execution_test update_execution_test.cpp)
target_link_libraries(update_execution_test query core transaction)

# DELETE statement test
add_executable(delete_test delete_test.cpp)
target_link_libraries(delete_test query core)

# DELETE plan test
add_executable(delete_plan_test delete_plan_test.cpp)
target_link_libraries(delete_plan_test query core)

# DELETE execution test
add_executable(delete_execution_test delete_execution_test.cpp)
target_link_libraries(delete_execution_test query core transaction)

# Simple DELETE test
add_executable(delete_simple_test delete_simple_test.cpp)
target_link_libraries(delete_simple_test query core transaction)

# JOIN test
add_executable(join_test join_test.cpp)
target_link_libraries(join_test query core)

# JOIN plan test
add_executable(join_plan_test join_plan_test.cpp)
target_link_libraries(join_plan_test query core)

# JOIN execution test
add_executable(join_execution_test join_execution_test.cpp)
target_link_libraries(join_execution_test query core transaction)

# JOIN comprehensive test
add_executable(join_comprehensive_test join_comprehensive_test.cpp)
target_link_libraries(join_comprehensive_test query core transaction)

# Optimizer enhancement test
add_executable(optimizer_enhancement_test optimizer_enhancement_test.cpp)
target_link_libraries(optimizer_enhancement_test query core transaction)