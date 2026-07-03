# Non-HQ Functions

Functions in `ChaseHQ.c` that have not yet received full high-quality processing.
A function is HQ when its prologue carries `[Conv: HQ]`; see CLAUDE.md for the
full criteria.

Generated from `grep '^ \* \$' ChaseHQ.c | grep -v '\[Conv: HQ\]'`.

| Address | C function | Description |
|---------|------------|-------------|
| `$5C00` | `end_screen` | End screen |
| `$8014` | `load_stage` | Load stage |
| `$8204` | `setup_engine_sfx_48k` | Setup engine SFX 48K |
| `$8234` | `play_engine_sfx_48k` | Play engine SFX 48K |
| `$8258` | `attract_mode_48k` | Attract mode 48K |
| `$83CD` | `bootstrap` | Bootstrap |
| `$8401` | `main_loop` | Main loop |
| `$852A` | `cpu_driver` | CPU driver |
| `$858C` | `run_pregame_screen` | Pre-game radio screen |
| `$85A8` | `run_pregame_screen_loop` | Run pregame screen loop |
| `$85E4` | `reveal_perp_car` | Reveal perp car |
| `$860F` | `animate_meters` | Animate meters |
| `$8646` | `am_set_attrs` | AM set attrs |
| `$865A` | `draw_pregame` | Draw pregame |
| `$873C` | `escape_scene` | Escape scene |
| `$87DC` | `set_up_stage` | Set up stage |
| `$8860` | `set_up_stage_reset_lights` | Set up stage reset lights |
| `$8876` | `check_user_input` | Check user input |
| `$88A9` | `check_user_input_quit_key` | Check user input quit key |
| `$8903` | `drive_sfx` | Drive SFX |
| `$8960` | `sfx_crash` | SFX crash |
| `$89D9` | `sfx_thud` | SFX thud |
| `$8A0F` | `sfx_cornering` | SFX cornering |
| `$8A36` | `sfx_bipbow` | SFX bipbow |
| `$8A57` | `handle_perp_caught` | Handle perp caught |
| `$8C35` | `hpc_set_perp_speed` | HPC set perp speed |
| `$8C3A` | `fully_smashed` | Fully smashed |
| `$8D8F` | `transition` | Transition |
| `$8DD8` | `transition_fade_chunk` | Overwrite odd/even UDG rows of the back buffer with a single byte |
| `$8DF9` | `setup_transition` | Setup transition |
| `$8E42` | `draw_overlay_messages` | Progressively draw overlay messages to the back buffer |
| `$8F5F` | `draw_scene_objects` | Draw everything else |
| `$9052` | `draw_overhead` | Draw overhead objects |
| `$916C` | `draw_stretchy_object_left` | Draw stretchy object left |
| `$9171` | `draw_stretchy_object_right` | Draw stretchy object right |
| `$9174` | `draw_stretchy_object_common` | Draw stretchy objects (e.g. trees) |
| `$924D` | `draw_tunnel_light_left` | Draw tunnel light (left) |
| `$9252` | `draw_tunnel_light_right` | Draw tunnel light (right) |
| `$9278` | `draw_object_left` | Draw object (left) |
| `$9279` | `draw_object_left_entrypt` | Draw object (left) entry point |
| `$9293` | `draw_object_left_stretchy_entrypt` | Draw object left stretchy entry point |
| `$929A` | `draw_object_left_helicopter_entrypt` | Draw object left helicopter entry point |
| `$92E1` | `draw_object_right` | Draw object (right) |
| `$92E2` | `draw_object_right_entrypt` | Draw object (right) entry point |
| `$92FC` | `draw_object_right_stretchy_entrypt` | Draw object (right) stretchy entry point |
| `$9309` | `draw_object_right_helicopter_entrypt` | Draw object right helicopter entry point |
| `$930E` | `draw_object_perspective_entrypt` | Draw object: compute perspective height/width and dispatch |
| `$949C` | `plot_sprite` | Plot an unmasked sprite |
| `$94B1` | `plot_sprite_even` | Plot an unmasked sprite (even byte widths) |
| `$94F2` | `plot_sprite_odd` | Plot an unmasked sprite (odd byte widths) |
| `$9542` | `plot_sprite_flipped` | Plot a flipped sprite |
| `$9565` | `plot_sprite_flipped_even` | Plot a flipped sprite (even byte widths) |
| `$95B3` | `plot_sprite_flipped_odd` | Plot a flipped sprite (odd byte widths) |
| `$9945` | `start_chatter` | Start chatter |
| `$9965` | `drive_chatter` | Drive chatter |
| `$99D3` | `drive_chatter_stop` | Stop chatter |
| `$99EC` | `print_chatter` | Print chatter |
| `$9A24` | `pc_chatter_message` | Print chatter (second entry point) |
| `$9A30` | `pc_clear_line` | Clear the chatter line |
| `$9A55` | `drive_noise_effect` | Drive the noise effect |
| `$9A5C` | `draw_noise_effect` | Draw the noise effect |
| `$9A98` | `ne_plot_attrs` | Set the noise attribute bytes |
| `$9AAB` | `plot_face` | Draw the given face |
| `$9ACE` | `plot_face_attributes` | Set the face attribute bytes |
| `$9AEC` | `plot_mini_font_cursor_off` | Plot a character at X with no cursor block |
| `$9AF1` | `plot_mini_font_cursor_on` | Plot a character at X with a cursor block |
| `$9AF4` | `pmf_go` | Plot a character |
| `$9BCF` | `check_time_up` | Handle "time up", countdown and continue |
| `$9CC2` | `speed_score` | Speed score |
| `$9E11` | `plot_turbos_and_digits` | Plot turbos and digits |
| `$9F1E` | `ptas_led_digits` | Draw LED digits |
| `$9F47` | `ledfont_plot` | Plot an LED font character |
| `$9FB4` | `draw_char` | Draw single character |
| `$A0D6` | `keyscan` | Keyscan |
| `$A112` | `keyscan_keydefs` | Scan a key-definition list, rotating each result into an accumulator |
| `$A399` | `check_scenery_collisions` | Check scenery collisions |
| `$A4B0` | `csc_hit_scenery` | Arrive here if hit scenery (e.g. drove into a tree or lamp post) |
| `$A4B8` | `scenery_hit` | Scenery hit |
| `$A4F6` | `check_fork_scenery_collisions` | Check fork scenery collisions |
| `$A579` | `layout_objects` | Layout objects |
| `$A637` | `perp_behaviour` | Perp behaviour |
| `$A7F3` | `spawn_cars` | Spawn cars |
| `$A89C` | `get_spawn_lanes` | Get spawn lanes |
| `$A8CD` | `hazard_handler` | Hazard handler |
| `$A955` | `choose_dirt_and_stones` | Choose dirt and stones |
| `$A97E` | `layout_dirt_and_stones` | Layout dirt and stones |
| `$A9DE` | `dust_stones_stuff` | Dust stones stuff |
| `$AA38` | `draw_helicopter` | Draw helicopter |
| `$AAC6` | `move_helicopter` | Move helicopter |
| `$AB33` | `drive_helicopter` | Drive helicopter |
| `$AB9A` | `spawn_hazards` | Spawn hazards |
| `$AC3C` | `hazard_hit` | Hazard hit |
| `$ADA0` | `draw_all_hazards` | Draw all hazards |
| `$AECF` | `draw_arrow_fire_smoke` | Draw arrow fire smoke |
| `$AFF1` | `dh_smoke` | DH smoke |
| `$B01B` | `dh_draw` | DH draw |
| `$B01C` | `dh_draw_bitmap` | DH draw bitmap |
| `$B063` | `move_hero_car` | Move hero car |
| `$B318` | `animate_hero_car` | Animate hero car |
| `$B457` | `ahc_check_hand_flag` | AHC check hand flag |
| `$B4CC` | `start_chase` | Start chase |
| `$B4F0` | `smash` | Smash |
| `$B549` | `draw_debris` | Draw debris |
| `$B58E` | `draw_hero_car` | Draw hero car |
| `$B627` | `draw_hero_car_part` | Draw a portion of the hero car |
| `$B648` | `draw_smoke` | Draw smoke |
| `$B67C` | `draw_cherry_light` | Draw cherry light |
| `$B699` | `draw_crash_unflipped` | Draw crash frame with no flip and no extra offset |
| `$B69E` | `draw_crash` | Draw crash |
| `$B6D6` | `draw_part` | Reduce y by car_y then fall through to draw_part_entrypt2 |
| `$B6DD` | `draw_part_entrypt2` | Second entry point into draw_part |
| `$B701` | `draw_part_plot_masked_sprite` | Draw part plot masked sprite |
| `$B716` | `plot_masked_sprite` | Plot a masked sprite |
| `$B76C` | `plot_masked_sprite_flipped` | Plot a flipped and masked sprite |
| `$B770` | `plot_masked_sprite_flipped_entrypt2` | Plot a flipped and masked sprite (2nd entry point) |
| `$B7EF` | `plot_masked_sprite_inverted` | Plot an inverted masked sprite |
| `$B848` | `scroll_horizon` | Scroll horizon |
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
| `$E810` | `entry_48k` | Entry point 48K |
| `$E816` | `entry_128k` | Entry point 128K |
| `$E81D` | `entry_common` | Entry point common |
| `$E8FE` | `stop_the_tape_48k` | "Stop the tape" handler (48K only) |
| `$EBF7` | `menu_draw_strings` | Draw menu strings until a NUL byte |
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
| `$F251` | `start_siren_128k` | Start siren 128K |
| `$F269` | `play_siren_sfx_128k` | Play siren SFX 128K |
| `$F29D` | `silence_audio_128k` | Silence audio 128K |
| `$F2A2` | `write_audio_registers_128k` | Write audio registers 128K |
| `$F2B6` | `engine_sfx_from_speed_128k` | Engine SFX from speed 128K |
| `$F2F1` | `setup_turbo_sfx_128k` | Setup turbo SFX 128K |
| `$F2FA` | `play_turbo_sfx_128k` | Play turbo SFX 128K |
| `$F342` | `play_speech_128k` | Play speech 128K |
| `$F39F` | `handle_perp_caught_128k` | Handle perp caught 128K |
| `$F3B6` | `call_bank_3_128k` | Call bank 3 128K |
| `$F3E2` | `page_128k` | Page 128K |
| `$F414` | `reset_paging_128k` | Reset paging 128K |
| `$F41B` | `attract_mode_128k` | Attract mode 128K |
