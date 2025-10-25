param()

Write-Host 'Verifying vcpkg ports directory...'
if (Test-Path 'vcpkg\ports') {
    Write-Host 'vcpkg\ports directory exists'
    $portCount = (Get-ChildItem 'vcpkg\ports' -Directory).Count
    Write-Host "Found $portCount port directories"

    # Check for specific Qt6 ports we need
    $qt6Declarative = Test-Path 'vcpkg\ports\qt6-declarative'
    $qt6QuickControls = Test-Path 'vcpkg\ports\qt6-quickcontrols2'

    if ($qt6Declarative) {
        Write-Host 'qt6-declarative port found'
    } else {
        Write-Host 'WARNING: qt6-declarative port NOT found'
    }

    if ($qt6QuickControls) {
        Write-Host 'qt6-quickcontrols2 port found'
    } else {
        Write-Host 'WARNING: qt6-quickcontrols2 port NOT found'
    }
} else {
    Write-Host 'ERROR: vcpkg\ports directory does not exist!'
    exit 1
}

exit 0
