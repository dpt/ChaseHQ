/**
 * $8E6C: Print a message on screen
 *
 * Renders a single message from the pregame chatter table. The message data
 * block encodes the attribute byte, the back-buffer destination address, the
 * attribute address, and then the NUL-terminated string itself. Returns a
 * pointer past the string so the caller can advance to the next message.
 *
 * \param[in] state    Pointer to game state.
 * \param[in] style    Rendering style selector. (was A)
 * \param[in] messages Pointer to the start of the message data block. (was HL)
 *
 * \return Pointer to the byte following the NUL terminator.
 */
const u8 *print_message(chqstate_t *state,
                        u8          style,
                        const u8   *messages)
{
  u8        Aattr;       /* attribute byte for the string (was A) */
  u16       DEbackbuf;   /* back-buffer destination address (was DE) */
  u16       BCtarget;    /* screen attribute address (was BC) */

  // Conv: header fields read explicitly rather than via INC HL chains.
  Aattr     = messages[1]; /* messages[0] is a flags byte we ignore */
  DEbackbuf = wordat(messages + 2);
  BCtarget  = wordat(messages + 4);
  messages += 6;

  return draw_string_with_style(state,
                                Aattr,
                                ADDRTOSCREEN(BCtarget),
                                ADDRTOBACKBUF(DEbackbuf),
                                messages,
                                style);
}
