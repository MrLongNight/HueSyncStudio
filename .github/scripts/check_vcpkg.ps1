param()

Write-Host 'Verifying vcpkg ports directory...'
if (Test-Path 'vcpkg\ports') {
    Write-Host 'vcpkg\ports directory exists'
    $portCount = (Get-ChildItem 'vcpkg\ports' -Directory).Count
    Write-Host "Found $portCount port directories"

    # Check for specific Qt6 ports we need
    $qtDeclarative = Test-Path 'vcpkg\ports\qtdeclarative'
    $qtQuickControls2 = Test-Path 'vcpkg\ports\qtquickcontrols2'

    if ($qtDeclarative) {
        Write-Host 'qtdeclarative port found'
    } else {
        Write-Host 'WARNING: qtdeclarative port NOT found'
    }

    if ($qtQuickControls2) {
        Write-Host 'qtquickcontrols2 port found'
    } else {
        Write-Host 'WARNING: qtquickcontrols2 port NOT found'
    }
} else {
    Write-Host 'ERROR: vcpkg\ports directory does not exist!'
    exit 1
}

exit 0
