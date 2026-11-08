<#
  Rebuilds ..\stages\*.exe by replaying the game folder's patch scripts, in the order they were
  applied to the real game, on a scratch copy that starts from the stock GOG exe.
  Each stage is the exe after one more mod; gen.py turns the differences into PatchData.cs.

  Usage: make_stages.ps1 [-GameDir <Racer folder>] [-Work <empty scratch folder>]
#>
param(
    [string]$GameDir = 'G:\Game Files\LucasArts\Star Wars Episode I Racer',
    [string]$Work = (Join-Path $env:TEMP 'racer-installer-stages')
)
$ErrorActionPreference = 'Stop'
$stages = Join-Path $PSScriptRoot '..\stages'
if (Test-Path $Work) { Remove-Item $Work -Recurse -Force -Confirm:$false }
New-Item -ItemType Directory -Force "$Work\data\lev01", "$Work\data\config\current", $stages | Out-Null

$stock = Join-Path $GameDir 'SWEP1RCR.EXE.original'
if ((Get-FileHash $stock -Algorithm MD5).Hash -ne 'E1FCF50C8DE2DBEF70E6AD8E09371322') { throw 'SWEP1RCR.EXE.original is not the stock GOG exe.' }
Copy-Item $stock "$Work\SWEP1RCR.EXE"
Copy-Item (Join-Path $GameDir 'data\Sounds.map.before-split') "$Work\data\Sounds.map"
Copy-Item (Join-Path $GameDir 'data\lev01\out_spriteblock.bin.original') "$Work\data\lev01\out_spriteblock.bin"
foreach ($s in 'AI-Difficulty-Patch.ps1', 'Bigger-Movies-Patch.ps1', 'Split-Screen-Patch.ps1', 'Boost-Start-Fix.ps1',
               'Pod-Collision-Damage-Fix.ps1', 'Hangar-Parts-Fix.ps1', 'Extra-Racers-Patch.ps1') {
    Copy-Item (Join-Path $GameDir $s) $Work
}
$exe = "$Work\SWEP1RCR.EXE"
Copy-Item $exe "$stages\0-stock.exe"

function Step([string]$name, [string]$script) {
    & powershell -NoProfile -ExecutionPolicy Bypass -File (Join-Path $Work $script) | Out-Host
    if ($LASTEXITCODE -ne 0) { throw "$script failed." }
    Copy-Item $exe "$stages\$name.exe"
}
Step '1-ai' 'AI-Difficulty-Patch.ps1'
Step '2-movies' 'Bigger-Movies-Patch.ps1'
Step '3-split' 'Split-Screen-Patch.ps1'
Step '4-boost' 'Boost-Start-Fix.ps1'
Step '5-pod' 'Pod-Collision-Damage-Fix.ps1'
$b = [IO.File]::ReadAllBytes($exe); $pe = [BitConverter]::ToInt32($b, 0x3C); $b[$pe + 22] = $b[$pe + 22] -bor 0x20
[IO.File]::WriteAllBytes($exe, $b); Copy-Item $exe "$stages\6-laa.exe"
Step '7-hangar' 'Hangar-Parts-Fix.ps1'
Step '8-extra' 'Extra-Racers-Patch.ps1'
Copy-Item "$Work\data\lev01\out_spriteblock.bin" "$stages\out_spriteblock.bin"
Write-Host ('Final stage MD5 ' + (Get-FileHash $exe -Algorithm MD5).Hash)
