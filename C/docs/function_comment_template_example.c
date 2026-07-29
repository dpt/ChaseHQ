// - Format to 80 columns wide.
// - C++ comments are used for "not finalised" commentary.
// - C-style comments are intended to be good/final/approved commentary.
// - Put a prologue before functions.
// - Write a short (< 80 col) summary.
// - Write a paragraph of explanation below that.
// - Use Doxygen style param/return in the prologue.
// - Omit the 'state' parameter since it's used everywhere.
// - Try to align parameters horizontally.

/**
 * $8E6C: Print a message on screen
 *
 * Renders a single message from the pregame chatter table. The message data
 * block encodes the attribute byte, the back-buffer destination address, the
 * attribute address, and then the NUL-terminated string itself. Returns a
 * pointer past the string so the caller can advance to the next message.
 *
 * \param[in] style    Rendering style selector. (was A)
 * \param[in] messages Pointer to the start of the message data block. (was HL)
 *
 * \return             Pointer to the byte following the NUL terminator.
 */
const u8 *print_message(chqstate_t *state,
                        u8          style,
                        const u8   *messages)
{
  u8  A_attr;     /* attribute byte for the string (was A) */
  u16 DE_backbuf; /* back-buffer destination address (was DE) */
  u16 BC_target;  /* screen attribute address (was BC) */

  /* Conv: header fields read explicitly rather than via INC HL chains. */
  A_attr     = messages[1]; /* messages[0] is a flags byte we ignore */
  DE_backbuf = wordat(messages + 2);
  BC_target  = wordat(messages + 4);
  messages += 6;

  return draw_string_with_style(state,
                                A_attr,
                                ADDRTOSCREEN(BC_target),
                                ADDRTOBACKBUF(DE_backbuf),
                                messages,
                                style);
}
