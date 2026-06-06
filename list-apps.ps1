$apps = @()
$paths = @(
    "HKLM:\Software\Microsoft\Windows\CurrentVersion\Uninstall\*",
    "HKLM:\Software\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall\*",
    "HKCU:\Software\Microsoft\Windows\CurrentVersion\Uninstall\*"
)
foreach ($path in $paths) {
    $items = Get-ItemProperty $path -ErrorAction SilentlyContinue
    foreach ($item in $items) {
        if ($item.DisplayName -and $item.DisplayName -notmatch 'Driver|Runtime|Redist|Update for|Service Pack|Hotfix|Security Update') {
            $sizeMB = if ($item.EstimatedSize) { [math]::Round($item.EstimatedSize / 1024, 1) } else { 0 }
            $apps += [PSCustomObject]@{
                Name = $item.DisplayName
                SizeMB = $sizeMB
                Date = $item.InstallDate
            }
        }
    }
}
$apps | Sort-Object SizeMB -Descending | Format-Table -AutoSize
