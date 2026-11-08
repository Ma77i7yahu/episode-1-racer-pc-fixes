@echo off
rem Rebuilds PatchData.cs from ..\stages (make_stages.ps1 makes them), then the installer.
rem Usage: build.cmd ["path to the stock Sounds.map"]
cd /d "%~dp0"
set SOUNDS=%~1
if "%SOUNDS%"=="" set SOUNDS=G:\Game Files\LucasArts\Star Wars Episode I Racer\data\Sounds.map.before-split
G:\msys64\mingw64\bin\python.exe -I gen.py ..\stages "%SOUNDS%" PatchData.cs || exit /b 1
if not exist ..\dist mkdir ..\dist
C:\Windows\Microsoft.NET\Framework64\v4.0.30319\csc.exe /nologo /target:winexe /optimize /out:..\dist\RacerModInstaller.exe /r:System.Windows.Forms.dll /r:System.Drawing.dll Installer.cs PatchData.cs RockFix.cs || exit /b 1
echo Built ..\dist\RacerModInstaller.exe
