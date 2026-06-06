$targets = @(
    "$env:USERPROFILE\Downloads",
    "$env:USERPROFILE\Desktop",
    "$env:USERPROFILE\Documents",
    "$env:LOCALAPPDATA",
    "$env:APPDATA",
    "$env:TEMP",
    "C:\Program Files",
    "C:\Program Files (x86)"
)

Write-Output "=== Disk space by location ==="
foreach ($dir in $targets) {
    if (Test-Path $dir) {
        try {
            $size = (Get-ChildItem -Path $dir -Recurse -ErrorAction SilentlyContinue | Measure-Object -Property Length -Sum).Sum
            $sizeGB = [math]::Round($size / 1GB, 2)
            Write-Output "$sizeGB GB`t$dir"
        } catch {
            Write-Output "??? GB`t$dir (access denied)"
        }
    }
}

Write-Output "`n=== Top 20 folders in C:\Users\罗亮 ==="
Get-ChildItem -Path "$env:USERPROFILE" -Directory -ErrorAction SilentlyContinue |
    ForEach-Object {
        $size = (Get-ChildItem -Path $_.FullName -Recurse -ErrorAction SilentlyContinue | Measure-Object -Property Length -Sum).Sum
        [PSCustomObject]@{Path=$_.FullName; SizeGB=[math]::Round($size/1GB, 2)}
    } |
    Sort-Object SizeGB -Descending |
    Select-Object -First 20 |
    Format-Table -AutoSize
