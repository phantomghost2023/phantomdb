# Benchmark module CMakeLists.txt

# Option to enable benchmarks
option(BENCHMARKS "Enable benchmark suite" OFF)

if(BENCHMARKS)
    # Find required packages
    find_package(Threads REQUIRED)
    
    # Benchmark framework
    set(BENCHMARK_SOURCES
        benchmark_runner.cpp
        benchmark_utils.cpp
    )
    
    add_library(benchmark_framework ${BENCHMARK_SOURCES})
    target_link_libraries(benchmark_framework ${CMAKE_THREAD_LIBS_INIT})
    target_include_directories(benchmark_framework PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src
    )
    
    # Core benchmarks
    add_executable(core_benchmarks core_benchmarks.cpp)
    target_link_libraries(core_benchmarks benchmark_framework core)
    
    # Storage benchmarks
    add_executable(storage_benchmarks storage_benchmarks.cpp)
    target_link_libraries(storage_benchmarks benchmark_framework core storage)
    
    # Query benchmarks
    add_executable(query_benchmarks query_benchmarks.cpp)
    target_link_libraries(query_benchmarks benchmark_framework core query)
    
    # Transaction benchmarks
    add_executable(transaction_benchmarks transaction_benchmarks.cpp)
    target_link_libraries(transaction_benchmarks benchmark_framework core transaction)
    
    # Distributed benchmarks
    add_executable(distributed_benchmarks distributed_benchmarks.cpp)
    target_link_libraries(distributed_benchmarks benchmark_framework core distributed)
    
    # All benchmarks runner
    add_executable(run_all_benchmarks run_all_benchmarks.cpp)
    target_link_libraries(run_all_benchmarks 
        benchmark_framework 
        core 
        storage 
        query 
        transaction 
        distributed
    )
endif()