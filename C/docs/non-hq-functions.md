# Non-HQ Functions

Functions in `ChaseHQ.c` that have not yet received full high-quality processing.
A function is HQ when its prologue carries `[Conv: HQ]`; see CLAUDE.md for the
full criteria.

Generated from `grep '^ \* \$' ChaseHQ.c | grep -v '\[Conv: HQ\]'`.

| Address | C function | Description |
|---------|------------|-------------|
| `$C2E7` | `draw_road_lanes_change` | Draw road lane change |
| `$C452` | `draw_road` | Draw road |
| `$C4AD` | `dr_read_lanes` | draw_road: read lanes |
| `$C534` | `dr_four_lane_highway` | draw_road: four lane highway |
| `$C54D` | `dr_set_lane_callback` | draw_road: set lane callback and enter fill dispatch |
| `$C551` | `dr_dispatch` | draw_road: dispatch to filled or unfilled path |
| `$C55F` | `dr_advance_unfilled` | draw_road: advance backbuffer and rollover check (unfilled path) |
| `$C565` | `dr_write_scanline_unfilled` | draw_road: write unfilled (zero) road scanline |
| `$C57C` | `dr_rollover_filled` | draw_road: backbuffer row advance with rollover (filled path) |
| `$C58A` | `dr_rollover_unfilled` | draw_road: backbuffer row advance with rollover (unfilled path) |
| `$C598` | `dr_dispatch_filled` | draw_road: enter filled path (set fill_fn and start advance) |
| `$C5A1` | `dr_advance_filled` | draw_road: advance backbuffer and rollover check (filled path) |
| `$C5A7` | `dr_fill` | draw_road: setup and draw right verge and centre spans |
| `$C62E` | `dr_fill_left_stripe` | draw_road: left hand verge fill, road edges and lane markers |
| `$C79A` | `dr_start_backdrop_fill` | Backdrop copy and sky fill |
| `$C8BE` | `pre_shift_backdrop` | Pre-shift backdrop |
| `$C8E3` | `draw_forked_road` | Forked road plotter |
| `$CBC5` | `backdrop_fill_dispatch` | Backdrop fill choice (height diff >= $50 in forked road) |
| `$CBD6` | `build_curve_table` | Build curve table |
| `$E81D` | `entry_common` | Entry point common |
| `$ECF3` | `redefine_keys_48k` | Redefine keys 48K |
| `$ED4D` | `keyscan_all` | Keyscan |
| `$ED6D` | `define_a_key` | Define a single key |
| `$EDCC` | `dak_move_down` | DAK move down |
| `$EE9E` | `play_music_48k` | Play music 48K |
| `$EF19` | `interrupt_entry` | Interrupt entry |
| `$EF22` | `playdrum_2` | Playdrum 2 |
| `$EF29` | `playdrum_1` | Playdrum 1 |
| `$EF2E` | `playdrum_start` | Playdrum start |
| `$EF38` | `playdrum_bank_go` | Playdrum bank go |
| `$EF39` | `playdrum_go` | Playdrum go |
| `$F0C6` | `noise` | Noise |
| `$F342` | `play_speech_128k` | Play speech 128K |
| `$F41B` | `attract_mode_128k` | Attract mode 128K |
