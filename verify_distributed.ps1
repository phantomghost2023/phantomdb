# PowerShell script to verify distributed components
Write-Host "Verifying PhantomDB Distributed Components..." -ForegroundColor Green

# Compile the verification test
Write-Host "Compiling verification test..." -ForegroundColor Yellow
g++ -std=c++17 -I. -o verify_distributed.exe verify_distributed_components.cpp

if ($LASTEXITCODE -eq 0) {
    Write-Host "✓ Compilation successful" -ForegroundColor Green
    
    # Run the verification test
    Write-Host "Running verification test..." -ForegroundColor Yellow
    .\verify_distributed.exe
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host "✓ All distributed components verified successfully!" -ForegroundColor Green
    } else {
        Write-Host "✗ Verification test failed" -ForegroundColor Red
        exit 1
    }
} else {
    Write-Host "✗ Compilation failed" -ForegroundColor Red
    exit 1
}

# Clean up
Remove-Item verify_distributed.exe -ErrorAction SilentlyContinue