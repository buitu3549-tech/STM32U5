$paths = @(
    'C:\Users\罗亮\AppData\Local\JianyingPro',
    'C:\Users\罗亮\AppData\Roaming\.minecraft',
    'C:\Users\罗亮\AppData\Local\Temp'
)
foreach ($p in $paths) {
    if (Test-Path $p) {
        $size = (Get-ChildItem -Path $p -Recurse -ErrorAction SilentlyContinue | Measure-Object -Property Length -Sum).Sum
        $gb = [math]::Round($size/1GB, 2)
        Write-Output "EXISTS: $p - $gb GB"
    } else {
        Write-Output "GONE: $p"
    }
}
