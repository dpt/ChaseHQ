> $C000 ; ChaseHQ-128K-bank-3.skool
> $C000 ;
> $C000 ; Bank 3 holds the animated title screen.
> $C000 ;
> $C000
@ $C000 org
c $C000 Entry points
C $C000,3 Title animations
C $C003,3 Check high score
C $C006,3 Plays success music
C $C009,3 Entry point for keyboard/joystick selection menu
c $C00C Format the score and check the high-score table
D $C00C Formats the 8-digit BCD #R$8002 (score_bcd) into an ASCII digit string with leading zeros blanked, then scans the 10-row high-score table at $C408 (33 bytes/row, score string first) to see whether the new score beats or ties an existing entry. Falls through into #R$C06E on a hit, with the beaten row's address left on the stack.
R $C00C Used by the routines at #R$C000 and #R$C59E.
@ $C00C label=check_high_score
C $C00C,11 Copy a 13-byte blank row template from $C580 to $C58D
C $C017,3 B = 4 BCD bytes to convert; C = 0 (no significant digit seen yet)
C $C01A,3 DE -> most significant byte of score_bcd
C $C01D,3 HL -> destination digit string
@ $C020 label=chs_digit_loop
C $C020,1 A = next BCD byte (two digits)
C $C021,4 Rotate the byte left 4 times so the old high (tens) nibble ends up in the low 4 bits
C $C025,2 A = tens digit of this byte
C $C027,2 Jump if the digit is non-zero
C $C029,4 Test the "significant digit seen" flag (all-$00 or all-$FF): carry = flag
C $C02D,2 Suppressed leading zero -> space
@ $C031 label=chs_digit_nonblank
C $C031,2 Mark "significant digit seen" for the rest of the score
C $C033,2 Convert BCD digit to ASCII
@ $C035 label=chs_store_digit
C $C035,1 Store tens digit
C $C037,1 A = same BCD byte again
C $C038,2 A = units digit of this byte
C $C03A,2 Jump if the digit is non-zero
C $C03C,2 Test the flag again
C $C040,2 Suppressed leading zero -> space
C $C044,2 Mark "significant digit seen"
C $C046,2 Convert BCD digit to ASCII
C $C048,1 Store units digit
C $C04A,1 Move to the next (less significant) BCD byte
C $C04B,2 Loop for all 4 bytes (8 digits total)
@ $C04D label=chs_row_loop
C $C04D,3 DE -> first row of the 10-row high-score table
C $C050,2 C = 10 rows left to check
C $C052,1 Save this row's address
C $C053,2 B = 8 digits to compare
C $C055,3 HL -> new score's digit string
@ $C058 label=chs_digit_cmp
C $C058,1 A = table entry's digit
C $C059,1 Compare against the new score's digit
C $C05A,2 Table digit < new digit -> new score beats this entry
C $C05C,2 Table digit > new digit -> this entry still outranks it
C $C05E,1 Digits equal -> compare the next pair
C $C062,2 All 8 digits equal -> treat as beating this entry too
@ $C064 label=chs_row_beaten
C $C064,1 HL = this row's address
C $C065,3 Row stride is 33 bytes
C $C068,1 Advance to the next row
C $C06A,1 One row fewer to check
C $C06B,2 Loop while rows remain
C $C06D,1 New score doesn't make the top 10 -> return
c $C06E Prepare and display a new high-score entry
D $C06E Reached from #R$C00C when the new score beats or ties the table row whose address was pushed there. Shifts the rows below the insertion point down by one (33 bytes each; row 1 needs no shift), writes the score digits, stage code and retry-attempt number into the row, runs a highlight/flash display, then drops into the joystick-driven name-entry loop at #R$C155.
R $C06E Used by the routine at #R$C00C.
@ $C06E label=insert_high_score_entry
C $C06E,1 A = row counter (1..10, from #R$C00C)
C $C06F,1 Bank the row counter
C $C070,1 A = row counter again
C $C072,2 Row 1 (bottom of the table) -> nothing below to shift
@ $C074 label=ihe_shift_rows
C $C074,3 DE -> one past the last table row (shift destination)
C $C077,3 HL -> last table row (shift source)
C $C07A,5 Shift 26 bytes of the row down into the row below
C $C07F,9 then step both pointers back the rest of the 33-byte row stride without copying -- the trailing 7 bytes of each row (2 padding bytes + the row's own 5-char rank suffix, e.g. "2ND  " -- see the data at #R$C3AF) are static labels, not player data, so they are simply left untouched by the shift
C $C088,1 One row done
C $C089,2 Loop up through the rows above the insertion point
@ $C08B label=ihe_write_row
C $C08B,1 DE -> the row being inserted (address pushed by #R$C00C)
C $C08C,3 HL -> formatted score digit string
C $C08F,5 Write the 8 score digits into the row
C $C094,4 DE += 4
C $C098,3 A = wanted_stage_number
C $C09B,4 C = stage_number * 3 -- index into a 3-byte-per-stage table
C $C09F,3 HL -> stage code/name table (3 bytes/stage)
C $C0A2,1 HL -> this stage's entry
C $C0A3,4 Write the 3-byte stage code into the row
C $C0A7,1 HL -> row (DE no longer needed for the copy)
C $C0A8,3 HL += 5
C $C0AB,3 A = retry_count (0/1/2)
C $C0AE,2 A = ASCII '1'..'3'
C $C0B0,1 Write the attempt-number digit into the row
C $C0B1,3 HL += 3
C $C0B4,3 Remember this position
C $C0B7,5 Write two static full stops ("..")
C $C0BD,2 A third full stop with bit 7 set -- likely the flash attribute variant, used as a blinking name-entry cursor
C $C0BF,1 Restore the row counter (banked at $C06F)
C $C0C0,3 Save the row counter
C $C0C3,1 B = row counter
N $C0C4 Address = $C401 + (10 - row_counter) * 33: locates this row's entry in a 33-byte-per-row table (screen or attribute positions for the 10-row name-entry table), used below to scroll the display to the new entry. The multiply is done as (10-row)*32 (5x ADD A,A, overflow captured in D via RL D) plus one more (10-row) added back in (ADD A,C) -- i.e. x*32 + x = x*33.
C $C0D8,3 Save the computed address
N $C0DB Border-flash colour = $A0 - clamp(7 - row_counter, 0..7) * 16: fades from bright ($A0) for early rows to darker for rows further down the table, giving the flashing highlight a position-dependent brightness.
C $C0E9,3 Save the computed flash colour (read back at $C13C)
C $C0EC,3 See #R$C890
N $C0EF Clear/prepare screen buffers for the name-entry display ($C0EF-$C132). Not traced instruction-by-instruction here.
C $C104,6 Zero two 32-byte blocks at $59E0 and $5AE0 in lockstep
C $C110,2 A = 10 (rows)
C $C112,13 Copy 2 bytes per row from the table at $C54B into the row-offset table at $C401 (31-byte stride)
C $C11F,3 HL -> #R$C3AF data block
C $C122,3 Common routine outside this bank
C $C125,3 See #R$F7AA
C $C12A,3 See #R$F7D6, called with A=3
C $C12E,1 Wait for the next interrupt
C $C130,3 Clear a flag/self-modified byte at $C2F2
@ $C133 label=ihe_flash_loop
C $C133,3 See #R$F82F
C $C136,3 See #R$C2B1
C $C139,3 See #R$C16A
@ $C149 label=ihe_entry_loop
C $C13C,25 Recomputes to a constant $9F every pass, so JR NZ is always taken here -- this loop can only actually end via a side effect inside one of the three CALLs above (e.g. popping this return address), not via this test $C9 = RET opcode Self-modify $C2F2 to RET Handle one frame of name entry (see below) See #R$C16A See #R$F82F Loop -- exits via a side effect inside a callee
N $C155 This entry point is used by the routine at #R$C16A.
@ $C155 label=name_entry_frame
C $C155,3 HL -> a saved (DE) pair, likely the current cursor/
C $C158,1 letter-selection state
C $C15C,1 Swap in DE (bank what was the caller's shadow set)
C $C15D,3 Preserve the caller's shadow BC/DE/HL on the stack, in case #R$C2FB uses EXX itself
C $C160,1 Swap back -- main regs include DE (the loaded state)
C $C161,3 See #R$C2FB (joystick/letter-selection update)
C $C164,1 Swap to the (now current) shadow set
C $C165,3 Restore the caller's shadow BC/DE/HL
C $C168,1 Swap back to the main set
c $C16A Name-entry cursor flash and letter-selection input
D $C16A Handles one frame of the high-score name-entry screen: flashes the underline cursor beneath the current letter/name-length position, reads input via #R$800E, and on confirm (bit 4) either advances to #R$C25D (scroll/select the next letter) or, if the entry is finished, stores the chosen character into the row (cursor position at $C59C, see #R$C06E) and patches $C2F2 to RET (see #R$C2B1 / #R$C2F6) to end the entry loop.
R $C16A Used by the routine at #R$C06E.
@ $C16A label=name_entry_input
C $C16A,3 HL -> flash-timer counter
C $C16E,2 Skip the cursor toggle until the timer hits zero
C $C170,2 Reload the timer (12 frames)
N $C172 Advances the blinking selector by writing attribute $46 into cell $5967+offset ($C596 holds the 0-19 offset within this row). While the advanced offset is still < 20 ($14), that is the whole effect for this call: save offset+1 back to $C596 and return via $C19B. Once the offset would wrap past 20, the selector has completed a full pass along this row: additionally nudge the row pointer held at $C597 -- clear its old highlighted cell (attribute $42), move it on by one screen line (+$20) then step it back by 31 (net +1 down and +1 right, i.e. onto the next row's first cell, attribute $02) -- and if that pointer's low byte has itself reached $17 (23), the whole multi-row cycle is complete and control passes to #R$C212 (name entry finished/row exhausted) instead of resetting the offset to 0 and continuing.
C $C19E,3 Clear the old cursor cell
C $C1A8,5 Toggle two flash-phase flag bytes ($C59A, $C59B)
C $C1CF,3 HL -> current position in the letter/name table
C $C1D5,2 Preserve HL/DE across the input read
C $C1D7,3 Read input (common routine outside this bank)
C $C1DC,2 Mask to the relevant input bits
C $C1DE,3 Nothing relevant pressed
C $C1E1,2 Test the confirm/fire bit
C $C1E3,2 Not confirming -> scroll/select the next letter
C $C1E5,7 Skip if a "locked" flag is set
C $C1EC,1 E += 2
C $C1EF,2 At the maximum name length?
C $C1F1,2 Yes -> finalise without storing another character
C $C1F8,4 DE -> row write cursor (see #R$C06E)
C $C1FC,1 A = selected character code
C $C1FD,6 $40 is the "blank" marker -> display as a full stop
C $C203,1 Store the confirmed character into the row
C $C205,4 Advance the row write cursor
C $C20B,3 Save the updated letter-table position
@ $C212 label=nei_finalise
C $C217,6 $40 is the "blank" marker -> store as a full stop
C $C22A,1 Store the final character into the row
C $C22F,2 $C9 = RET opcode
C $C231,3 Patch $C2F2 to RET, ending the entry loop in #R$C06E
C $C234,3 Common routine outside this bank (confirm sound?)
C $C23B,3 Run one more entry frame (see #R$C155)
N $C23E Redraw the cursor cells one final time ($C23E-$C251), mirroring $C172-$C18D above.
C $C255,3 See #R$F7D6, called with A=2
C $C258,3 See #R$F82F
C $C25B,2 Loop -- exits via a side effect inside a callee
c $C25D Cycle the selected letter and draw it
D $C25D Entered from #R$C16A with C = the masked direction bits. Advances or retreats the letter code at (HL) by one (wrapping through the blank marker $40), then falls through to redraw the corresponding glyph on screen.
R $C25D Used by the routine at #R$C16A.
@ $C25D label=cycle_and_draw_letter
C $C25D,1 C = masked input bits (bit 0 = next, bit 1 = previous)
C $C25E,1 A = current letter code
C $C25F,4 Bit 0 -> A += 1
C $C263,4 Bit 1 -> A -= 1
C $C267,6 Wrap below the range to the top code ($5A)
C $C26D,6 Wrap above the range to the blank marker ($40)
C $C273,1 Store the new letter code
@ $C274 label=cad_clear_locked
N $C275 This entry point is used by the routine at #R$C16A.
C $C275,3 Clear the "locked" flag (see #R$C16A)
@ $C278 label=cad_draw_entry
C $C278,1 A = letter code to draw
C $C27B,2 Blank marker?
C $C27F,2 Map the letter code to a font-table index
C $C282,11 Compute this glyph's offset into the font bitmap table (index * 6): not traced bit-by-bit here
C $C28D,3 HL = font bitmap table base
C $C290,1 HL -> this glyph's bitmap
N $C291 This entry point is used by the routine at #R$C16A.
@ $C291 label=cad_draw_glyph
C $C291,1 Save the screen address
C $C292,10 Draw the top 4 pixel rows of the glyph, each row 256 bytes further down the ZX screen (same third)
C $C29C,8 Adjust DE across the screen-third boundary and down one character row (standard ZX non-linear screen addressing)
C $C2A4,10 Draw the remaining 3 pixel rows the same way
C $C2AE,1 Restore the screen address
C $C2AF,1 Advance one column, ready for the next glyph
c $C2B1 Scroll the 10 high-score row addresses down by one pixel line
D $C2B1 Advances each of the 10 screen addresses in the row-offset table at $C401 (built by #R$C06E) down by one pixel row, handling the ZX screen's non-linear third-boundary wrap. If a row's address runs into one of the bottom-of-window bounds ($C2D3-$C2EB), calls #R$C2F6 to redraw/reset that row's name text. The byte at $C2F2 (normally NOP) is patched to RET by #R$C16A when name entry finishes, which stops this scroll immediately.
R $C2B1 Used by the routine at #R$C06E.
@ $C2B1 label=scroll_score_rows
C $C2B1,2 B = 10 rows
C $C2B3,3 HL -> row-offset table
@ $C2B6 label=ssr_row_loop
C $C2B6,3 DE = this row's screen address
C $C2B9,1 Advance one pixel row
C $C2BB,19 Handle the screen-third boundary wrap (as in #R$C25D)
C $C2CE,5 Write the updated address back into the table
N $C2D3 $C2D3-$C2EB tests whether this row's address has reached the bottom of one of its two allowed screen windows; if so, jumps to #R$C2F6 instead of continuing the scroll.
C $C2EE,3 Advance to the next row's table slot (31-byte stride)
N $C2F2 This entry point is used by the routine at #R$C2F6.
@ $C2F2 label=ssr_continue
C $C2F2,1 Self-modified to RET by #R$C16A to stop the scroll
c $C2F6 Redraw a high-score row's name text, letter by letter
D $C2F6 Reached from #R$C2B1 when a row's scroll position hits the bottom of its window, and from #R$C06E (via the #R$C2FB entry point) once per name-entry frame. Draws each character of the row's name field (terminated by a byte with bit 7 set) using the shadow register set, so the caller's main BC/DE/HL (the #R$C2B1 row loop, or the caller's own state) survive the call unharmed. The per-character glyph offset/address arithmetic here mirrors #R$C25D and is not traced line-by-line.
R $C2F6 Used by the routine at #R$C2B1.
@ $C2F6 label=redraw_score_name
N $C2FB This entry point is used by the routine at #R$C06E.
@ $C2FB label=redraw_name_frame
C $C2FB,7 Toggle a blink flag at $C58D
C $C302,2 Alternate blink phase -> the second name-draw loop below
@ $C304 label=rsn_char_loop
C $C304,1 A = next name character
C $C309,3 Non-space -> draw it
C $C30C,1 Space -> just advance one column
C $C30D,2 End of name (bit 7 set)?
C $C310,3 No -> next character
C $C313,3 Yes -> resume the #R$C2B1 scroll loop
C $C316,3 Move the screen address into the shadow DE
C $C33E,15 Compute this glyph's bitmap address in font table $800C (offset = index * 6, as in #R$C25D)
@ $C353 label=rsn_draw_row
C $C353,9 Draw one pixel row of the glyph, handling the screen third-boundary wrap (as in #R$C25D / #R$C2B1)
C $C35C,3 More glyph rows to draw
C $C35F,1 Restore the caller's BC/DE/HL
C $C360,3 Next character
C $C363,15 Screen-third boundary wrap
C $C377,1 Restore the caller's BC/DE/HL
C $C378,3 Next character
N $C37B Second name-draw loop, used on the alternate blink phase ($C302): scans the same string as the draw loop above (bit-7-terminated, checked at $C37C/$C384) but instead of looking up and blitting each glyph's bitmap, it writes 8 rows of zero bytes per character ($C390-$C3A9, same screen-third-wrap handling as $C363-$C371) -- i.e. this erases the text rather than drawing it, giving the name-entry cursor its blink effect by alternating between #R$C304 (draw) and this loop (erase) each frame.
@ $C37B label=rsn_char_loop2
C $C38A,3 Resume the #R$C2B1 scroll loop
C $C3AB,1 Restore the caller's BC/DE/HL
C $C3AC,3 Next character
b $C3AF High-score table screen: strings, preset rows and layout tables
D $C3AF Each text string here is encoded as 2 position-control bytes, 1 attribute byte, then ASCII text with the final character's bit 7 set to mark the end of the string (consumed by the print routine reached via #R$C122). The four strings, decoded in full: $C3AF pos=$02,$0A attr=$48 "BEST OFFICERS" $C3BF pos=$C6,$67 attr=$48 "ENTER YOUR INITIALS" $C3D5 pos=$07,$8E attr=$48 ". . ." (3-initial cursor placeholder) $C3DD pos=$C6,$C0 attr=$48 "RANK    SCORE  STAGE  PLAY  NAME" The last string's terminator byte falls exactly at $C3FF, where the preset-rows table below begins.
R $C3AF These are followed by the 10 preset high-score table rows themselves (the
R $C3AF same 33-byte layout #R$C06E writes into: 8-digit score, stage code,
R $C3AF play/attempt count, 3-character initials -- full field breakdown at
R $C3AF $C400 below). The preset initials include "JOB" (row 1) -- John O'Brien,
R $C3AF this game's programmer.
N $C3AF At $C54B: 10 packed 2-byte values, copied by #R$C06E into the row-offset table at $C401 (one per high-score row, for #R$C2B1's scroll). At $C567: the 6-entry, 3-byte-per-stage code table read by #R$C06E (" 1 ", " 2 ", " 3 ", " 4 ", " 5 ", "ALL" -- indexed by wanted_stage_number, #R$8007, which is 1..5 or 6 for the end screen). At $C55F: the 8-byte score-digit buffer written by #R$C00C and read by #R$C06E (initialised here to "00000000").
B $C3AF,2,2 Position
B $C3B1,1,1 Attribute
T $C3B2,13,12:n1 "BEST OFFICERS"
B $C3BF,2,2 Position
B $C3C1,1,1 Attribute
T $C3C2,19,18:n1 "ENTER YOUR INITIALS"
B $C3D5,2,2 Position
B $C3D7,1,1 Attribute
T $C3D8,5,4:n1 ". . ."
B $C3DD,2,2 Position
B $C3DF,1,1 Attribute
T $C3E0,32,31:n1 "RANK    SCORE  STAGE  PLAY  NAME"
N $C400 Ten preset rows follow, 33 bytes each (base $C408, stride #R$C065): score (8 ASCII digits), a 4-byte gap, stage code (3 chars), a 5-byte gap, retry digit (1 ASCII char), a 2-byte gap, 3-char name (bit-7 terminated, written by #R$C06E), then a 7-byte tail. The tail is 2 zero bytes followed by the *next* row's 5-char rank suffix ("2ND  " etc) -- these suffixes are static screen labels tucked into the previous row's unused stride, not part of the row that follows. Row 10 has no eleventh row to label, so its tail instead begins the packed offset table at $C54B.
B $C400,3,3 Padding
T $C403,5,5 Row 1's rank suffix
T $C408,8,8 Row 1 score
B $C410,4,4 Gap
T $C414,3,3 Row 1 stage code
B $C417,5,5 Gap
B $C41C,1,1 Row 1 retry attempt '2'
B $C41D,2,2 Gap
T $C41F,3,2:n1 "JOB" (row 1 name)
B $C422,2,2 Padding
T $C424,5,5 Row 2's rank suffix
T $C429,8,8 Row 2 score
B $C431,4,4 Gap
T $C435,3,3 Row 2 stage code
B $C438,5,5 Gap
B $C43D,1,1 Row 2 retry attempt '2'
B $C43E,2,2 Gap
T $C440,3,2:n1 "BIL" (row 2 name)
B $C443,2,2 Padding
T $C445,5,5 Row 3's rank suffix
T $C44A,8,8 Row 3 score
B $C452,4,4 Gap
T $C456,3,3 Row 3 stage code
B $C459,5,5 Gap
B $C45E,1,1 Row 3 retry attempt '1'
B $C45F,2,2 Gap
T $C461,3,2:n1 "JON" (row 3 name)
B $C464,2,2 Padding
T $C466,5,5 Row 4's rank suffix
T $C46B,8,8 Row 4 score
B $C473,4,4 Gap
T $C477,3,3 Row 4 stage code
B $C47A,5,5 Gap
B $C47F,1,1 Row 4 retry attempt '2'
B $C480,2,2 Gap
T $C482,3,2:n1 "JAM" (row 4 name)
B $C485,2,2 Padding
T $C487,5,5 Row 5's rank suffix
T $C48C,8,8 Row 5 score
B $C494,4,4 Gap
T $C498,3,3 Row 5 stage code
B $C49B,5,5 Gap
B $C4A0,1,1 Row 5 retry attempt '1'
B $C4A1,2,2 Gap
T $C4A3,3,2:n1 "ROB" (row 5 name)
B $C4A6,2,2 Padding
T $C4A8,5,5 Row 6's rank suffix
T $C4AD,8,8 Row 6 score
B $C4B5,4,4 Gap
T $C4B9,3,3 Row 6 stage code
B $C4BC,5,5 Gap
B $C4C1,1,1 Row 6 retry attempt '1'
B $C4C2,2,2 Gap
T $C4C4,3,2:n1 "J.B" (row 6 name)
B $C4C7,2,2 Padding
T $C4C9,5,5 Row 7's rank suffix
T $C4CE,8,8 Row 7 score
B $C4D6,4,4 Gap
T $C4DA,3,3 Row 7 stage code
B $C4DD,5,5 Gap
B $C4E2,1,1 Row 7 retry attempt '1'
B $C4E3,2,2 Gap
T $C4E5,3,2:n1 "M.L" (row 7 name)
B $C4E8,2,2 Padding
T $C4EA,5,5 Row 8's rank suffix
T $C4EF,8,8 Row 8 score
B $C4F7,4,4 Gap
T $C4FB,3,3 Row 8 stage code
B $C4FE,5,5 Gap
B $C503,1,1 Row 8 retry attempt '1'
B $C504,2,2 Gap
T $C506,3,2:n1 "STE" (row 8 name)
B $C509,2,2 Padding
T $C50B,5,5 Row 9's rank suffix
T $C510,8,8 Row 9 score
B $C518,4,4 Gap
T $C51C,3,3 Row 9 stage code
B $C51F,5,5 Gap
B $C524,1,1 Row 9 retry attempt '1'
B $C525,2,2 Gap
T $C527,3,2:n1 "PIX" (row 9 name)
B $C52A,2,2 Padding
T $C52C,5,5 Row 10's rank suffix
T $C531,8,8 Row 10 score
B $C539,4,4 Gap
T $C53D,3,3 Row 10 stage code
B $C540,5,5 Gap
B $C545,1,1 Row 10 retry attempt '1'
B $C546,2,2 Gap
T $C548,3,2:n1 "IES" (row 10 name)
N $C54B $C54B: 10 packed 2-byte values, copied by #R$C06E into the row-offset table at $C401 (screen/attribute address pairs used by the scroll code).
B $C54B,20,8*2,4
T $C55F,8,8 Score-digit entry buffer
N $C567 $C567: the 6-entry stage-code table indexed by #R$C06E ($C09F) to fill each row's stage-code field.
T $C567,18,3
B $C579,6,6 Padding
B $C57F,31,8*3,7
c $C59E Routine at C59E
D $C59E Title-screen driver: picks one of 5 pre-scripted animation scenes, populates the 9-entry animated-object array at $BB00 from the chosen scene's object table, draws overlay text (title/credits, and an "insert coin" prompt when in coin-op mode), then falls into the attract-mode wait loop (#R$C61E) which animates the scene each frame while polling for coin/fire/keyboard input to start a game.
R $C59E Used by the routines at #R$C000 and #R$FBC8.
@ $C59E label=title_screen_driver
C $C59E,3 Clear the screen bitmap and attribute buffers
C $C5A1,2 A = 0 (self-modified below to persist a running value between calls -- acts as a pseudo-random/rotating scene selector)
C $C5A9,3 Self-modify the operand at $C5A2 (the value loaded next call) with the rotated/incremented value
C $C5AC,27 Pick one of 5 scene tables by testing successive bits of A via RRA; first bit set selects the table
C $C5C7,1 Save chosen scene table pointer
C $C5C8,3 Draw the copyright/credits text block ($CC50)
C $C5CE,13 Zero the whole $BB00-$BB4F object array (9 records x 9 bytes -- see #R$C705 for field layout)
C $C5DB,39 Copy the 5-byte-per-object scene table (pointed to by the popped HL) into the 9 object records, one object per iteration, reordering into fields: script pointer low/high (+$07/+$08), then two more bytes into +$06 and +$04/+$05
C $C602,3 (not traced in detail here -- appears unrelated setup, e.g. sound/interrupt state)
C $C605,3 Draw the first animation frame before entering the wait loop, so the scene is visible immediately
C $C608,3 Draw the "BEST OFFICERS"-style title text block
C $C60E,3 If in coin-op (arcade) mode...
C $C612,3 ...also draw the "insert coin" text block
C $C619,3 (not traced -- likely sound-related)
C $C61D,1 Sync to the next interrupt before the wait loop
N $C61E Attract-mode wait loop: animates the current scene once per interrupt and polls for coin-insert / fire / any-key input to start the game or jump to a fresh title screen. Re-entered every frame via #R$C61E; #R$C59E is re-run (new scene) when a key other than fire is pressed.
@ $C61E label=ts_wait_loop
C $C61E,3 Per-frame service call (not traced -- likely sound or sprite bookkeeping)
C $C621,3 If a flag is already set, skip straight to the
C $C624,1 fire-button check (demo/attract cycle already
C $C625,2 running, don't replay the tune-wait below)
C $C627,2 Otherwise play tune #4 and wait out ~180 frames
C $C629,3 (one #R$F82F service call per frame) before
C $C62C,2 falling through to the coin/name-table refresh
C $C62E,1 at #R$C69A
C $C635,1 B wraps 0 -> 1 (DJNZ leaves B = 0)
@ $C638 label=ts_check_fire
C $C638,2 Read keyboard half-row for SPACE (fire)
C $C63E,3 Fire pressed -> start the game
C $C641,3 In coin-op mode, read the coin-slot input
C $C64C,3 Coin inserted -> refresh coin/credit display
C $C64F,3 If not in coin-op mode, check the "1"/"2" player
C $C652,1 select keys instead
C $C65C,3 Neither select key held -> check for "any key"
C $C65F,1 A player-select key was pressed: initialise a
C $C660,3 fresh #R$8002 (score_bcd) of $87654321 (a
C $C663,3 recognisable placeholder/test score) and stage/
C $C666,2 retry state, check it against the high-score
C $C668,1 table, then restart the title screen
C $C681,2 Read keyboard half-row for ENTER
C $C688,3 No key pressed -> keep waiting
C $C68B,1 A key was pressed: seed the scene selector with
C $C68C,3 the key-scan bits, then restart the title screen
C $C68F,1 with a new scene
@ $C696 label=ts_coin_inserted
C $C696,3 Push $8011 as an extra "credit awarded" flag/value
C $C699,1 for the shared tail below
@ $C69A label=ts_refresh_name_table
C $C69A,1 Entry point also reached directly after the ~180- frame attract-tune wait (#R$C61E), without the $8011 flag push above
C $C69C,3 (not traced -- likely a sound/ROM-switch helper)
C $C69F,35 Copy the 3 preset high-score name/rank rows from $C403 into the work buffer pointed to by ($800A), 3 times (A = 3), each copy split into 15+7+6 bytes with 2-byte gaps skipped between segments
C $C6C2,1 Discard the flag/counter pushed by the caller
N $C6C4 Per-frame animation driver: waits for the next interrupt, draws the 6 "foreground" objects (records 0-5 of the $BB00 array) via #R$C8C5, steps the object animation scripts (#R$C705), clears the playfield bitmap (#R$CC04), then draws the 3 "background" objects (records 6-8) via the alternate blitter #R$C94F. Loops forever -- exited only by #R$C93C / #R$C9D3-style RET-via-restored-SP inside the blitters when a draw overruns the interrupt, or by the caller of #R$C6C4 abandoning the wait loop (e.g. fire pressed). Uses EXX around each #R$C8C5/#R$C94F call: the object record's script pointer (B/C) and screen-position byte (L) are loaded into the shadow registers so the blitter can use HL/DE/BC freely without disturbing the loop's own IX/DE/B state in the main set.
@ $C6C4 label=ts_animate_frame
C $C6C7,2 6 foreground objects
C $C6C9,3 Object record stride (9 bytes)
@ $C6D0 label=ts_draw_fg_objects
C $C6D0,1 Bank in shadow BC/DE/HL for the blitter call
C $C6DD,1 Restore main BC/DE/HL (IX/DE loop state)
C $C6E2,1 (unbalanced EXX vs. the loop above -- swaps back to the blitter's shadow set one more time before the object-script step, effect not fully traced)
C $C6E3,3 Step every object's animation script by one frame
C $C6E6,3 Clear the playfield bitmap ready for the next frame
C $C6EA,4 Background objects start at record 6 ($BB00 + 6*9)
C $C6EE,2 3 background objects
@ $C6F0 label=ts_draw_bg_objects
N $C705 Object animation script interpreter: advances all 9 objects' scripts by one frame. Each object record (9 bytes, offsets relative to IX): +$00 current opcode / countdown-active flag (0 = idle, fetch next op) +$01 countdown value for the "wait N frames" opcode +$02 X velocity/step, +$03 Y velocity/step +$04/+$05 script pointer (low/high) -- the object's byte-code cursor +$06 screen row/position byte consumed by the blitters +$07/+$08 current X/Y screen position The script byte-code (fetched at #R$C740) is a simple state machine: opcode bytes >= $80 (sign bit set) are treated as immediate 2-axis step deltas (#R$C868); opcodes $C8-$D1 select a movement/velocity mode (constant velocity, decelerate-to-stop via the #R$D272 lookup table, accelerate, or a literal position jump) and may consume further operand bytes from the script stream. Two separate dispatch chains share this state machine: 1. #R$C70E (the per-object per-frame entry): dispatches on the *active* opcode already stored at +$00 ($C9-$CF, one of the six movement modes below) and performs one frame's worth of incremental movement. An idle object (+$00 = 0) instead falls through to #R$C740 to fetch a fresh opcode from the script stream. 2. #R$C746 (oss_fetch_opcode_cont): dispatches on the *next script byte* fetched from the stream ($C8-$D2, or an immediate step if bit 7 is clear), reads that opcode's operand bytes from the stream, and stores them into +$00-+$03 ready for chain 1 to act on next frame. The six active movement modes (chain 1) are: constant velocity ($C9, #R$C78D -- position += velocity every frame, no countdown); decelerate X/Y ($CA/$CB, #R$C7ED/#R$C812 -- apply the current step, then look up the next speed from #R$D272 indexed by the +$01/+$03 countdown value, negate it into the other axis's velocity, and count the countdown up toward zero); and three accelerate-X variants ($CC/$CD/$CE, #R$C827/ #R$C853/#R$C83E -- apply the current step, look up the next speed from #R$D272 the same way, and count +$02 down/up/down respectively). The "wait N frames" opcode ($CF) has no chain-1 handler of its own: chain 1 falls through to #R$C731 (oss_countdown), which just decrements +$01 and goes idle (opcode -> 0) when it reaches zero. #R$D272 (oss_lookup_speed, #R$C804) is a 256-byte deceleration/ acceleration curve table indexed by the countdown value passed in C (B is temporarily zeroed for the table-relative add and shuttled through A so the caller's B -- the outer object-loop DJNZ counter -- survives the call); table[C] >> 2 gives that countdown step's velocity magnitude, shared by all five countdown-driven modes.
@ $C705 label=object_script_step
@ $C70E label=oss_object_loop
C $C70E,3 Opcode 0 = idle -> fetch the next script opcode
C $C715,2 Dispatch on the active movement-mode opcode
C $C717,3 ($C9 = constant velocity)
C $C71A,1 ($CA = decelerate X, via #R$D272)
C $C71E,1 ($CB = decelerate Y)
C $C722,1 ($CC = accelerate X)
C $C726,1 ($CD = ? -- accelerate/decelerate variant)
C $C72A,1 ($CE = ? -- accelerate/decelerate variant)
C $C72E,1 ($CF = "wait N frames": falls through to the
C $C72F,2 countdown decrement below)
@ $C731 label=oss_countdown
C $C731,3 Decrement the wait counter; when it reaches 0,
C $C734,3 go idle so the next frame fetches a new opcode
@ $C73B label=oss_next_object
@ $C740 label=oss_fetch_opcode
C $C740,3 HL = object's script cursor
@ $C746 label=oss_fetch_opcode_cont
C $C746,1 A = next script byte, advance the cursor
C $C749,3 Sign bit clear ($00-$7F) -> immediate step opcode
C $C74C,3 Sign bit set -> store as the new active opcode and
C $C74F,2 dispatch on which one it is
C $C751,2 ($C8 = set screen-row byte, 1 operand byte)
C $C753,1 ($C9 = set velocity, 2 operand bytes)
C $C756,1 ($CA/$CB = decelerate, 3 operand bytes)
C $C75D,1 ($CC/$CD/$CE = accelerate variants, 3 operand
C $C75E,2 bytes)
C $C766,1 ($CF = "wait N frames", 1 operand byte)
C $C769,1 ($D0 = jump to absolute position, 2 operand bytes)
C $C76C,2 ($D2 = end of script: restore the caller's saved
C $C76E,2 HL and return, i.e. stop animating this object)
@ $C772 label=oss_save_cursor
C $C772,3 Save the advanced script cursor back to the object
C $C775,3 record, then loop back to re-fetch/execute
C $C77B,1 Read the 2 velocity operand bytes
@ $C785 label=oss_read_wait_operand
C $C785,1 Read the 1-byte wait-count operand
@ $C78D label=oss_op_velocity
C $C78D,3 Constant velocity: position += velocity each frame
@ $C7A2 label=oss_apply_x_step
C $C7A2,3 Helper: X position += X velocity
@ $C7AC label=oss_apply_y_step
C $C7AC,3 Helper: Y position += Y velocity
C $C7B6,1 Read the 2-byte absolute-position operand
C $C7C2,1 Read the 1-byte screen-row operand
C $C7CA,1 Read 3 operand bytes: velocity/counter pair for an
C $C7CB,1 accelerate/decelerate opcode (order differs from
C $C7CC,3 the $CA/$CB variant below)
C $C7DC,1 Read 3 operand bytes for the $CA/$CB decelerate
C $C7DD,1 opcodes
@ $C7ED label=oss_op_decel_x
C $C7ED,3 Decelerate X: apply X step, look up the next speed
C $C7F0,3 from #R$D272 (indexed by the countdown value),
C $C7F3,3 negate it and add to Y velocity, then advance the
C $C7F6,2 countdown
@ $C804 label=oss_lookup_speed
C $C804,1 Helper: speed = #R$D272[(B<<8 | C)] >> 2 -- decodes
C $C805,2 a countdown value C into a velocity magnitude via
C $C807,3 the shared deceleration curve table
@ $C812 label=oss_op_decel_y
C $C812,3 Decelerate Y (mirror of #R$C7ED for the Y axis)
@ $C827 label=oss_op_accel_x_a
C $C827,3 Accelerate/decelerate X variant (countdown-driven,
C $C82A,3 negated speed, counts down)
@ $C83E label=oss_op_accel_x_b
C $C83E,3 Accelerate/decelerate X variant (countdown-driven,
C $C841,3 positive speed, counts down)
@ $C853 label=oss_op_accel_x_c
C $C853,3 Accelerate/decelerate X variant (countdown-driven,
C $C856,3 positive speed, counts up)
@ $C868 label=oss_op_immediate_step
C $C868,1 Immediate 2-axis step opcode ($00-$7F): the low 3
C $C869,2 bits of each nibble-pair select a step magnitude
C $C86B,2 (0-3, doubled via SLA) and a sign bit chooses the
C $C86D,2 direction for X (bits 0-2) and Y (bits 4-6, tested
C $C86F,2 via BIT 5) independently
N $C890 Clear the screen bitmap ($4000-$57FF) and attribute area ($5800-$59FF) to zero. This entry point is used by the routine at #R$C06E.
@ $C890 label=clear_screen_bitmap_and_attrs
C $C890,3 Clear attributes $5900-$5AFF (i.e. from the bottom
C $C893,3 up), then... (Conv note for later C port: this
C $C896,1 range actually only needs $5800-$59FF -- treat as
C $C897,3 a plain memset)
C $C89C,3 ...clear the bitmap $4800-$57FF (leaving $4000-
C $C89F,3 $47FF, the top screen third, untouched here)
N $C8A9 Clear the bitmap/attributes (#R$C890) then set the border attribute rows ($5900-$59FF) to a fixed pattern: black border rows top/bottom (2 bytes each) sandwiching 28 rows of attribute $45 (flashing? -- bit 7 set, paper/ink from bits 0-5) per attribute-cell column, repeated across all 16 columns.
@ $C8A9 label=clear_and_fill_border_attrs
C $C8AF,2 16 columns
C $C8B2,1 2 black rows
C $C8B6,2 28 rows of attribute $45
C $C8BD,1 2 more black rows
N $C8C5 Compute the screen address and masked-sprite blitter parameters for a single title-screen glyph/object, then dispatch to a width-specific unrolled OR-blit routine (#R$C917 onward) which draws it into the playfield bitmap. Called (via EXX-banked BC/L, see #R$C6D0) once per foreground object per frame. B = object Y screen position (+$08, clamped to a max of $6F -- the overflow past $6F is banked via #R$C8CD and used later as a row-skip count), C = object X screen position (+$07), L = the object's screen row/height byte (+$06, set by the script's $C8 opcode). #R$C8C5- #R$C8CC clamp B; #R$C8CD-#R$C8EC then build a masked-sprite screen destination address in D/E from B and C, using the same "rotate through carry, XOR, mask $F8, XOR" idiom the ROM uses to interleave a pixel row into the non-linear screen-third layout (not traced bit- exactly here: getting each rotate/XOR wrong silently corrupts only the row within a third, which is easy to get subtly wrong without emulator verification -- flag for later verification against a working build). #R$C8ED-#R$C8FE then use C (object X) and L (screen row/height byte) to index the 4-byte-per-entry glyph table at #R$D296 (mask width, byte-count, source address), giving B/C/HL = mask width, byte count, and source glyph address. #R$C902 restores the row-clamp flag banked at #R$C8CD: if set, the source address is used as-is; otherwise #R$C906-#R$C916 (cgb_row_offset_loop) walks the source pointer forward row-by-row (stride = C<<1, i.e. mask-width*2 bytes/row) for the overflow-past-$6F row count, before falling into the blit dispatch at #R$C917.
@ $C8C5 label=compute_glyph_blit_params
C $C8FA,1 B/C/HL = mask width, byte-count, source address
C $C8FB,1 from the #R$D296 glyph table entry
C $C902,1 Restore the flag from the earlier RRA (row clamp)
C $C903,3 -- if set, skip straight to the blit dispatch
@ $C906 label=cgb_row_offset_loop
C $C906,1 Otherwise walk the source pointer forward by one
C $C907,1 row at a time (not traced bit-for-bit here) until
C $C908,2 the row-clamp countdown reaches zero
N $C917 Masked-sprite blit dispatch: HL/DE hold the destination screen address (from the caller), the popped HL/DE here hold the source glyph address and its mask width in C. Dispatches to one of 6 width-specific unrolled OR-blit routines (#R$C9D5 .. #R$CBC5) which draw the glyph into the bitmap by treating the source address as a stack pointer (LD SP,HL) and POPping pixel/mask byte pairs -- see the project's known "LD SP,HL; POP x N sprite copy" translation pitfall. The real SP is saved at $C93D and restored by each blit routine (or by #R$C93C on early abort) before returning.
@ $C917 label=blit_masked_sprite_dispatch
C $C918,4 Save the real stack pointer
C $C91C,1 SP now points at the glyph source data
C $C91D,1 HL = destination screen address
@ $C935 label=cgb_row_offset_done
C $C936,2 Small fixed delay (not blitting this frame --
C $C938,2 object scrolled fully off, nothing to draw)
N $C93C Shared "abort the blit early" tail: restores the real SP (saved at $C93D by #R$C917/#R$C9AF) and returns to the object-draw loop.
@ $C93C label=blit_abort_restore_sp
C $C93C,3 (operand self-modified by #R$C918/#R$C9B4)
@ $C940 label=cgb_delay_tail
N $C94F Second glyph blit-parameter computation, structurally identical to #R$C8C5 above but feeding the alternate dispatch table at #R$C9AF. Used for the 3 "background" objects (see #R$C6F0).
@ $C94F label=compute_glyph_blit_params_b
@ $C990 label=cgbb_row_offset_loop
N $C9AF Alternate masked-sprite blit dispatch used by the background-object draw path (#R$C6F0 via #R$C94F). Identical width-selection logic to #R$C917, but preserves B (via an extra push/pop) as a small fixed delay count, and its early-exit tail (#R$C9D3) simply restores DE rather than aborting via #R$C93C.
@ $C9AF label=blit_masked_sprite_dispatch_b
N $C9D5 Width-1 (mask-only, no fill byte) unrolled OR-blit. #R$C917/#R$C9AF dispatch here when the width selector (C, decremented to 0) picks this variant. Draws 2 scanlines per POP DE: E blits into the current HL byte (LD A,(HL) / OR E / LD (HL),A -- a masked, non-overwriting sprite draw), then INC H steps to the next screen row; if that crosses a third boundary (H AND $07 = 0), the SUB $08 / ADD A,$20 / carry-adjust pattern at #R$C9F3 fixes up H/L, otherwise the row falls straight through. D then blits the same way into the following row, with its own boundary fix-up at #R$CA05. A short fixed delay (#R$C9E8, a 30-iteration DEC-A loop) pads out the timing before DJNZ repeats for the next row-pair (B counts row-pairs); once B reaches 0, execution falls into #R$C93C, which restores the real SP and returns.
@ $C9D5 label=blit_width1
N $CA17 Width-2 unrolled OR-blit variant (see #R$C9D5 for the general pattern).
@ $CA17 label=blit_width2
N $CA64 Width-3 unrolled OR-blit variant (see #R$C9D5 for the general pattern).
@ $CA64 label=blit_width3
N $CABC Width-4 unrolled OR-blit variant (see #R$C9D5 for the general pattern).
@ $CABC label=blit_width4
N $CB17 Width-5 unrolled OR-blit variant (see #R$C9D5 for the general pattern).
@ $CB17 label=blit_width5
N $CB7B Width-6 unrolled OR-blit variant (see #R$C9D5 for the general pattern).
@ $CB7B label=blit_width6
N $CBC5 Width-7 unrolled OR-blit variant (see #R$C9D5 for the general pattern); the widest/final entry in the dispatch chain, reached when C reaches 0 without matching any of the DEC C; JP Z tests above.
@ $CBC5 label=blit_width7
C $CBDA,3 Restore the real SP (saved by #R$C917/#R$C9AF)
N $CC04 Clear the playfield bitmap ($4800-$57FF, i.e. the lower two screen thirds, excluding the top third at $4000-$47FF which holds the fixed title/credits text) to zero, ready for the next frame's objects. Classic "LD SP,HL; PUSH x N" fast-fill trick (see the project's known translation pitfall of the same name): SP is repointed at the bitmap, then 14 PUSH DE instructions fill 28 bytes per pass, DJNZ-looped 8 times per screen-third row (C = 8, giving 8 x 28 = 224 bytes per call through the inner loop, i.e. one attribute-row's worth), stepping through third-boundary wraps via the usual ADD A,$20 / carry pattern. The real SP is saved at $CC4D and restored before returning.
@ $CC04 label=clear_playfield_buffer
C $CC04,4 Save the real stack pointer
@ $CC10 label=cpb_third1_row_loop
@ $CC11 label=cpb_third1_fill
C $CC2C,2 Move on to the second screen third ($5000-$57FF)
@ $CC2E label=cpb_third2_row_loop
@ $CC2F label=cpb_third2_fill
C $CC4C,3 Restore the real stack pointer (operand self-
C $CC4F,1 modified by $CC04) and return
b $CC50 Title-screen text, scene/object tables and script byte-code
D $CC50 Four position-encoded text strings (2 position bytes, 1 attribute byte, ASCII text terminated by a character with bit 7 set -- same encoding as the high-score text at #R$C3AF): "(C) 1989 OCEAN SOFTWARE", "(C) 1988 TAITO CORPORATION" (Chase H.Q. was originally a Taito arcade game, licensed to Ocean for the home computer ports), "PRESS GEAR TO PLAY" and "PRESS ENTER FOR OPTIONS" (drawn by #R$C608/#R$C615 via #R$FDA4). Text ends at $CCB6.
R $CC50 From $CCB7: 5 scene definition tables, one selected per title-screen
R $CC50 call by #R$C5AC-#R$C5C4 ($CCB7, $CD4F, $CF10, $CFCD, $D16C). Each table
R $CC50 holds 9 x 5-byte object records (initial X, initial Y, initial screen-
R $CC50 row byte, script pointer low, script pointer high -- read by the loop
R $CC50 at #R$C5E5) followed immediately by that scene's pool of object
R $CC50 animation scripts, addressed by the pointers in the records above.
N $CC50 Script byte-code is consumed by the interpreter at #R$C705/#R$C740: bytes with bit 7 clear are immediate 2-axis step opcodes (#R$C868); bytes $C8-$D1 select a movement mode (set row, set velocity, accelerate/decelerate via the #R$D272 table, or jump to an absolute position) with further operand bytes following in the stream; $D2 ends the script. The raw bytes below are not decoded byte-by-byte per scene here -- that would mean reproducing every scene's animation script inline -- but the full opcode set they are built from is documented at #R$C740/#R$C746 for anyone tracing an individual scene's animation. At $D272: a 36-byte monotonically-increasing speed/deceleration curve table, indexed (via #R$C804) by a 0-35 countdown value to produce a velocity magnitude for the decelerate/accelerate script opcodes.
B $CC50,1606,8*200,6
b $D296 Character graphic data table
@ $D296 label=glyphs
B $D296,1,1 Height/2
B $D297,1,1 Width in bytes
W $D298,2,2 Bitmap pointer
B $D29A,1,1 Height/2
B $D29B,1,1 Width in bytes
W $D29C,2,2 Bitmap pointer
B $D29E,1,1 Height/2
B $D29F,1,1 Width in bytes
W $D2A0,2,2 Bitmap pointer
B $D2A2,1,1 Height/2
B $D2A3,1,1 Width in bytes
W $D2A4,2,2 Bitmap pointer
B $D2A6,1,1 Height/2
B $D2A7,1,1 Width in bytes
W $D2A8,2,2 Bitmap pointer
B $D2AA,1,1 Height/2
B $D2AB,1,1 Width in bytes
W $D2AC,2,2 Bitmap pointer
B $D2AE,1,1 Height/2
B $D2AF,1,1 Width in bytes
W $D2B0,2,2 Bitmap pointer
B $D2B2,1,1 Height/2
B $D2B3,1,1 Width in bytes
W $D2B4,2,2 Bitmap pointer
B $D2B6,1,1 Height/2
B $D2B7,1,1 Width in bytes
W $D2B8,2,2 Bitmap pointer
B $D2BA,1,1 Height/2
B $D2BB,1,1 Width in bytes
W $D2BC,2,2 Bitmap pointer
B $D2BE,1,1 Height/2
B $D2BF,1,1 Width in bytes
W $D2C0,2,2 Bitmap pointer
B $D2C2,1,1 Height/2
B $D2C3,1,1 Width in bytes
W $D2C4,2,2 Bitmap pointer
B $D2C6,1,1 Height/2
B $D2C7,1,1 Width in bytes
W $D2C8,2,2 Bitmap pointer
B $D2CA,1,1 Height/2
B $D2CB,1,1 Width in bytes
W $D2CC,2,2 Bitmap pointer
B $D2CE,1,1 Height/2
B $D2CF,1,1 Width in bytes
W $D2D0,2,2 Bitmap pointer
B $D2D2,1,1 Height/2
B $D2D3,1,1 Width in bytes
W $D2D4,2,2 Bitmap pointer
B $D2D6,1,1 Height/2
B $D2D7,1,1 Width in bytes
W $D2D8,2,2 Bitmap pointer
B $D2DA,1,1 Height/2
B $D2DB,1,1 Width in bytes
W $D2DC,2,2 Bitmap pointer
B $D2DE,1,1 Height/2
B $D2DF,1,1 Width in bytes
W $D2E0,2,2 Bitmap pointer
B $D2E2,1,1 Height/2
B $D2E3,1,1 Width in bytes
W $D2E4,2,2 Bitmap pointer
B $D2E6,1,1 Height/2
B $D2E7,1,1 Width in bytes
W $D2E8,2,2 Bitmap pointer
B $D2EA,1,1 Height/2
B $D2EB,1,1 Width in bytes
W $D2EC,2,2 Bitmap pointer
B $D2EE,1,1 Height/2
B $D2EF,1,1 Width in bytes
W $D2F0,2,2 Bitmap pointer
B $D2F2,1,1 Height/2
B $D2F3,1,1 Width in bytes
W $D2F4,2,2 Bitmap pointer
B $D2F6,1,1 Height/2
B $D2F7,1,1 Width in bytes
W $D2F8,2,2 Bitmap pointer
B $D2FA,1,1 Height/2
B $D2FB,1,1 Width in bytes
W $D2FC,2,2 Bitmap pointer
B $D2FE,1,1 Height/2
B $D2FF,1,1 Width in bytes
W $D300,2,2 Bitmap pointer
B $D302,1,1 Height/2
B $D303,1,1 Width in bytes
W $D304,2,2 Bitmap pointer
B $D306,1,1 Height/2
B $D307,1,1 Width in bytes
W $D308,2,2 Bitmap pointer
B $D30A,1,1 Height/2
B $D30B,1,1 Width in bytes
W $D30C,2,2 Bitmap pointer
B $D30E,1,1 Height/2
B $D30F,1,1 Width in bytes
W $D310,2,2 Bitmap pointer
B $D312,1,1 Height/2
B $D313,1,1 Width in bytes
W $D314,2,2 Bitmap pointer
B $D316,1,1 Height/2
B $D317,1,1 Width in bytes
W $D318,2,2 Bitmap pointer
B $D31A,1,1 Height/2
B $D31B,1,1 Width in bytes
W $D31C,2,2 Bitmap pointer
B $D31E,1,1 Height/2
B $D31F,1,1 Width in bytes
W $D320,2,2 Bitmap pointer
B $D322,1,1 Height/2
B $D323,1,1 Width in bytes
W $D324,2,2 Bitmap pointer
B $D326,1,1 Height/2
B $D327,1,1 Width in bytes
W $D328,2,2 Bitmap pointer
B $D32A,1,1 Height/2
B $D32B,1,1 Width in bytes
W $D32C,2,2 Bitmap pointer
B $D32E,1,1 Height/2
B $D32F,1,1 Width in bytes
W $D330,2,2 Bitmap pointer
B $D332,1,1 Height/2
B $D333,1,1 Width in bytes
W $D334,2,2 Bitmap pointer
B $D336,1,1 Height/2
B $D337,1,1 Width in bytes
W $D338,2,2 Bitmap pointer
B $D33A,1,1 Height/2
B $D33B,1,1 Width in bytes
W $D33C,2,2 Bitmap pointer
B $D33E,1,1 Height/2
B $D33F,1,1 Width in bytes
W $D340,2,2 Bitmap pointer
B $D342,1,1 Height/2
B $D343,1,1 Width in bytes
W $D344,2,2 Bitmap pointer
B $D346,1,1 Height/2
B $D347,1,1 Width in bytes
W $D348,2,2 Bitmap pointer
B $D34A,1,1 Height/2
B $D34B,1,1 Width in bytes
W $D34C,2,2 Bitmap pointer
B $D34E,1,1 Height/2
B $D34F,1,1 Width in bytes
W $D350,2,2 Bitmap pointer
B $D352,1,1 Height/2
B $D353,1,1 Width in bytes
W $D354,2,2 Bitmap pointer
B $D356,1,1 Height/2
B $D357,1,1 Width in bytes
W $D358,2,2 Bitmap pointer
B $D35A,1,1 Height/2
B $D35B,1,1 Width in bytes
W $D35C,2,2 Bitmap pointer
B $D35E,1,1 Height/2
B $D35F,1,1 Width in bytes
W $D360,2,2 Bitmap pointer
B $D362,1,1 Height/2
B $D363,1,1 Width in bytes
W $D364,2,2 Bitmap pointer
B $D366,1,1 Height/2
B $D367,1,1 Width in bytes
W $D368,2,2 Bitmap pointer
B $D36A,1,1 Height/2
B $D36B,1,1 Width in bytes
W $D36C,2,2 Bitmap pointer
B $D36E,1,1 Height/2
B $D36F,1,1 Width in bytes
W $D370,2,2 Bitmap pointer
B $D372,1,1 Height/2
B $D373,1,1 Width in bytes
W $D374,2,2 Bitmap pointer
B $D376,1,1 Height/2
B $D377,1,1 Width in bytes
W $D378,2,2 Bitmap pointer
B $D37A,1,1 Height/2
B $D37B,1,1 Width in bytes
W $D37C,2,2 Bitmap pointer
B $D37E,1,1 Height/2
B $D37F,1,1 Width in bytes
W $D380,2,2 Bitmap pointer
B $D382,1,1 Height/2
B $D383,1,1 Width in bytes
W $D384,2,2 Bitmap pointer
B $D386,1,1 Height/2
B $D387,1,1 Width in bytes
W $D388,2,2 Bitmap pointer
B $D38A,1,1 Height/2
B $D38B,1,1 Width in bytes
W $D38C,2,2 Bitmap pointer
B $D38E,1,1 Height/2
B $D38F,1,1 Width in bytes
W $D390,2,2 Bitmap pointer
B $D392,1,1 Height/2
B $D393,1,1 Width in bytes
W $D394,2,2 Bitmap pointer
B $D396,1,1 Height/2
B $D397,1,1 Width in bytes
W $D398,2,2 Bitmap pointer
B $D39A,1,1 Height/2
B $D39B,1,1 Width in bytes
W $D39C,2,2 Bitmap pointer
B $D39E,1,1 Height/2
B $D39F,1,1 Width in bytes
W $D3A0,2,2 Bitmap pointer
B $D3A2,1,1 Height/2
B $D3A3,1,1 Width in bytes
W $D3A4,2,2 Bitmap pointer
B $D3A6,1,1 Height/2
B $D3A7,1,1 Width in bytes
W $D3A8,2,2 Bitmap pointer
B $D3AA,1,1 Height/2
B $D3AB,1,1 Width in bytes
W $D3AC,2,2 Bitmap pointer
B $D3AE,1,1 Height/2
B $D3AF,1,1 Width in bytes
W $D3B0,2,2 Bitmap pointer
B $D3B2,1,1 Height/2
B $D3B3,1,1 Width in bytes
W $D3B4,2,2 Bitmap pointer
B $D3B6,1,1 Height/2
B $D3B7,1,1 Width in bytes
W $D3B8,2,2 Bitmap pointer
B $D3BA,1,1 Height/2
B $D3BB,1,1 Width in bytes
W $D3BC,2,2 Bitmap pointer
B $D3BE,1,1 Height/2
B $D3BF,1,1 Width in bytes
W $D3C0,2,2 Bitmap pointer
B $D3C2,1,1 Height/2
B $D3C3,1,1 Width in bytes
W $D3C4,2,2 Bitmap pointer
B $D3C6,1,1 Height/2
B $D3C7,1,1 Width in bytes
W $D3C8,2,2 Bitmap pointer
B $D3CA,1,1 Height/2
B $D3CB,1,1 Width in bytes
W $D3CC,2,2 Bitmap pointer
B $D3CE,1,1 Height/2
B $D3CF,1,1 Width in bytes
W $D3D0,2,2 Bitmap pointer
B $D3D2,1,1 Height/2
B $D3D3,1,1 Width in bytes
W $D3D4,2,2 Bitmap pointer
B $D3D6,1,1 Height/2
B $D3D7,1,1 Width in bytes
W $D3D8,2,2 Bitmap pointer
B $D3DA,1,1 Height/2
B $D3DB,1,1 Width in bytes
W $D3DC,2,2 Bitmap pointer
B $D3DE,1,1 Height/2
B $D3DF,1,1 Width in bytes
W $D3E0,2,2 Bitmap pointer
B $D3E2,1,1 Height/2
B $D3E3,1,1 Width in bytes
W $D3E4,2,2 Bitmap pointer
B $D3E6,1,1 Height/2
B $D3E7,1,1 Width in bytes
W $D3E8,2,2 Bitmap pointer
B $D3EA,1,1 Height/2
B $D3EB,1,1 Width in bytes
W $D3EC,2,2 Bitmap pointer
B $D3EE,1,1 Height/2
B $D3EF,1,1 Width in bytes
W $D3F0,2,2 Bitmap pointer
B $D3F2,1,1 Height/2
B $D3F3,1,1 Width in bytes
W $D3F4,2,2 Bitmap pointer
B $D3F6,1,1 Height/2
B $D3F7,1,1 Width in bytes
W $D3F8,2,2 Bitmap pointer
B $D3FA,1,1 Height/2
B $D3FB,1,1 Width in bytes
W $D3FC,2,2 Bitmap pointer
B $D3FE,1,1 Height/2
B $D3FF,1,1 Width in bytes
W $D400,2,2 Bitmap pointer
B $D402,1,1 Height/2
B $D403,1,1 Width in bytes
W $D404,2,2 Bitmap pointer
B $D406,1,1 Height/2
B $D407,1,1 Width in bytes
W $D408,2,2 Bitmap pointer
B $D40A,1,1 Height/2
B $D40B,1,1 Width in bytes
W $D40C,2,2 Bitmap pointer
B $D40E,1,1 Height/2
B $D40F,1,1 Width in bytes
W $D410,2,2 Bitmap pointer
B $D412,1,1 Height/2
B $D413,1,1 Width in bytes
W $D414,2,2 Bitmap pointer
B $D416,1,1 Height/2
B $D417,1,1 Width in bytes
W $D418,2,2 Bitmap pointer
B $D41A,1,1 Height/2
B $D41B,1,1 Width in bytes
W $D41C,2,2 Bitmap pointer
B $D41E,1,1 Height/2
B $D41F,1,1 Width in bytes
W $D420,2,2 Bitmap pointer
B $D422,1,1 Height/2
B $D423,1,1 Width in bytes
W $D424,2,2 Bitmap pointer
B $D426,1,1 Height/2
B $D427,1,1 Width in bytes
W $D428,2,2 Bitmap pointer
B $D42A,1,1 Height/2
B $D42B,1,1 Width in bytes
W $D42C,2,2 Bitmap pointer
B $D42E,1,1 Height/2
B $D42F,1,1 Width in bytes
W $D430,2,2 Bitmap pointer
B $D432,1,1 Height/2
B $D433,1,1 Width in bytes
W $D434,2,2 Bitmap pointer
B $D436,1,1 Height/2
B $D437,1,1 Width in bytes
W $D438,2,2 Bitmap pointer
B $D43A,1,1 Height/2
B $D43B,1,1 Width in bytes
W $D43C,2,2 Bitmap pointer
B $D43E,1,1 Height/2
B $D43F,1,1 Width in bytes
W $D440,2,2 Bitmap pointer
B $D442,1,1 Height/2
B $D443,1,1 Width in bytes
W $D444,2,2 Bitmap pointer
B $D446,1,1 Height/2
B $D447,1,1 Width in bytes
W $D448,2,2 Bitmap pointer
B $D44A,1,1 Height/2
B $D44B,1,1 Width in bytes
W $D44C,2,2 Bitmap pointer
B $D44E,1,1 Height/2
B $D44F,1,1 Width in bytes
W $D450,2,2 Bitmap pointer
B $D452,1,1 Height/2
B $D453,1,1 Width in bytes
W $D454,2,2 Bitmap pointer
B $D456,1,1 Height/2
B $D457,1,1 Width in bytes
W $D458,2,2 Bitmap pointer
B $D45A,1,1 Height/2
B $D45B,1,1 Width in bytes
W $D45C,2,2 Bitmap pointer
B $D45E,1,1 Height/2
B $D45F,1,1 Width in bytes
W $D460,2,2 Bitmap pointer
b $D462 Graphics
B $D462,112,4 Bitmap: "C" 32x28
B $D4D2,112,4 Bitmap: "C" 32x28 shifted right 4?
B $D542,112,4 Bitmap: "C" 32x28 shifted right 8?
B $D5B2,140,5 Bitmap: "C" 40x28
B $D63E,112,4 Bitmap: "H" 32x28
B $D6AE,112,4 Bitmap: "H" 32x28 shifted right
B $D71E,140,5 Bitmap: "H" 40x28
B $D7AA,140,5 Bitmap: "H" 40x28 shifted right
B $D836,84,3 Bitmap: "A" 24x28
B $D88A,84,3 Bitmap: "A" 24x28 shifted right
B $D8DE,112,4 Bitmap: "A" 32x28
B $D94E,112,4 Bitmap: "A" 32x28 shifted right
B $D9BE,112,4 Bitmap: "S" 32x28
B $DA2E,112,4 Bitmap: "S" 32x28 shifted right
B $DA9E,112,4 Bitmap: "S" 32x28 shifted right again
B $DB0E,140,5 Bitmap: "S" 40x28
B $DB9A,112,5*22,2 Bitmap: "E" 32x28
B $DC0A,112,5*22,2 Bitmap: "E" 32x28 shifted right
B $DC7A,112,5*22,2 Bitmap: "E" 32x28 shifted right again
B $DCEA,112,5*22,2 Bitmap: "E" 32x28 shifted right again again
B $DD5A,128,4 Bitmap: "Q" 32x32
B $DDDA,128,4 Bitmap: "Q" 32x32 shifted right
B $DE5A,128,4 Bitmap: "Q" 32x32 shifted right again
B $DEDA,160,5 Bitmap: "Q" 40x32
B $DF7A,16,2 Bitmap: "." 16x8
B $DF8A,16,2 Bitmap: "." 16x8
B $DF9A,16,2 Bitmap: "." 16x8
B $DFAA,16,2 Bitmap: "." 16x8
B $DFBA,78,3 Bitmap: "A" 24x26
B $E008,80,8 Bitmap
B $E058,72,8 Bitmap
B $E0A0,60,8*7,4 Bitmap
B $E0DC,60,8*7,4 Bitmap
B $E118,80,8 Bitmap
B $E168,80,8 Bitmap
B $E1B8,28,8*3,4 Bitmap
B $E1D4,28,8*3,4 Bitmap
B $E1F0,28,8*3,4 Bitmap
B $E20C,42,8*5,2 Bitmap
B $E236,8,8 Bitmap
B $E23E,16,8 Bitmap
B $E24E,16,8 Bitmap
B $E25E,16,8 Bitmap
B $E26E,60,8*7,4 Bitmap
B $E2AA,60,8*7,4 Bitmap
B $E2E6,80,8 Bitmap
B $E336,80,8 Bitmap
B $E386,28,8*3,4 Bitmap
B $E3A2,28,8*3,4 Bitmap
B $E3BE,42,8*5,2 Bitmap
B $E3E8,42,8*5,2 Bitmap
B $E412,8,8 Bitmap
B $E41A,16,8 Bitmap
B $E42A,16,8 Bitmap
B $E43A,16,8 Bitmap
B $E44A,40,8 Bitmap
B $E472,60,8*7,4 Bitmap
B $E4AE,60,8*7,4 Bitmap
B $E4EA,60,8*7,4 Bitmap
B $E526,28,8*3,4 Bitmap
B $E542,28,8*3,4 Bitmap
B $E55E,28,8*3,4 Bitmap
B $E57A,28,8*3,4 Bitmap
B $E596,8,8 Bitmap
B $E59E,8,8 Bitmap
B $E5A6,16,8 Bitmap
B $E5B6,16,8 Bitmap
B $E5C6,60,8*7,4 Bitmap
B $E602,60,8*7,4 Bitmap
B $E63E,80,8 Bitmap
B $E68E,80,8 Bitmap
B $E6DE,28,8*3,4 Bitmap
B $E6FA,28,8*3,4 Bitmap
B $E716,28,8*3,4 Bitmap
B $E732,42,8*5,2 Bitmap
B $E75C,8,8 Bitmap
B $E764,16,8 Bitmap
B $E774,16,8 Bitmap
B $E784,16,8 Bitmap
B $E794,60,8*7,4 Bitmap
B $E7D0,60,8*7,4 Bitmap
B $E80C,60,8*7,4 Bitmap
B $E848,60,8*7,4 Bitmap
B $E884,28,8*3,4 Bitmap
B $E8A0,28,8*3,4 Bitmap
B $E8BC,28,8*3,4 Bitmap
B $E8D8,42,8*5,2 Bitmap
B $E902,8,8 Bitmap
B $E90A,8,8 Bitmap
B $E912,16,8 Bitmap
B $E922,16,8 Bitmap
B $E932,72,8 Bitmap
B $E97A,72,8 Bitmap
B $E9C2,72,8 Bitmap
B $EA0A,96,8 Bitmap
B $EA6A,32,8 Bitmap
B $EA8A,32,8 Bitmap
B $EAAA,48,8 Bitmap
B $EADA,48,8 Bitmap
B $EB0A,10,8,2 Bitmap
B $EB14,20,8*2,4 Bitmap
B $EB28,20,8*2,4 Bitmap
B $EB3C,20,8*2,4 Bitmap
B $EB50,6,6 Bitmap
B $EB56,12,8,4 Bitmap
B $EB62,12,8,4 Bitmap
B $EB6E,12,8,4 Bitmap
B $EB7A,4,4 Bitmap
B $EB7E,4,4 Bitmap
B $EB82,8,8 Bitmap
B $EB8A,8,8 Bitmap
B $EB92,2,2 Bitmap
B $EB94,2,2 Bitmap
B $EB96,4,4 Bitmap
B $EB9A,4,4 Bitmap
c $EB9E Start playing a tune (AY-3-8912 music driver)
D $EB9E A = tune number. Looks up the tune's 7-byte entry (index = A*7, via the ADD A,A / ADD A,C doubling sequence at #R$EBA7-#R$EBAB) in the table at #R$F225: 1 tempo/speed byte followed by 3 x 2-byte pattern-data pointers, one per channel. Uses that entry to initialise the 3 channel-tracker records at $EC01/$EC26/$EC4B (37 bytes each, stride $25 -- offsets used elsewhere in this sound driver: +$00 note/status, +$01/+$02 pattern pointer, +$03/+$04 envelope or effect pointer, +$05 initial speed, +$06 counter, +$10 enable flag, +$1D/+$1F/+$20/+$21 misc playback state) before flagging the tune active via #R$F223 for the per-frame service routine at #R$EC71.
R $EB9E The per-channel effect/envelope pointer (+$01/+$02) is not read from
R $EB9E the tune table directly -- it is read from the first 2 bytes of the
R $EB9E pattern data that the channel's own pattern pointer (+$03/+$04) points
R $EB9E to (#R$EBDF-$EBE2), i.e. every pattern begins with an envelope-pointer
R $EB9E header. #R$EDD6 and #R$EE9E (referenced by #R$EC71 below) do the
R $EB9E actual per-frame pattern-data processing.
N $EB9E Used by the routine at #R$F7D6.
@ $EB9E label=start_tune
C $EB9E,3 Clear the "tune active" flag and its companion
C $EBA1,2 byte at $F224
C $EBA6,1 BC = A * 7 (2A, +A = 3A, doubled = 6A, +A = 7A):
C $EBA7,1 the $F225 table's 7-byte-per-tune entry stride
C $EBAE,3 HL -> this tune's entry in the tune-select table
C $EBB2,1 First byte = tempo/speed, saved for later use
C $EBB7,4 IX -> first of the 3 channel-tracker records
C $EBBB,2 Record stride (37 bytes)
C $EBBD,2 3 channels
@ $EBBF label=stu_channel_loop
C $EBBF,1 Read this channel's pattern-data pointer from the
C $EBC0,1 tune table
C $EBC4,4 Reset misc playback state for this channel
C $EBCC,4 Enable the channel
C $EBD0,3 B = 0 here: clear note/status
C $EBD9,3 Store the pattern pointer
C $EBDF,1 Follow the pattern pointer to read a second,
C $EBE0,1 effect/envelope pointer from the start of the
C $EBE1,1 pattern data itself
C $EBE3,4 Initial speed/divider = 2
C $EBE7,3 Counter = 0
C $EBEB,3 Store the effect/envelope pointer
C $EBF1,2 Next channel record
C $EBF6,3 A = 0 here: clear a driver-internal flag
C $EBF9,1 A = 1: mark the tempo counter for an immediate
C $EBFA,3 refresh (see #R$EC71)
C $EBFD,3 Flag the tune as active
b $EC01 The 3 channel-tracker records (37 bytes each) used by the music driver, initialised by #R$EB9E and processed each frame by #R$EC71.
B $EC01,112,8
c $EC71 Per-frame music service. If no tune is active, does nothing (falls straight through to #R$ECCA which returns immediately). Otherwise: decrements the tempo counter (#R$EC70) and, once it reaches zero, re-processes all 3 channel-tracker records (#R$EDD6) one tracker "row" further and resets the counter; then (every frame, tick or not) recomputes the AY tone-period/volume register values for all 3 channels from their current tracker state (#R$EE9E) into the register cache at #R$EFAF-$EFB9; finally outputs the full cached register block ($EFBA onward, registers 11 down to 0) to the AY chip via ports $FFFD/$BFFD.
D $EC71 Used by the routine at #R$F82F.
@ $EC71 label=ts_music_service
C $EC71,3 Tune-active flag
C $EC78,2 Clear a driver-internal flag (consumed elsewhere
C $EC7A,3 in the pattern processing, not traced here)
C $EC7D,3 Decrement the tempo counter; only re-process the
C $EC80,1 tracker patterns when it reaches zero
C $EC83,1 B = 0 (post-decrement)
C $EC84,4 Advance channel 1's pattern by one tracker row
C $EC8B,4 Channel 2
C $EC92,4 Channel 3
C $EC99,2 Reset the tempo counter (Conv note: reset to a
C $EC9B,3 fixed 1, not the tune's stored tempo at $EC9A -- appears to always tick every other frame)
@ $EC9E label=tms_refresh_registers
C $EC9E,4 Recompute the AY register values for all 3
C $ECA2,3 channels from their current tracker state
C $ECC7,3 Clear the cached mixer/noise register byte
@ $ECCA label=tms_output_registers
C $ECCA,3 Re-check the tune-active flag: if no tune is
C $ECCD,1 playing, do nothing further (registers are only
C $ECCE,1 ever written while a tune is active)
C $ECCF,3 Output the cached register block: 12 registers
C $ECD2,2 (11 downto 0), each selected via port $FFFD then
C $ECD4,2 its data byte written via port $BFFD
@ $ECD6 label=tms_output_loop
C $ECD8,2 Select AY register A
C $ECDC,2 Write (HL) to the data port, HL--
C $ECDE,1 Next (lower) register
N $ECE3 Helper: write a single AY register (D = register number, E = value). Not directly referenced elsewhere in this bank -- may be dead code or called from another bank.
N $ECED Silence the AY chip: write 0 to every register 13 downto 0 (in particular the 3 volume registers 8-10), then write register 7 (mixer) a second time for good measure.
@ $ECED label=ay_silence_all
C $ECEF,3 D = register 13 (downto 0), E = 0 (dummy)
C $ECF2,1 A = 0 (the value written to every register)
@ $ECF3 label=asa_reg_loop
C $ECF5,2 Select register D
C $ECF9,2 Write 0
C $ECFF,3 Belt-and-braces: rewrite register 7 (mixer) = 0
N $ED0B Stop any playing tune and silence the AY chip. This entry point is used by the routines at #R$C16A, #R$C59E, #R$F7D6 and #R$FBC8.
@ $ED0B label=stop_music_and_silence
C $ED0C,3 Clear the tune-active flag
C $ED12,1 Clear the per-channel mixer/noise register cache
N $ED1D Not traced (structural pass) -- nothing in this bank jumps or calls into $ED1D-$ED32; the instruction stream here does not obviously cohere (an SBC/ADD/ADC/SUB chain feeding register loads with no clear purpose before falling into a POP HL/JP). Likely either dead code or a misdisassembled data table; revisit if $F1AE below turns out to need it.
N $ED36 Pattern command/effect opcode handlers ($ED36-$EDD1), reached only via the computed jump at #R$EE96 (never by a direct CALL/JP visible to the disassembler, hence no entry-point markers below). Each handler consumes a fixed number of further bytes from the pattern stream (DE) and stores them into fields of the current channel's 37-byte tracker record (see #R$EC01), then rejoins the main interpreter at #R$EDE4 or #R$EE22. Traced mechanically below; the AY-level meaning of $EF7A and +$1D/+$1F is inferred (marked "likely"), not confirmed against a working build. Handlers 1-3 all bit-merge P = (IX+$24) into the shared byte $EF7A using the "A XOR M; A AND mask; A XOR M" replace-bits-under-mask idiom; $EF7A is likely a per-channel AY mixer (tone/noise enable) bit cache, given #R$EEA0 onward outputs a mixer byte to the AY.
@ $ED36 label=pcmd_set_mixer_bits_low3
C $ED36,3 Merge (P AND $07) into $EF7A under mask P; likely
C $ED39,1 sets this channel's tone-enable bit(s)
C $ED43,2 Set +$1E (slide/portamento-active flag) = 1
@ $ED4B label=pcmd_set_mixer_bits_high3
C $ED4B,3 Merge (P AND $38) into $EF7A under mask P; likely
C $ED4E,1 sets this channel's noise-enable bit(s)
C $ED58,1 Clear +$1E (slide/portamento-active flag) = 0
@ $ED5F label=pcmd_clear_mixer_bits
C $ED5F,3 Clear the bits of $EF7A that are set in P (AND
C $ED62,3 with the complement of P)
C $ED68,2 Set +$1E (slide/portamento-active flag) = 1
@ $ED6F label=pcmd_set_slide_target
C $ED6F,1 Read 2 operand bytes: clear +$07/+$08 (the
C $ED70,1 per-frame X/Y-style step accumulators used
C $ED71,3 elsewhere for object motion, reused here as
C $ED74,3 slide step accumulators), store operand 1 into
C $ED77,3 +$0D (slide target) and operand 2 into +$0E
C $ED7A,4 (slide speed), then set status bit 2 (slide
C $ED7E,1 active)
@ $ED85 label=pcmd_set_driver_flag
C $ED85,1 Read 1 operand byte and store it to the
C $ED86,1 driver-internal flag at $EED1 (cleared to 0 by
C $ED87,3 #R$EB9E on tune start)
@ $ED8C label=pcmd_set_envelope_params
C $ED8C,1 Read 2 operand bytes: operand 1 -> +$1B, operand
C $ED8D,3 2 -> both +$1A and +$1C (likely an envelope/pitch
C $ED90,1 curve's current and target value sharing one
C $ED91,1 initial value)
@ $ED9B label=pcmd_set_status_bits_3_7
C $ED9B,4 Set status bits 7 and 3 of +$00 (no operand
C $ED9F,4 bytes); likely a note-release/end-of-note marker
@ $EDA5 label=pcmd_vibrato_off
C $EDA5,3 +$1D = 0 (B is 0 here): clears bit 6 (vibrato
C $EDA8,2 enable, tested at #R$EEFF) and bit 7/5 (mode/ direction, tested at #R$EF09/#R$EF14)
@ $EDAA label=pcmd_vibrato_on
C $EDAA,4 +$1D = $40: sets bit 6 only -- enables vibrato
C $EDAE,2 (see #R$EEFC phase 3), direction/mode bits clear
@ $EDB0 label=pcmd_vibrato_on_mode2
C $EDB0,4 +$1D = $C0: sets bits 6 and 7 -- enables vibrato in the alternate mode selected at #R$EF09
@ $EDB6 label=pcmd_set_status_bit1
C $EDB6,4 Set status bit 1 of +$00 (no operand bytes)
@ $EDBC label=pcmd_reset_row_counter_clear13
C $EDBC,3 +$13 = 0 (B is 0 here), clear status bit 5, then
C $EDBF,4 rejoin at #R$EE22 (acp_reset_row_counter) rather
C $EDC3,2 than #R$EDE4 -- likely a pattern-loop/jump effect
@ $EDC5 label=pcmd_reset_row_counter
C $EDC5,4 Clear status bit 5 only, then rejoin at #R$EE22
C $EDC9,2 -- likely the no-parameter variant of the above
@ $EDCB label=pcmd_mute_channel
C $EDCB,4 +$1F = $FF; likely mutes this channel
@ $EDD1 label=pcmd_unmute_channel
C $EDD1,3 +$1F = 0 (B is 0 here); likely unmutes this
C $EDD4,2 channel
N $EDD6 Advance one channel's pattern by one tracker row. Decrements the per-row wait counter (+$10); while it is still counting down, only #R$EE38 (envelope/portamento upkeep) runs. Once it reaches zero, reads and processes the next byte(s) from the pattern stream via #R$EDE4.
@ $EDD6 label=advance_channel_pattern
@ $EDE4 label=acp_read_pattern_byte
C $EDE7,3 Bit 7 set = command/effect byte, decode via #R$EE49; bit 7 clear = a note value, handled below
@ $EE22 label=acp_reset_row_counter
N $EE38 Between pattern rows: nudge the current note (+$12) up or down by one per frame if a portamento/slide effect (channel flag bit 3) is active.
@ $EE38 label=channel_slide_upkeep
N $EE49 Command/effect byte decode: range-tests the byte against a cascade of thresholds ($B0, then +$20, +$10, +$18) to find which opcode group it falls in, then either handles it directly (note-table/pointer effects) or falls through to the jump table at #R$EE96 for the fixed-length opcode handlers at #R$ED36-$EDD1.
@ $EE49 label=decode_pattern_command
N $EE96 Jump-table dispatch into the fixed-length opcode handlers at #R$ED36-$EDD1, indexed by the remaining low bits of the command byte (A, with B assumed 0 as throughout this driver). Rather than storing a full 2-byte address per entry, the table at $EC9D stores a 1-byte displacement from the table slot itself to the handler: HL is first set to the table slot ($EC9D + A), then the displacement byte at that slot is added back onto HL, giving the handler's address. This works because every handler lies within 256 bytes of its table slot.
@ $EE96 label=dispatch_pattern_command
C $EE96,3 HL = table slot for this command
C $EE9B,1 C = displacement to the handler
C $EE9C,1 HL = table slot + displacement = handler address
N $EE9E Recompute this channel's AY tone-period and volume/envelope register values from its current tracker state (note, envelope index, effect flags) for the per-frame register cache write at #R$EC71. Returns the tone period in HL and the volume/mixer byte in A (see the callers at #R$ECA2/$ECAF/$ECBC). Five phases, in order: 1. #R$EEA1-$EED0: envelope amplitude advance. If status bit 5 (+$00) is set, decrements the envelope-step counter (+$19); on underflow, reloads it from the speed value (+$0F) and reads the next byte from the envelope-shape table pointer (+$16/+$17, advanced by 1 unless the byte read has bit 7 set, an end-of-table marker that halts the pointer), storing that byte as the current amplitude (+$18). Either way, +$18 is copied into +$13, the volume value this call returns. 2. #R$EED0-$EEFC: tone-period lookup. Note index = (+$12) [set by #R$EDEA from the pattern's raw note + transpose (+$20)] plus a pitch-offset byte read from a per-channel offset sequence at +$0B/+$0C (default/loop start +$09/+$0A): if the byte's bit 7 is clear it is used as-is and the pointer advances; if set, it is a 7-bit end-of-sequence delta and the pointer resets to +$09/+$0A for next time. (note + offset) indexes the 2-byte-per-note tone-period table at #R$EFBC, giving the base tone period in DE. 3. #R$EEFC-$EF4B: vibrato. Only runs if +$1D bit 6 is set (skips to phase 4 otherwise). +$1C is a triangle-wave phase counter that ping-pongs between 0 and (+$1A)*2 by +/-(+$1B) each call, direction tracked in +$1D bit 5 (bit 7 and status bit 0 gate whether the phase updates this call at all, at #R$EF09-$EF12 -- not fully resolved). The phase is centred (-depth..+depth), sign-extended, then left-shifted a note-dependent number of times (the $EF3D-$EF47 loop, incrementing a counter seeded from the note index by $18 until it overflows) to scale a linear vibrato depth into the tone-period domain, where pitch and period are inversely related. The scaled offset is added onto the phase-2 tone period. 4. #R$EF4B-$EF76: portamento/slide. Status bit 0 (+$00) is unconditionally toggled every call. If status bit 2 was set (slide active, #R$ED6F), a countdown in +$0E ticks down; when it reaches zero it reloads and the signed step (+$0D) is sign-extended into BC and accumulated into +$07/+$08, which is then added onto the tone period. 5. #R$EF76-$EFAE: mixer/volume finalisation. Every 4th call (via the bit-0 toggle from phase 4), XORs bit 3 of a global byte ($EC79) into $ECC6 (purpose not established elsewhere in this bank). Merges this channel's tone-enable bits into the shared mixer cache at $EFB6 using the same replace-bits-under-mask idiom as #R$ED36 (mask = +$24). If +$1F was just set to $FF (#R$EDCB, "mute"), handles the one-shot mute transition: clears bit 7 of +$1F (so this only fires once), clears this channel's noise-enable bits in the $EFB6 cache, and forces $ECC6 to $41. Returns the phase-3/4 tone period in HL and the phase-1 volume (+$13) in A.
@ $EE9E label=compute_channel_ay_registers
b $EFAF Data block at EFAF
D $EFAF $EFAF-$EFBA (12 bytes): the per-frame AY register cache, refreshed each frame by #R$EC71/#R$EE9E and flushed to the AY chip by #R$ECCA (registers 0-11: 3 channels' tone-period pairs, noise period, mixer, 3 channels' volumes, envelope-period-fine). Initial contents here are just start-up defaults, overwritten before first use.
R $EFAF $EFBC onward: an AY tone-period lookup table (2 bytes/entry, one per
R $EFAF note), indexed by note number via #R$EE9E ($EEF1 LD HL,$EFBC).
N $EFAF Further sub-tables referenced elsewhere in this driver, contents not decoded byte-by-byte (raw lookup data, not algorithmic): $F07C (indexed pointer table, see #R$EE5A), $F123 (indexed pointer table, see #R$EE7E), $F1AE (an alternate entry point jumped to from the unreferenced code at $ED33), $F225 (the per-tune channel-pointer table used by #R$EB9E, 7 bytes/entry -- see #R$EB9E for the layout).
B $EFAF,2043,8*255,3
c $F7AA Set up the classic ZX Spectrum IM2 "257-byte table" interrupt vector trick: fills $BC00-$BDBD with the byte $BD so that, whatever the low byte of the interrupt vector happens to be, I:HL together always resolve to the single byte at $BDBD; patches that byte to a JP opcode ($C3) whose operand ($BDBE/$BDBF) is set to #R$F8AD, making $F8AD the interrupt handler for every subsequent interrupt. Sets I to the table's page and enables IM 2.
D $F7AA Used by the routines at #R$C06E, #R$C59E, #R$F7C7 and #R$FB99.
@ $F7AA label=setup_im2_interrupt_table
c $F7C7 Boot entry point: sets up interrupts, starts tune 1, then loops forever calling the SFX/music per-frame service (#R$F82F) once per 50Hz interrupt (synchronised via HALT).
D $F7C7 Used by the routine at #R$C000.
@ $F7C7 label=boot_and_run_sound_loop
@ $F7D1 label=basl_service_loop
c $F7D6 Start playing tune A (via #R$EB9E), then set up this tune's sound effect trigger table: looks up a pointer in the table at $FA75 (indexed by A*2) into a per-tune SFX script, and clears the 3 SFX "busy" flags at $F837/$F895/$F8A2. Falls into the script reader below, whose byte-code and interaction with #R$F82F's SFX dispatch is documented at #R$F7F4.
D $F7D6 Used by the routines at #R$C06E, #R$C16A, #R$C59E, #R$F7C7, #R$FB99 and #R$FBC8.
@ $F7D6 label=start_tune_and_sfx_table
N $F7DB This entry point is used by the routine at #R$C59E.
@ $F7DB label=stst_load_sfx_script
N $F7F4 SFX script byte-code reader: reads opcode bytes from the script pointer at $F7FC, with $FE meaning "jump to a new script pointer read from the following 2 bytes" ($F823) and $FF meaning "end of script, disable interrupts and stop the tune" (#R$ED0B via $F829). Any other byte is a delay/repeat value (stored to $F7F5 and used to throttle #R$F7F4's own re-entry -- it decrements $F7F5 and returns early via RET NZ until it counts down to 0). The *following* script byte is a raw byte offset (not a scaled index) into the table at $FAA4: each entry there is 3 bytes -- a selector byte (copied to both $F84E and $F842, read back by #R$F82F) followed by a 2-byte pointer (stored to $F85E, the reload source for #R$F82F's sample-selector-stream pointer at $F853). This entry point is used by the routine at #R$F82F.
@ $F7F4 label=sfx_script_advance
@ $F7FE label=ssa_read_opcode
c $F82F Per-frame SFX/music service: runs the AY music driver (#R$EC71), then dispatches the current sound effect's parameter byte (set up by #R$F7DB/#R$F7FE) to one of three 1-bit "digitised sample" bit-bang players: two fixed 8-row sample tables (#R$F8F2, #R$F95A, played via the shared loop at $F8CD which pulses port $FE from bitmap data clocked out with RLC (HL)/DJNZ) or the procedural routine at #R$FA3A. $F8AD (installed as the IM2 handler by #R$F7AA) just sets a "frame occurred" flag ($F8A8) consumed here and re-enables interrupts.
D $F82F All the "LD A,$00" / "LD HL,$0000" instructions below are self-modified: the operand byte(s) immediately following each opcode double as a persistent state variable, written directly (not via the instruction) by this routine and by #R$F7D6/$F7DB/$F7FE. Three such state bytes are the SFX "busy" flags named at #R$F7D6: $F837 (SFX slot 1), $F895 (SFX slot 2), $F8A2 (1-bit sample playback active).
R $F82F Slot 1 ($F836-$F894): if $F837 = 0 (idle), arms it (=1) and reloads
R $F82F the sample-selector-stream read pointer ($F853) from the reload
R $F82F source at $F85E (set by #R$F7DB/$F7FE) before falling into the
R $F82F stream-reading loop at #R$F855. If already armed, $F842 (a
R $F82F countdown, also the selector byte copied by #R$F7FE) is decremented;
R $F82F only when it reaches 1 does #R$F84D re-enter the stream-reading loop
R $F82F -- otherwise slot 1 is skipped this frame and control falls to slot 2.
N $F82F Stream-reading loop (#R$F855): reads a byte from the selector stream; a byte of exactly 1 calls #R$F7F4 (sfx_script_advance's own re-entry, throttled by its own $F7F5 countdown) to pull in a fresh selector byte and restarts the loop with the reload pointer. Any other byte is the entry to act on this frame: bit 7 marks it as also arming slot 2 ($F842 and $895 both set to 1, banked via AF' so it doesn't disturb the byte being decoded); the low 3 bits (1/2/3) select which of the three 1-bit-sample engines to trigger this frame (#R$F8B6 = sample table #R$F8F2, #R$F8BD = sample table #R$F95A, or #R$FA3A the procedural generator), with the byte's upper 5 bits stashed via $F8CE as a playback-rate/pitch parameter for the two fixed samples. A low-3-bits value of 0 triggers nothing and falls through to slot 2. Slot 2 ($F894-$F8A1): if $F895 is set, decrements both $F842 and $F895 (companion countdowns for whatever slot 1 armed via the bit-7 path above); purpose of the parallel countdown not established further. Tail ($F8A1-$F8A6): if $F8A2 (sample-playback-active) is exactly 1, falls into #R$F8CC/#R$F8CD to pulse out the next row of whichever fixed sample was armed above; otherwise returns without playing anything this frame. Used by the routines at #R$C06E, #R$C16A, #R$C59E, #R$F7C7 and #R$FBC8.
@ $F82F label=sfx_music_service
C $F82F,3 Run the AY music driver for this frame
C $F832,1 Clear the "frame occurred" flag consumed by
C $F833,3 #R$F8A7
@ $F836 label=sfx1_check_busy
C $F836,2 A = SFX slot 1 busy flag ($F837)
C $F839,2 Busy -> $F841 to tick the countdown
C $F83B,1 Idle -> arm slot 1 (busy = 1)
C $F83F,2 Reload the read pointer and enter the loop
@ $F841 label=sfx1_tick_countdown
C $F841,2 A = slot 1 countdown/selector byte ($F842)
C $F844,3 Reached 1 -> re-enter the stream-reading loop
C $F847,3 Else store the decremented countdown and skip
C $F84A,3 slot 1 entirely this frame
@ $F84D label=sfx1_reenter_stream
C $F84D,2 Reset the countdown to 0 (about to be replaced by
C $F84F,3 the next entry read from the stream)
C $F852,3 HL = current stream read pointer ($F853)
@ $F855 label=sfx_read_stream_byte
C $F855,1 A = next byte from the selector stream
C $F857,3 Not "1" -> $F866, this is an entry to act on
C $F85A,3 "1" -> pull in a fresh selector byte
@ $F85D label=sfx1_reload_pointer
C $F85D,3 Reload the stream pointer from the source set by
C $F860,3 #R$F7DB/$F7FE ($F85E) and retry
@ $F866 label=sfx_dispatch_entry
C $F866,1 Advance and persist the stream pointer
C $F86A,1 Restore the original entry byte (undo the DEC)
C $F86B,2 Bit 7 set -> also arm slot 2
C $F86F,2 Mask off bit 7 (banked via AF' so the low bits
C $F871,1 used for dispatch below are undisturbed)
C $F874,3 Arm slot 1's countdown/selector and slot 2's busy
C $F877,3 flag together
C $F87B,1 D = entry byte (masked if bit 7 was set)
C $F87C,2 Low 3 bits select which 1-bit-sample engine
C $F87E,2 0 -> nothing to trigger this frame
C $F881,1 A = upper 5 bits: playback-rate/pitch parameter
C $F882,2 for the two fixed samples (stashed via $F8CE at
C $F884,2 #R$F8C2)
C $F889,3 Selector 1 -> fixed sample table #R$F8F2
C $F88D,3 Selector 2 -> fixed sample table #R$F95A
C $F891,3 Selector 3 -> procedural engine noise
@ $F894 label=sfx2_tick_countdown
C $F894,2 A = SFX slot 2 busy flag ($F895)
C $F897,2 Idle -> nothing to tick
C $F899,3 Busy -> decrement both companion countdowns
@ $F8A1 label=sfx_tail_check_active
C $F8A1,2 A = 1-bit sample playback active flag ($F8A2)
C $F8A4,3 Active -> play the next row of the armed sample
N $F8A7 This entry point is used by the routines at #R$F8EB and #R$FA3A.
@ $F8A7 label=wait_for_frame_flag
N $F8AD IM2 interrupt handler (installed by #R$F7AA): just sets the "frame occurred" flag ($F8A8) polled by #R$F8A7 and re-enables interrupts. The actual per-frame work happens synchronously from the main loop at #R$F7D1, not here.
@ $F8AD label=frame_interrupt_handler
N $F8CD 1-bit sample bit-bang loop: pulses port $FE (border/speaker) from bitmap data at HL, one row of 8 bits per iteration, RLC (HL) rotating the next bit into carry-adjacent position; D counts rows remaining.
@ $F8CD label=play_sample_row
c $F8EB Clears the "sample playing" flag and jumps back to #R$F8A7 to wait for the next frame -- the tail end of #R$F82F's sample playback path.
D $F8EB Used by the routine at #R$F82F.
@ $F8EB label=finish_sample_playback
b $F8F2 Data block at F8F2
D $F8F2 1-bit digitised sample: 104 bytes (see #R$F8B6, D=$68=104), each byte one playback "row" of 8 bits, played back by #R$F82F/#R$F8CD.
B $F8F2,104,8
b $F95A Data block at F95A
D $F95A 1-bit digitised sample: 224 bytes (see #R$F8BD, D=$E0=224), played back the same way as #R$F8F2.
B $F95A,224,8
c $FA3A Procedural engine/tyre-noise generator: repeatedly reads and updates self-modifying state bytes at $FA72-$FA74 (a running counter/pitch value nudged each call) to derive a variable duty-cycle square wave, toggling port $FE (speaker/border) through busy-wait delay loops (#R$FA58/#R$FA5F) whose lengths are driven by that state -- this is how the beeper engine-note pitch varies with speed/RPM. Loops D=$32 times per call, and E times overall (#R$FA6C), polling the frame flag (#R$F8A7's $F8A8) to bail out early if a new frame has started.
D $FA3A A (-> E) is the pitch/rate parameter from #R$F82F (the dispatch byte's upper 5 bits). Two nested loops: outer E times (#R$FA6C/$FA6D), inner D = 50 times each (#R$FA64/$FA65); each inner iteration advances the 3-byte self-modified state at $FA72-$FA74 (a phase counter, a wrapping accumulator subtracting a fixed constant, and a rotating byte mixed back into the accumulator) and tests bit 4 of the result to decide whether to emit a click this iteration -- a bit-4 test on a steadily-advancing counter behaves like a variable duty-cycle gate, the source of the engine/tyre buzz. When a click fires, the ON delay is $18-E cycles and the OFF delay is E cycles (#R$FA58/#R$FA5F): a larger E (higher dispatch parameter) shortens the ON wait but lengthens the OFF wait, lowering the effective pitch -- consistent with this parameter tracking engine RPM/speed. After each inner-loop pass, the frame flag ($F8A8, set by #R$F8AD) is checked; note that the "AND A" immediately before "RET C" always clears the carry flag, so that RET C can never actually fire -- an apparent dead check preserved as found in the original code, not a translation artifact. Once the outer loop completes, control falls into #R$F8A7 to wait for the next frame.
R $FA3A Used by the routine at #R$F82F.
@ $FA3A label=procedural_engine_noise
C $FA3A,1 E = pitch/rate parameter; outer loop counter
C $FA3B,2 D = 50; inner loop counter
@ $FA3D label=pen_inner_loop
C $FA3D,3 Advance the 3-byte phase/accumulator state
C $FA50,2 Bit 4 of the result gates this iteration's click
C $FA54,2 ON delay = $18 - E
C $FA5C,2 Speaker/border on
C $FA5E,1 OFF delay = E
C $FA62,2 Speaker/border off
@ $FA64 label=pen_inner_loop_tail
C $FA67,3 Frame flag check (see comment above: RET C here
C $FA6A,1 never fires, AND A always clears carry)
C $FA6C,1 Outer loop
C $FA6F,3 Done: wait for the next frame
b $FA72 Data block at FA72
D $FA72 $FA72-$FA74: self-modifying counter/pitch state read and rewritten by #R$FA3A on every call (the procedural engine-noise generator).
R $FA72 $FA75 onward: the per-tune SFX trigger-script pointer table indexed by
R $FA72 tune-number*2, referenced by #R$F7DB.
N $FA72 $FAA4 onward: the per-sound-ID parameter table read by #R$F7FE's script interpreter (3 bytes/entry: 1 selector byte + a 2-byte pointer -- see #R$F7F4), giving each triggered sound its sample table selector and pointer. The remainder of this block is raw pattern/parameter data consumed by the SFX scripts and not decoded byte-by-byte here.
B $FA72,295,8*36,7
c $FB99 Options menu driver (reached from the title screen's "PRESS ENTER FOR OPTIONS" prompt, see the text block at #R$CC50). Boots interrupts and tune 0, then loops: draws the control-select screen text (#R$FC29 via #R$FD9C -- "ENTER OPTION" / P1. SINCLAIR JOYSTICK / P2. CURSOR JOYSTICK / P3. KEMPSTON JOYSTICK / P4. KEYBOARD / P5. DEFINE KEYS) and polls the keyboard (port $FE, keys 1-5 read as a 5-bit mask) to dispatch into one of the five listed choices.
D $FB99 Keys are read from half-row $F7FE (keys "1"-"5"), which is exactly the "P1"-"P5" option-select row shown by the #R$FC29 "ENTER OPTION" text. #R$FBB2-$FBB3 inverts and masks to a 5-bit "pressed" mask (bit 0 = key "1" .. bit 4 = key "5"); no key pressed loops back to poll again. #R$FBB7-$FBC1 then shifts that mask right one bit at a time via RRA, testing each bit's carry in turn: key "1" -> #R$FBD4 (Sinclair joystick, control-key list A), "2" -> #R$FBD9 (Cursor joystick, list B, shares the copy loop at #R$FBDC), "3" -> #R$FC14 (Kempston joystick detect), "4" -> #R$FBE4 (keyboard, handled inline). Key "5" falls through the whole RRA chain unbranched, so it is the default action: #R$FEA9, the "DEFINE KEYS" key-redefinition screen.
R $FB99 Used by the routine at #R$C000.
@ $FB99 label=options_menu_driver
N $FBA2 This entry point is used by the routine at #R$C59E.
@ $FBA2 label=omd_redraw_and_poll
N $FBAB This entry point is used by the routine at #R$FC14.
@ $FBAB label=omd_service_and_read_keys
C $FBAB,3 Service sound/music while polling
C $FBAE,2 Read keyboard half-row $F7FE (keys "1"-"5")
C $FBB5,2 No key -> poll again
C $FBB7,1 Key "1"
C $FBBA,1 Key "2"
C $FBBD,1 Key "3"
C $FBC0,1 Key "4"
C $FBC3,3 Key "5" (default): "DEFINE KEYS" screen
c $FBC8 Runs one frame of the SFX/music service (#R$F82F) and, if no tune is currently active, restarts tune 0 -- keeps the options-menu background music looping.
D $FBC8 Used by the routines at #R$FB99, #R$FC14, #R$FEA9 and #R$FF2C.
@ $FBC8 label=service_sound_and_loop_tune0
N $FBD4 This entry point is used by the routine at #R$FB99.
N $FBD9 This entry point is used by the routine at #R$FB99.
N $FBE4 This entry point is used by the routine at #R$FB99.
N $FBE5 This entry point is used by the routine at #R$FC14.
c $FC14 Joystick-present detector for the control-select sub-screen: samples the Kempston port ($1F) $14=20 times, servicing sound each iteration (#R$FBC8), and bails out to the main poll loop (#R$FBAB) as soon as the port's value changes (i.e. a joystick is moving/present); otherwise falls through to #R$FBE5 with A=1 (no joystick detected).
D $FC14 Used by the routine at #R$FB99.
@ $FC14 label=detect_kempston_joystick
b $FC29 Data block at FC29
D $FC29 Text for the options menu and hidden test/service menu screens (same position+attribute+ASCII-with-bit7-terminator encoding as #R$CC50), printed via #R$FD9C from pointers set up in #R$FB99/#R$FEA9: - Control select: "ENTER OPTION" / P1. SINCLAIR JOYSTICK / P2. CURSOR JOYSTICK / P3. KEMPSTON JOYSTICK / P4. KEYBOARD / P5. DEFINE KEYS - Key redefinition: "REDEFINE KEYS" / GEAR / ACCELERATE / BRAKE / LEFT / RIGHT / QUIT / PAUSE / TURBO - Test mode: "CHASE H.Q. TEST MODE" / TITLE SCREEN / 1 TO 5. LOGO ANIMATION / 6. SCORE ENTRY / PIN GAME / P1. RESTART LEVEL / P2. NEXT LEVEL / P3. END SCREEN / P4. EXTRA CREDIT
R $FC29 The text ends around $FD97; the bytes from roughly $FD99 onward
R $FC29 disassemble as plausible Z80 code (LDIR/CALL $FBC8 patterns), so this
R $FC29 may actually be a code routine that the control file has mislabeled
R $FC29 as part of this data block -- worth revisiting when regenerating the ctl (not reclassified here, structural pass).
B $FC29,640,8
c $FEA9 "Redefine keys" screen driver. Prints the title/prompt text and the 8 control-name labels (#R$FC29: gear, accelerate, brake, left, right, quit, pause, turbo) via #R$FD9C, then for each of the 8 controls in turn (BC=$0801 at #R$FEBE) waits for a fresh keypress (#R$FF2C) and stores the chosen key, redrawing its name on screen.
D $FEA9 Hidden test-mode unlock ($FEE2-$FEF1): compares the 8 keys just chosen against a fixed 8-byte reference sequence at #R$FFEF (in the same scan-key-code encoding documented at #R$FF2C). Decoding those 8 bytes through that encoding spells S, H, O, C, K, E, D, ENTER -- i.e. this is a hidden cheat code: redefining the 8 controls to spell "SHOCKED" followed by ENTER. On any mismatch the loop's RET NZ returns immediately (the ordinary case -- the new key mapping is simply kept and control returns to the caller, #R$FB99). On an exact match it falls through, sets the test-mode/cheat flag at $8000 (see #R$84C2 in the main skool), shows a "TEST ... CHASE H.Q. TEST MODE" confirmation (#R$FCF0 text) and waits for fire, then loops back to $FEA9 to redisplay the redefine-keys screen (there is no path back to the caller once the secret code has been entered other than through this routine itself).
R $FEA9 Used by the routine at #R$FB99.
@ $FEA9 label=redefine_keys_screen
C $FEE2,2 "SHOCKED"+ENTER secret-code check (see block header)
C $FEE4,3 DE -> fixed reference key sequence
C $FEE7,3 HL -> the 8 keys just chosen
C $FEEE,1 Mismatch -> ordinary case, return to caller
C $FEF1,2 All 8 matched -> enable test mode
c $FF0C Scans the ZX Spectrum keyboard matrix (the classic 8 half-row ports $FEFE, $FDFE, ..., $7FFE, walked by rotating the row-select byte in B via RLC) for a single currently-held key. D starts at $FF and is incremented once per row found to have any key held, then overwritten with the key's bit-position within its row (via the SRL/JR NC shift- count at $FF1D-$FF21); E counts down the row number. Returns NZ (an early RET NZ at $FF1A/$FF23) if more than one row has a key held (reject as ambiguous); otherwise always returns with Z set (via the unconditional CP A at $FF2A) and the caller (#R$FF2C) tells "no key" from "one key" by checking whether D is still $FF.
D $FF0C Used by the routine at #R$FF2C.
@ $FF0C label=scan_keyboard_matrix
c $FF2C Waits for a fresh single keypress (servicing sound each poll via #R$FBC8 and scanning via #R$FF0C), rejecting it and retrying if it is an ambiguous multi-row press (NZ from #R$FF0C) or "no key held" (D still $FF).
D $FF2C Duplicate check ($FF3D-$FF48): compares the new key code against the C-1 entries already stored at $FFF7 (C is the 1-based control index from the caller); a match rejects and loops back to re-scan.
R $FF2C Store ($FF4C-$FF52): the new key code is written to $FFF6+C (i.e.
R $FF2C $FFF7 for control 1, ... $ FFFE for control 8) -- the growing list
R $FF2C used by the duplicate check above and read back by #R$FEE2-$FEF1.
N $FF2C Name lookup ($FF53-$FF68): the scan-matrix key code (bits 0-2 = column/bit-within-row 0-4, bits 3+ = row 0-7, from #R$FF0C) is unpacked into row and bit, then re-combined as index = 5*bit + row (a column-major layout, not row-major) to index the 2-bytes/entry name table at #R$FF95. Print ($FF6D-$FF7F): the looked-up character and a second byte (with bit 7 forced set, presumably an attribute/terminator flag for the string printer) are written into a 2-character scratch buffer at $FD97-$FD9B alongside the caller's screen address (DE), then #R$FDA4 prints it. Column advance ($FF82-$FF94): restores the caller's screen address and steps it on by $20 (one label column); on overflow (E wraps past $FF) also bumps D by 8 to drop down a pixel row. The same step is applied unconditionally once more when the caller's remaining loop count B is exactly 4 (#R$FF87-$FF8A) -- the extra step that wraps from the first row of 4 control labels to the second row of 4. Used by the routine at #R$FEA9.
@ $FF2C label=read_new_key_definition
b $FF95 Data block at FF95
D $FF95 $FF95-$FFE4: key-name lookup table for the "redefine keys" screen (2 bytes/entry -- printable character + space, with SYMBOL SHIFT/SPACE/ ENTER/CAPS SHIFT spelled out as two-letter codes -- indexed by #R$FF2C's row/bit arithmetic). The entries run in *reverse* matrix order: row 7 ($7FFE) down to row 0 ($FEFE), and within each row bit 4 down to bit 0 -- e.g. the first 5 entries (B, N, M, SY, SP) are row 7's keys read backwards, matching #R$FF2C's index = 5*(7-row) + (4-bit).
R $FF95 $FFE5-$ FFE9 ("list A", installed by #R$FBD4 for the "SINCLAIR
R $FF95 JOYSTICK" option) and $FFEA-$FFEE ("list B", by #R$FBD9 for "CURSOR
R $FF95 JOYSTICK") are each a 5-byte set of scan-key-codes in the same
R $FF95 encoding. Decoded, list A uses only keys from the "0 9 8 7 6" row and
R $FF95 list B uses keys {0, 5, 6, 7, 8} -- i.e. genuine emulation of the
R $FF95 classic Sinclair Interface II (keys 6-0) and Cursor/Protek (keys
R $FF95 5,6,7,8,0) joystick wiring conventions, not arbitrary key choices.
N $FF95 $FFEF-$FFF6 is the fixed 8-byte "SHOCKED"+ENTER secret-code reference used by #R$FEA9's hidden test-mode unlock (see that routine's header for the full decode). $FFF7-$FFFE is not fixed data at all: it is the live scan-key-code buffer that #R$FBDC/#R$FF52 write the currently-active 5- or 8-key control scheme into, and #R$FBF3/#R$FF3D/#R$FEE7 read back from; the bytes shown here are simply whatever was resident when this snapshot was taken. End of bank 3.
B $FF95,107,8*13,3
