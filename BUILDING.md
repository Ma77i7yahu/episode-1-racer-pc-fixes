# Building

You don't need any of this to install the mods. It's for anyone who wants to rebuild the installer or port the patches.

## Installer only

`src/PatchData.cs` already holds every patch, so the installer builds with the C# compiler that ships with Windows (.NET Framework 4):

```
cd src
C:\Windows\Microsoft.NET\Framework64\v4.0.30319\csc.exe /nologo /target:winexe /optimize /define:PUBLIC /out:RacerModInstaller.exe /r:System.Windows.Forms.dll /r:System.Drawing.dll Installer.cs PatchData.cs RockFix.cs
```

`/define:PUBLIC` only changes the description of the Jinn and Cy option: this repo's `PatchData.cs` holds two empty portrait frames for them instead of portrait art.

## Regenerating the patches

`src/make_stages.ps1` replays each mod's patch script on the stock exe, one after another, into a `stages` folder, and `src/gen.py` diffs consecutive stages into `PatchData.cs`, stopping if two mods change the same byte. The patch scripts and the stages are modified game files or embed game data, so they aren't in this repo; `PatchData.cs` is the reference. `build.cmd` expects Python from MSYS2's MinGW-w64 at `G:\msys64`, so change that path to suit your machine.

`src/moviescale/` is the source of the movie scaler that the "Bigger, smoother movies" option adds to the exe (built with MSYS2's i686 GCC by `build_moviescale.cmd`; `test_moviescale.c` checks it on sample frames).

## Tests

With all options on, the installer's output was checked to be byte for byte the exe that was played and tested; the single options were checked against each mod's own patch script run on the stock exe, and install, change and restore were round-tripped on copies of the game files.
