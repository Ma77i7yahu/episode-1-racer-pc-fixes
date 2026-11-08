# Star Wars Episode I Racer (PC) – Mod Installer

Optional fixes and improvements for the **GOG** release of **Star Wars Episode I Racer**: tougher AI opponents, two-player split screen, Jinn Reeso and Cy Yunga as selectable racers, bigger and smoother movies, the boost start kept when you skip the intro, no damage from other podracers, the Executioner tunnel rocks fixed, upgrade parts placed correctly on Inspect Vehicle, and more. Each option can be turned on or off on its own, and the installer keeps your original files so you can change or undo anything later.

**This is a patch only.** No game files are included. You need your own copy of the game.

## Download

Get `RacerMods-v1.0.zip` from the [**Releases**](https://github.com/Ma77i7yahu/episode-1-racer-pc-fixes/releases/latest) page. It holds the installer and its README. `SHA256SUMS.txt` on the release page lets you check your download.

Full instructions, requirements and troubleshooting: [docs/README-installer.md](docs/README-installer.md).

## The options

| Option | Status |
|---|---|
| Tougher AI opponents + "AI Patch" menu row | Tested |
| Other podracers don't damage your pod | Tested boosting into a pod and hitting a wall; being rammed from behind not yet tested |
| Skipping the pre-race intro keeps your boost start | Not yet confirmed in game |
| Jinn Reeso and Cy Yunga as racers 24 and 25 | Tested (this public build adds empty portrait frames for them) |
| Executioner tunnel rocks disappear when hit | Played on the stock file; byte-checked on SWE1R HD Redraw's |
| Two-player split screen (+ optional Free Play track records) | Tested |
| Bigger, smoother movies | Tested |
| Upgrade parts sit in their boxes on Inspect Vehicle | Tested at 1920 x 1080 |
| Large Address Aware | Needed by SWE1R HD Redraw |
| Enter no longer counts as Thrust | Tested |

The installer needs GOG's `SWEP1RCR.EXE` (872,448 bytes, MD5 `E1FCF50C8DE2DBEF70E6AD8E09371322`), with or without the Large Address Aware flag that SWE1R HD Redraw sets. **Always run the game as administrator**: without it the game reports a bad install and can overwrite your saved records.

## What's in this repo

- `docs/README-installer.md`: the README that comes with the installer.
- `src/`: the installer's C# source and the patch tools. See [BUILDING.md](BUILDING.md).

## Licence

The code, patches and documentation here are under the [MIT licence](LICENSE): reuse or port them freely, with credit. The game itself isn't covered.

## Credits

Research on the game's code was helped by the [SW_RACER_RE](https://github.com/tim-tim707/SW_RACER_RE) project. Thanks also to the makers of SWE1R HD Redraw, which these mods are built to work alongside.

## Disclaimer

Unofficial fan-made mods, not affiliated with or endorsed by Lucasfilm, LucasArts, Disney or GOG. Star Wars is a trademark of its owners. No game files are distributed here. Use at your own risk, and consider backing up your `data\player` and `save` folders first.
