# Pulling apart Chase H.Q. for the ZX Spectrum

Reverse engineering by David Thomas

Project started: March 2023

![Using Spectrum Analyser to investigate the game, including turning the screen green](ChaseHQ.png)

This is the beginnings of a disassembly of the ZX Spectrum version of Chase H.Q. by Ocean Software. Initially this is the 48K version. The 128K version is better but it has a lot more code to consider so it's easier to start off with the 48K version first.

In this repository is a [SkoolKit](https://skoolkit.ca/) control file which you can use to disassemble the game. This works with the game when it is in a "pristine" just-loaded state. See below for how to make a pristine snapshot.

## WHY?

To find out how it works (and maybe one day we could make new levels).

## HOW TO

- Install SkoolKit:

``` sh
pip3 install skoolkit
```

- Clone the t2s repository that can make pristine snapshots from emulator files:

``` sh
git clone https://github.com/skoolkid/t2sfiles/
```

- Download and convert the game into a .z80 format snapshot:

``` sh
tap2sna.py @t2sfiles/t2s/c/chase-hq.t2s
```

You'll see:

```
Downloading https://worldofspectrum.net/pub/sinclair/games/c/ChaseH.Q..tzx.zip
Extracting Chase HQ - Side 1.tzx
Program: CHASE HQ
Fast loading data block: 23755,3870
Data (514 bytes)
Data (6914 bytes)
Data (19074 bytes)
Data (16130 bytes)
Data (4 bytes)
Data (6898 bytes)
Tape finished
Simulation stopped (PC at start address): PC=23372
Writing chase-hq.z80
```

Build a Skool file like so:

``` sh
sna2skool.py --hex -c ChaseHQ.ctl chase-hq.z80 > ChaseHQ.skool  # remove --hex if you want decimal numbers
```

A Skool file is a high-level assembly listing from which we can generate regular assembly listings, or HTML disassemblies. You can edit the Skool file and turn it back into a control file like so:

``` sh
skool2ctl.py --hex ChaseHQ.skool > ChaseHQ.tmp.ctl
```

This makes it easy to pull your changes back into the main control file by copying and pasting modified chunks across. The bigger and more detailed the control file gets, the better our explanation of the game is!

Build an assembly listing like so:

``` sh
skool2asm.py --hex --create-labels ChaseHQ.skool > ChaseHQ.asm
```

Build a tap file for loading into emulators or real Spectrums like so:

``` sh
skool2bin.py ChaseHQ.skool - | bin2tap.py --org 16384 --stack 65530 --start 23372 - ChaseHQ.tap
```

## HOW TO DETERMINE HOW THE GAME WORKS?

- Option (1) is to run the game in the [Spectrum Analyser interactive disassembler](https://colourclash.co.uk/spectrum-analyser/) and look for clues.
- Option (2) is to stare at the code _really hard_ until it makes sense.

You may have to do both.

See https://youtu.be/BGVI0TbePsQ for a short video of me running Spectrum Analyser to find out how the game builds the back buffer up.

## RELATED

Play Chase H.Q. online: http://torinak.com/qaop#!chasehq

Chat: https://matrix.to/#/#The-Great-Escape_Lobby:gitter.im

My previous disassembly project: https://github.com/dpt/The-Great-Escape

Spectrum Computing forums thread: https://spectrumcomputing.co.uk/forums/viewtopic.php?p=113633
