Write-Output "=== Clearing Temp ==="
$tempSize = (Get-ChildItem -Path $env:TEMP -Recurse -ErrorAction SilentlyContinue | Measure-Object -Property Length -Sum).Sum
Write-Output ("Temp before: " + [math]::Round($tempSize/1GB, 2) + " GB")
Get-ChildItem -Path $env:TEMP -Recurse -Force -ErrorAction SilentlyContinue | Remove-Item -Recurse -Force -ErrorAction SilentlyContinue
$tempAfter = (Get-ChildItem -Path $env:TEMP -Recurse -ErrorAction SilentlyContinue | Measure-Object -Property Length -Sum).Sum
Write-Output ("Temp after: " + [math]::Round($tempAfter/1GB, 2) + " GB")
Write-Output "Temp cleared!"

Write-Output "`n=== Deleting JianyingPro cache ==="
$jp = "$env:LOCALAPPDATA\JianyingPro"
if (Test-Path $jp) {
    $size = (Get-ChildItem -Path $jp -Recurse -ErrorAction SilentlyContinue | Measure-Object -Property Length -Sum).Sum
    Write-Output ("JianyingPro: " + [math]::Round($size/1GB, 2) + " GB - deleting...")
    Remove-Item -Path $jp -Recurse -Force -ErrorAction SilentlyContinue
    Write-Output "JianyingPro deleted."
} else { Write-Output "JianyingPro not found" }

Write-Output "`n=== Deleting .minecraft ==="
$mc = "$env:APPDATA\.minecraft"
if (Test-Path $mc) {
    $size = (Get-ChildItem -Path $mc -Recurse -ErrorAction SilentlyContinue | Measure-Object -Property Length -Sum).Sum
    Write-Output (".minecraft: " + [math]::Round($size/1GB, 2) + " GB - deleting...")
    Remove-Item -Path $mc -Recurse -Force -ErrorAction SilentlyContinue
    Write-Output ".minecraft deleted."
} else { Write-Output ".minecraft not found" }

Write-Output "`nDone!"
