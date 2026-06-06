$dirs = @(
    "C:\Program Files (x86)",
    "C:\Users\$env:USERNAME\AppData\Local",
    "C:\Users\$env:USERNAME\AppData\Roaming",
    "C:\Program Files"
)

foreach ($base in $dirs) {
    Write-Output "`n=== $base ==="
    if (Test-Path $base) {
        Get-ChildItem -Path $base -Directory -ErrorAction SilentlyContinue |
            ForEach-Object {
                $size = (Get-ChildItem -Path $_.FullName -Recurse -ErrorAction SilentlyContinue | Measure-Object -Property Length -Sum).Sum
                $sizeGB = [math]::Round($size / 1GB, 2)
                if ($sizeGB -gt 0.1) {
                    [PSCustomObject]@{Folder=$_.Name; SizeGB=$sizeGB}
                }
            } |
            Sort-Object SizeGB -Descending |
            Select-Object -First 15 |
            Format-Table -AutoSize
    }
}
