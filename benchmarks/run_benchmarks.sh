#!/bin/bash
# Script to run all PhantomDB benchmarks

echo "Building PhantomDB with benchmarks enabled..."

# Create build directory
mkdir -p build_benchmarks
cd build_benchmarks

# Configure with benchmarks enabled
cmake -DBENCHMARKS=ON ..

# Build benchmarks
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

if [ $? -ne 0 ]; then
    echo "Error: Build failed"
    cd ..
    exit 1
fi

echo "Running benchmarks..."

# Create results directory
mkdir -p ../benchmark_results
timestamp=$(date +"%Y%m%d_%H%M%S")
results_dir="../benchmark_results/benchmark_$timestamp"

mkdir -p $results_dir

# Run all benchmarks
echo "Running core benchmarks..."
./benchmarks/core_benchmarks > $results_dir/core_benchmarks.txt 2>&1

echo "Running storage benchmarks..."
./benchmarks/storage_benchmarks > $results_dir/storage_benchmarks.txt 2>&1

echo "Running query benchmarks..."
./benchmarks/query_benchmarks > $results_dir/query_benchmarks.txt 2>&1

echo "Running transaction benchmarks..."
./benchmarks/transaction_benchmarks > $results_dir/transaction_benchmarks.txt 2>&1

echo "Running distributed benchmarks..."
./benchmarks/distributed_benchmarks > $results_dir/distributed_benchmarks.txt 2>&1

# Run all benchmarks together
echo "Running all benchmarks..."
./benchmarks/run_all_benchmarks > $results_dir/all_benchmarks.txt 2>&1

cd ..

echo ""
echo "Benchmark results saved to $results_dir"
echo "Summary of results:"
echo "-------------------"
for file in $results_dir/*.txt; do
    echo "=== $(basename $file) ==="
    grep -A 5 "Benchmark:" $file | head -20
    echo ""
done