# Native RISC OS ChaseHQ port

Starting revision: `f7c20a2` (`develop`). Development takes place on
`feature/riscos-port` as a sequence of reviewable functional commits.

Generated object directories (`o32`, `aif32`, installed bundles, and similar)
and all `.rotransform` content are build products and must never be committed.

## Build and assumptions

The supported desktop target is 32-bit RISC OS using AMU and RISC_OSLib. An
experimental 64-bit, fullscreen-only host is also available without
RISC_OSLib. The canonical CMake/SDL3 source layout remains intact, although
that build is not a release acceptance requirement. One game may run at a
time. The first release is silent; native AY and beeper audio, save games,
networking, CRT effects, 26-bit builds, a 64-bit Wimp host, and multiple
instances are deferred.

Build and test with:

    riscos-amu
    riscos-amu test
    riscos-amu install INSTDIR=install
    riscos64-amu install INSTDIR=install64

## Staged checklist and acceptance criteria

### 1. Port plan

- [x] Record stages, acceptance criteria, commands, assumptions, deferred work,
  the starting revision, commit boundaries, and generated-file policy.

### 2. Native build

- [x] Add a `CApp` AMU build with an explicit source manifest and RISC_OSLib.
- [x] Generate an ignored `.rotransform/riscos` filename-layout mirror.
- [x] Build shared engine/data/Spectrum code and the native host only; exclude
  SDL, the CRT shader, and `slopay-chip.c`.
- [x] Apply only Norcroft-required C89 and fixed-width portability changes.
- [x] Provide native tests for data sizes and modular clock arithmetic.

### 3. Desktop application shell

- [x] Package `!Boot`, `!Run`, sprites, Messages, Help, and Templates.
- [x] Initialise one Wimp task, a right-side iconbar icon, game and Info windows.
- [x] Put Info first and Quit last in the iconbar menu and unwind errors cleanly.
- [x] Size WimpSlot at least 32K above the completed `!RunImage`.

### 4. Windowed graphics

- [x] Describe claimed frames with pixels, format, dimensions, and byte stride.
- [x] Preserve ARGB/ABGR output and expose packed indexed 4-bpp output.
- [x] Update and plot a 256 by 192 16-colour sprite through `OS_SpriteOp`.
- [x] Honour Wimp clipping, rebuild ColourTrans tables after display changes,
  centre the image, shade sizes that do not fit, and scale 1x through 4x.
- [x] Test indexed conversion and stride.

### 5. Cooperative runtime

- [x] Keep the blocking `chq_start()` lifecycle and avoid threads.
- [x] Pace with an absolute monotonic deadline, fixed-point T-state remainder,
  wrap-safe clock arithmetic, and the existing four-frame lag cap.
- [x] Service Wimp events with `Wimp_PollIdle`, including accumulated short
  speech delays, and defer unsafe actions to callback boundaries.
- [x] Use `chq_stop()` for clean unwinding and null audio callbacks.
- [x] Test deferred host-action dispatch.

### 6. Native controls

- [x] Poll held keys through `OS_Byte IN_KEY` and populate ZX/Kempston input.
- [x] Limit gameplay input to the game window while preserving host shortcuts.
- [x] Provide pause, restart, scale, fullscreen, and 48K/128K new-game actions;
  default to 128K.
- [x] Closing the window stops only the game; application Quit exits the task.
- [x] Test key mapping.

### 7. Fullscreen

- [x] Defer entry/exit, preserve the full desktop mode selector and window state,
  and use `ChaseHQ$ScreenMode` or the closest suitable 4-bpp mode.
- [x] Centre at the largest integer scale and use documented sprite, palette,
  mode, and cursor interfaces without direct framebuffer access.
- [x] Escape restores the desktop; paused redraw and all error exits restore
  mode, palette, cursors, and window state.
- [x] Document that fullscreen is deliberately single-tasking.

### 8. Packaging and documentation

- [x] Complete Help, controls, build/install instructions, requirements, and the
  silent-first-release limitation.
- [x] Verify installation and final WimpSlot sizing.
- [x] Complete every first-release item while retaining audio as deferred work.

### 9. Experimental 64-bit fullscreen host

- [x] Select `FULLSCREEN_ONLY` automatically for `BUILD64` and exclude
  RISC_OSLib and the Wimp host from that build.
- [x] Start silent 128K play directly, poll controls with documented OS SWIs,
  and use Escape to stop at a safe engine callback.
- [x] Preserve and restore the desktop mode and cursor state on normal and
  error exits.
- [x] Prefer 4-bpp output, fall back through 8-bpp and 32-bpp modes, and use a
  ColourTrans pixel translation table for every selected mode.
- [x] Build and install with `riscos64-amu install INSTDIR=install64`; keep the
  experimental target outside CI and verify it on a real 64-bit system.

Each numbered section is completed by its correspondingly numbered functional
commit. Before each commit, files are inspected and staged explicitly.
