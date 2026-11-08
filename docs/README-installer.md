# Star Wars Episode I Racer (PC, GOG version) – Mod Installer

Optional mods for the **GOG** release of **Star Wars Episode I Racer**. Each can be switched on or off on its own. Most are changes to `SWEP1RCR.EXE`. Three of them also edit a data file, which is listed with each mod below.

## Installing

1. Install the game from GOG and close it.
2. Run `RacerModInstaller.exe`. If it's in the game folder it finds the game on its own. Otherwise click **Browse…** and pick the folder that holds `SWEP1RCR.EXE`.
3. Tick the options you want. Point at any option to see what it does.
4. Click **Install selected**.
5. **Always run the game as administrator.** Run without it, the game reports a bad install and can overwrite your saved records.

On first use the installer saves the GOG exe as `SWEP1RCR.EXE.original`. Every later install rebuilds the exe from that copy, so you can run the installer again at any time to change options. It ticks the options that are installed now. **Restore original game** removes every mod.

The installer accepts only GOG's exe (872,448 bytes, MD5 `E1FCF50C8DE2DBEF70E6AD8E09371322`), with or without the Large Address Aware flag that SWE1R HD Redraw sets, or a folder holding that exe as `SWEP1RCR.EXE.original` (or HD Redraw's `SWEP1RCR.EXE.orig`). Close the game before installing.

## The mods

| Mod | Default | Also changes | Status |
|---|---|---|---|
| Tougher AI opponents + "AI Patch" menu row | On | | Tested |
| Other podracers don't damage your pod | On | | Tested boosting into a pod and hitting a wall; being rammed from behind not yet tested |
| Skipping the pre-race intro keeps your boost start | On | | Not yet confirmed in game |
| Jinn Reeso and Cy Yunga as racers 24 and 25 | On | `data\lev01\out_spriteblock.bin` | Tested |
| Executioner tunnel rocks disappear when hit | On with HD Redraw, otherwise off | `data\lev01\out_modelblock.bin` | Played on the stock file; byte-checked on HD Redraw's |
| Two-player split screen | On | `data\Sounds.map` (4 lines) | Tested |
| Free Play sets track records (split screen option) | Off | | Tested |
| Bigger, smoother movies | On | | Tested |
| Upgrade parts sit in their boxes on Inspect Vehicle | On | | Tested at 1920 x 1080 |
| Large Address Aware | On | | Needed by SWE1R HD Redraw |
| Enter no longer counts as Thrust | Off | `data\config\current\current_control.map` | Tested |

All of these were tested together. Each mod's changes were checked to touch different bytes from every other mod, and with all of them on the installer produces exactly the tested exe. Other combinations are byte-checked but haven't been played.

### Tougher AI opponents
Opponents are faster, by this much above each track's stock AI level:

| Circuit | Increase |
|---|---|
| Amateur | +30% |
| Semi-Pro | +25% |
| Galactic | +24% |
| Invitational | +24% |

The track favourite starts chasing you once you're about 4 seconds ahead (stock: about 10), at x1.4. While it leads it runs at x1.0 instead of x1.06, which leaves room to recover from a crash. AI Speed Slow and Fast are x0.90 and x1.15.

The AI Speed row also appears on the Tournament race screen and now applies there. An **AI Patch** row after it turns all of this off and on from the menu. It goes back to ON each time the game starts.

Also included: on Oovo IV, a pod that respawns clears any breakable asteroid around it, so it can't be trapped inside the one it just hit.

### Other podracers don't damage your pod
When pods touch, the game puts damage on four random engine parts, and above 325 speed it can set your engine on fire. With faster AI these collisions are constant. For local players both are now skipped when another pod hits you. You still get bumped, sparks still fly, and a hit still cancels a boost. Walls and scenery damage you as normal, including when a pod pushes you into them. AI pods and online races are unchanged.

### Skipping the pre-race intro keeps your boost start
The game allows one boost-start attempt per race, and the first new thrust press uses it up. Skipping the intro can count as that press, because the key is still down when the countdown starts. With this mod a thrust input still held from the skip no longer counts as a new press. This works for keys, mouse and controller.

### Jinn Reeso and Cy Yunga as racers 24 and 25
The game has two hidden pilots with their own pods, built for a cheat that swaps them in over Mars Guo and Bullseye. This adds them to the selection screen instead, so all 25 can be picked and nobody is replaced. They're always unlocked. Jinn uses Mars Guo's stats and voice, and Cy uses Bullseye's. Two empty portrait frames are added to `out_spriteblock.bin` for them (no portrait art is included), used on the results and records screens. The 179 original images are unchanged, and the file is backed up as `out_spriteblock.bin.original` first.

**Before removing this mod,** pick a stock pilot for any profile set to Jinn or Cy. Records set by Jinn or Cy will show the wrong racer without it.

### Executioner tunnel rocks disappear when hit
**If you use SWE1R HD Redraw (by Wachhabender), install it before turning this on.** HD Redraw's installer only accepts the untouched model file, and this fix changes it. If you need to reinstall HD Redraw later, untick this option and click **Install selected** first.

In the Executioner tunnel on Oovo IV, three breakable rocks were built into the tunnel walls' model. When you hit one it explodes but stays where it is, still solid, and a rock over 6,000 units away vanishes instead. Each of the three now gets its own part of the model, and its trigger points at it, so they disappear when hit like the track's other rocks. Rock positions, damage, explosions and the other tracks are unchanged.

The fix edits `out_modelblock.bin` in place, so it works on the stock file and on the one HD Redraw builds. Unticking it, or using **Restore original game**, takes it back out exactly. The installer checks this by applying the fix again and comparing.

### Two-player split screen
**Split Screen** is added to the main menu, and single player still works. Player 1 uses the keyboard with your normal keys. Player 2 uses an Xbox controller:

| Control | Action |
|---|---|
| Left stick | Steer and pitch |
| RT | Thrust |
| LT | Brake |
| A | Boost |
| B | Repair |
| X | Taunt |
| Y | Change view |
| LB / RB | Roll |

Choose Split Screen, pick player 1's name and then player 2's, then each player picks a pod. Player 1 chooses the course and starts the race. With Racers set to 1 or 2 it's just the two of you.

Split screen comes with these changes:
- Pods are fully upgraded in Free Play and Split Screen. Tournament keeps the upgrades you've earned.
- AI is a further 25% faster in those modes to match, which includes single-player Free Play.
- The players don't get the two-player catch-up boost.
- Player 2 gets their own engine sounds, set in four lines of `data\Sounds.map` (backed up as `Sounds.map.original`).
- Only Tournament sets track records, unless you tick **Let Free Play races set track records too**. Split Screen never sets them.

Known issues:
- Both pods glow blue when player 1 respawns.
- Racer tags can sit on the line between the two views.
- The wrong-way arrow can show on the other player's view.
- Online play hasn't been tested with split screen installed.

### Bigger, smoother movies
At 640 x 480 and above the game shows each movie 1:1 in the middle of the screen. The movies are now stretched to the full screen width with their shape kept (1920 x 816 at 1080p), and smoothed rather than blocky.

### Upgrade parts sit in their boxes on Inspect Vehicle
On a widescreen display the hangar's 3D view is zoomed, so the part models land outside their boxes. They're moved back to suit the screen shape, with a small per-box nudge tuned at 1920 x 1080. At 4:3 they stay where the game puts them. This is meant for use with a widescreen fix such as SWE1R HD Redraw.

### Large Address Aware
Lets the game use more than 2 GB of memory. SWE1R HD Redraw needs this, and the game crashes without it. When HD Redraw is in the game folder this stays on, and **Restore original game** keeps it on too.

### Enter no longer counts as Thrust
Removes the Enter = Thrust binding from your controls file, so you skip the intro with Escape. Unticking the option puts the binding back. You don't need this with the boost-start fix; it's there if you prefer it.

## SWE1R HD Redraw
HD Redraw is a separate mod by Wachhabender and isn't included. It works alongside these mods: it patches the game in memory at places none of these mods change. Install it on its own, **before** this installer's Executioner rock fix. Its installer replaces `out_modelblock.bin` and `out_textureblock.bin`. This installer never touches the texture file, and it changes the model file only for the rock fix.

## Small print
Research on the game's code was helped by the SW_RACER_RE project: github.com/tim-tim707/SW_RACER_RE

This is an unofficial fan mod, not connected with LucasArts, Lucasfilm, Disney or GOG. It contains no game files. Use it at your own risk, and consider backing up your `data\player` and `save` folders first.

Source code and full patch details: https://github.com/Ma77i7yahu/episode-1-racer-pc-fixes
