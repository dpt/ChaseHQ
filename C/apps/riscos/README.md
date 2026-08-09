# Native RISC OS application

The repository also builds a self-contained 32-bit `!ChaseHQ` desktop
application. It runs one 48K or 128K game in a Wimp window or in single-tasking
fullscreen mode. This first native release is deliberately silent; AY and beeper
audio are deferred.

The target requires a 32-bit RISC OS system with the Wimp, ColourTrans and
SpriteExtend. A 26-bit system is not supported. To build it, use a RISC OS
cross-build environment containing AMU, Norcroft C, RISC_OSLib and
`riscos-build-run`, then run these commands from the repository root:

```sh
riscos-amu -f C/apps/riscos/Makefile test
riscos-amu -f C/apps/riscos/Makefile install INSTDIR=install
```

The test should print `ChaseHQ native tests passed`. The install command creates
`install/!ChaseHQ`; copy that application directory to the desired RISC OS filer
location, or run it from there. Generated `o32`, `aif32`, `install` and
`.rotransform` content is disposable and must not be committed.

An experimental 64-bit build is also available for local testing. It avoids
RISC_OSLib and the Wimp application shell, starts a silent 128K game directly in
single-tasking fullscreen mode, and exits back to the saved desktop when Escape
is pressed:

```sh
riscos64-amu -f C/apps/riscos/Makefile install INSTDIR=install64
```

This creates `install64/!ChaseHQ`. The fullscreen host has been exercised on a
real 64-bit system, but remains deliberately outside CI.

Select the iconbar icon to open the game. The native host shortcuts are:

| Key         | Action                               |
| ----------- | ------------------------------------ |
| `F2` / `F3` | Start a new 128K / 48K game          |
| `F5`        | Pause or resume                      |
| `F6`        | Restart the selected machine version |
| `F7`–`F10`  | Select window scale 1x–4x            |
| `F11`       | Enter fullscreen mode                |
| `Escape`    | Leave fullscreen mode                |
| `Ctrl-Q`    | Quit the application                 |

Gameplay keys are accepted only while the game window has the input caret. The
original Spectrum keyboard is available, and the arrow keys emulate a Kempston
joystick with keypad Enter as fire. Closing the game window stops that game but
leaves the iconbar application running.

Fullscreen mode prefers a suitable 4-bpp mode, then falls back to 8-bpp or
32-bpp output. Set `ChaseHQ$ScreenMode` to a RISC OS mode number or mode string
to override it, for example `X800 Y600 C16`. Only Escape is handled as a host
shortcut while fullscreen because the application deliberately does not poll the
Wimp in that mode.

## Contents

- `Makefile,fe1` — AMU makefile (root-relative paths; run from the repository
  root)
- `Riscos.c`, `Host.c`/`Host.h`, `Fullscreen.c`, `Tests.c` — native host sources
- `Resources/` — `!ChaseHQ` application resources (`!Boot`, `!Run`, sprites,
  templates)
- `VersionNum` — vmanage-maintained version stamp
- `make-riscos-layout` — builds the generated leaf-name view AMU compiles from
- `make-riscos-icons` — regenerates the iconbar sprites from `chq-ico.png`
- `.robuild.yaml` — build script for https://build.riscos.online/
