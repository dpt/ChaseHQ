> $4000 ; demo.skool
> $4000 ;
> $4000 ; This is a SkoolKit control file that disassembles the Sinclair User issue 94
> $4000 ; "Megatape 23" promotional demo of "Chase H.Q." by Ocean Software.
> $4000 ;
> $4000 ; <https://worldofspectrum.org/archive/software/cover-tapes-and-electronic-magazines/sinclair-user-issue-94-megatape-23-sinclair-user>
> $4000 ;
> $4000 ; This is a distinct build from the retail game disassembled by
> $4000 ; ../ChaseHQ-128K.skool: a single 48K (non-banked) memory layout, its own
> $4000 ; "Paul Owens" tape protection loader (handled by SkoolKit's built-in
> $4000 ; accelerator=rom,paul-owens sim-load config), and its own menu system. It
> $4000 ; features a single stage using stage 1's graphics and a custom map, no retry
> $4000 ; on fail.
> $4000 ;
> $4000 ; Reverse engineering by David Thomas <dave@davespace.co.uk>, 2026.
> $4000 ; Sources live at <https://github.com/dpt/ChaseHQ>.
> $4000 ;
> $4000 ;
> $4000 ; MEMORY MAP (pristine, as-loaded state)
> $4000 ; ---------------------------------------
> $4000 ; $4000-$5AFF  Screen memory (bitmap + attributes), loaded by tape block 4
> $4000 ; $5B00-$5BFF  Unloaded gap; corresponds to the retail game's pre-shifted
> $4000 ;              backdrop scratch area at the same address
> $4000 ; $5C00-$EE7F  Main data+code block, loaded by tape block 5 (37504 bytes).
> $4000 ;              Starts at the same address the retail 128K game uses for its
> $4000 ;              own per-stage data - hints at shared engine heritage, not yet
> $4000 ;              investigated further
> $4000 ; $8000        Real code entry point: the loader hands off here with
> $4000 ;              JP C,$8000 once both blocks have loaded successfully
> $4000 ; $EE80-$FFFF  Not part of any tape block; leftover/padding RAM content, not
> $4000 ;              meaningful (confirmed for $EE80-$EFFF by inspection; assumed
> $4000 ;              likewise up to $FFFF pending further checking)
> $4000 ; $F000-$F0EB  Paul Owens protection loader (BASIC loader's resident machine
> $4000 ;              code, disassembled by hand - see below)
> $F000 ;
> $F000 ; PAUL OWENS LOADER ($F000-$F0EB)
> $F000 ; --------------------------------
> $F000 ; $F000  Entry point. Clears attributes, then calls the block loader twice
> $F000 ; $F017  CALL $F043 to load the screen block to $4000, length 6912
> $F000 ; $F02A  CALL $F043 to load the main block to $5C00, length 37504
> $F000 ; $F034  JP C,$8000 - handoff to real game code on successful load
> $F000 ; $F037  Border-flash error loop, taken on load failure
> $F000 ; $F043  Block loader: reads a flag byte, then a length-prefixed run of
> $F000 ;         bytes via the low-level bit/edge-reading primitives at $F0CA
> $F000 ; $F0CA  Low-level tape bit-read primitive
> $F000 ; $F0CE  Low-level tape edge-read primitive
> $F000 ;
b $4000 Screen memory
u $5B00 Unloaded gap (pre-shifted backdrop scratch area in the retail game)
b $5C00 Main data+code block (structure not yet identified)
c $8000 Real code entry point (handed off to by the Paul Owens loader; not yet disassembled)
i $EE80 Leftover/padding RAM content; not part of any tape block
c $F000 Paul Owens protection loader
i $F0EC Leftover/padding RAM content; not part of any tape block
