# Non-HQ Functions

Functions in `ChaseHQ.c` that have not yet received full high-quality processing.
A function is HQ when its prologue carries `[Conv: HQ]`; see CLAUDE.md for the
full criteria.

Generated from `grep '^ \* \$' ChaseHQ.c | grep -v '\[Conv: HQ\]'`.

| Address | C function | Description |
|---------|------------|-------------|
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
