#!/usr/bin/env python3
"""
PhantomDB Benchmark Visualizer

This script generates visual charts from benchmark results.
"""

import json
import argparse
import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path
import re

def parse_benchmark_output(file_path):
    """
    Parse benchmark output file and extract results.
    """
    results = []
    
    with open(file_path, 'r') as f:
        content = f.read()
    
    # Extract benchmark results using regex
    benchmark_pattern = r'Benchmark: ([^\n]+)\s+Duration: ([0-9.]+) ms\s+Iterations: ([0-9]+)\s+Throughput: ([0-9.]+) ops/sec'
    matches = re.findall(benchmark_pattern, content)
    
    for match in matches:
        name, duration, iterations, throughput = match
        results.append({
            'name': name.strip(),
            'duration_ms': float(duration),
            'iterations': int(iterations),
            'throughput_ops_per_sec': float(throughput)
        })
    
    return results

def create_throughput_chart(results, output_file):
    """
    Create a bar chart showing throughput for each benchmark.
    """
    names = [r['name'] for r in results]
    throughput = [r['throughput_ops_per_sec'] for r in results]
    
    plt.figure(figsize=(12, 8))
    bars = plt.bar(range(len(names)), throughput, color='skyblue')
    plt.xlabel('Benchmark')
    plt.ylabel('Throughput (ops/sec)')
    plt.title('PhantomDB Benchmark Throughput')
    plt.xticks(range(len(names)), names, rotation=45, ha='right')
    
    # Add value labels on bars
    for i, (bar, value) in enumerate(zip(bars, throughput)):
        plt.text(bar.get_x() + bar.get_width()/2, bar.get_height() + max(throughput)*0.01,
                f'{value:.0f}', ha='center', va='bottom')
    
    plt.tight_layout()
    plt.savefig(output_file, dpi=300, bbox_inches='tight')
    plt.close()

def create_duration_chart(results, output_file):
    """
    Create a bar chart showing duration for each benchmark.
    """
    names = [r['name'] for r in results]
    durations = [r['duration_ms'] for r in results]
    
    plt.figure(figsize=(12, 8))
    bars = plt.bar(range(len(names)), durations, color='lightcoral')
    plt.xlabel('Benchmark')
    plt.ylabel('Duration (ms)')
    plt.title('PhantomDB Benchmark Duration')
    plt.xticks(range(len(names)), names, rotation=45, ha='right')
    
    # Add value labels on bars
    for i, (bar, value) in enumerate(zip(bars, durations)):
        plt.text(bar.get_x() + bar.get_width()/2, bar.get_height() + max(durations)*0.01,
                f'{value:.2f}', ha='center', va='bottom')
    
    plt.tight_layout()
    plt.savefig(output_file, dpi=300, bbox_inches='tight')
    plt.close()

def generate_summary_report(results, output_file):
    """
    Generate a summary report in markdown format.
    """
    with open(output_file, 'w') as f:
        f.write("# PhantomDB Benchmark Summary Report\n\n")
        
        f.write("## Benchmark Results\n\n")
        f.write("| Benchmark Name | Duration (ms) | Iterations | Throughput (ops/sec) |\n")
        f.write("|----------------|---------------|------------|---------------------|\n")
        
        for result in results:
            f.write(f"| {result['name']} | {result['duration_ms']:.2f} | {result['iterations']} | {result['throughput_ops_per_sec']:.0f} |\n")
        
        f.write("\n## Performance Summary\n\n")
        
        if results:
            throughputs = [r['throughput_ops_per_sec'] for r in results]
            durations = [r['duration_ms'] for r in results]
            
            f.write(f"- **Fastest Benchmark**: {max(results, key=lambda x: x['throughput_ops_per_sec'])['name']} ({max(throughputs):.0f} ops/sec)\n")
            f.write(f"- **Slowest Benchmark**: {min(results, key=lambda x: x['throughput_ops_per_sec'])['name']} ({min(throughputs):.0f} ops/sec)\n")
            f.write(f"- **Average Throughput**: {sum(throughputs) / len(throughputs):.0f} ops/sec\n")
            f.write(f"- **Average Duration**: {sum(durations) / len(durations):.2f} ms\n")

def main():
    parser = argparse.ArgumentParser(description='PhantomDB Benchmark Visualizer')
    parser.add_argument('input_dir', help='Directory containing benchmark result files')
    parser.add_argument('-o', '--output', default='benchmark_report', help='Output directory for charts and report')
    
    args = parser.parse_args()
    
    input_dir = Path(args.input_dir)
    output_dir = Path(args.output)
    output_dir.mkdir(exist_ok=True)
    
    # Parse all benchmark files
    all_results = []
    
    for file_path in input_dir.glob('*.txt'):
        print(f"Parsing {file_path}...")
        results = parse_benchmark_output(file_path)
        all_results.extend(results)
    
    if not all_results:
        print("No benchmark results found!")
        return
    
    # Generate charts
    print("Generating throughput chart...")
    create_throughput_chart(all_results, output_dir / 'throughput_chart.png')
    
    print("Generating duration chart...")
    create_duration_chart(all_results, output_dir / 'duration_chart.png')
    
    print("Generating summary report...")
    generate_summary_report(all_results, output_dir / 'summary_report.md')
    
    print(f"Visualization complete! Results saved to {output_dir}")

if __name__ == '__main__':
    main()