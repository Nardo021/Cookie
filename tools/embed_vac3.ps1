param(
    [Parameter(Mandatory = $true)][string]$InputDll,
    [Parameter(Mandatory = $true)][string]$OutputHeader
)

if (-not (Test-Path -LiteralPath $InputDll)) {
    Write-Error "Missing VAC inhibitor DLL: $InputDll"
    exit 1
}

$bytes = [System.IO.File]::ReadAllBytes($InputDll)
$lines = New-Object System.Collections.Generic.List[string]
$lines.Add('#pragma once')
$lines.Add('')
$lines.Add('#include <cstdint>')
$lines.Add('using BYTE = uint8_t;')
$lines.Add('')
$lines.Add("// Auto-generated from Vac3Inhibitor build. Do not edit.")
$lines.Add("constexpr BYTE vac3_data[$($bytes.Length)] = {")

for ($i = 0; $i -lt $bytes.Length; $i += 12) {
    $chunk = $bytes[$i..([Math]::Min($i + 11, $bytes.Length - 1))]
    $hex = ($chunk | ForEach-Object { '0x{0:X2}' -f $_ }) -join ', '
    if ($i + 12 -lt $bytes.Length) { $hex += ',' }
    $lines.Add("`t$hex")
}

$lines.Add('};')
$lines.Add('')

$content = ($lines -join "`r`n") + "`r`n"
[System.IO.File]::WriteAllText($OutputHeader, $content, [System.Text.UTF8Encoding]::new($false))
Write-Host "Embedded $($bytes.Length) bytes -> $OutputHeader"
