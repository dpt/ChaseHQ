#!/usr/bin/env python3
"""Check function prologues against C/docs/function_comment_template_example.c.

Usage: python3 check_prologues.py [file.c ...]
Default: all *.c in libraries/ChaseHQ/Engine/

Checks the /** ... */ block only (not variable declarations in the body):
  - title line "$XXXX: description [Conv: HQ]"
  - blank separator line
  - non-empty description paragraph
  - blank line, then \\param[in|out|in,out] name  ...  for every parameter
    (the leading chqstate_t *state parameter is exempt)
  - \\return line iff the function's return type is not void
  - every comment line starts with " * " (or is " */")
"""
import glob
import re
import sys

C_KEYWORDS = {
  "for", "while", "if", "switch", "return", "sizeof", "do", "else",
}

TITLE_RE = re.compile(r"^\$[0-9A-Fa-f]+.*:\s*\S.*$")
PLAIN_TITLE_RE = re.compile(r"^\S.*$")  # C-only helper with no Z80 address
PARAM_RE = re.compile(r"^\\param\[(in|out|in,out)\]\s+(\S+)\s+\S.*$")
RETURN_RE = re.compile(r"^\\return\s+\S.*$")
DOUBLE_SPACE_AFTER_PERIOD_RE = re.compile(r"[.!?]  ")
# A real Conv: note starts a sentence (start of paragraph or after ". "); this
# excludes mentions of the word inside quotes, e.g. a comment discussing
# another function's "Conv: ..." note by name.
CONV_NOTE_RE = re.compile(r"(?:^|\. )Conv:")


def check_prose_lines(lines):
  """Flag old-style double spaces after sentence-ending punctuation.

  Only checks title/description/\\return/Conv prose, not \\param blocks or
  continuation lines, which are hand-aligned into columns (ReflowComments is
  off) and legitimately contain runs of spaces unrelated to sentence spacing.
  """
  errors = []
  for line in lines:
    if DOUBLE_SPACE_AFTER_PERIOD_RE.search(line):
      errors.append("double space after sentence-ending punctuation: %r" % line)
  return errors

DEFAULT_GLOB = "libraries/ChaseHQ/Engine/*.c"


def find_matching_paren(text, open_idx):
  depth = 0
  for i in range(open_idx, len(text)):
    if text[i] == "(":
      depth += 1
    elif text[i] == ")":
      depth -= 1
      if depth == 0:
        return i
  return -1


def split_params(params_str):
  params_str = params_str.strip()
  if not params_str or params_str == "void":
    return []
  parts = []
  depth = 0
  current = ""
  for ch in params_str:
    if ch == "(":
      depth += 1
    elif ch == ")":
      depth -= 1
    if ch == "," and depth == 0:
      parts.append(current)
      current = ""
    else:
      current += ch
  parts.append(current)
  return [p.strip() for p in parts]


def param_name(param):
  fnptr = re.search(r"\(\s*\*\s*([A-Za-z_]\w*)\s*\)", param)
  if fnptr:
    return fnptr.group(1)
  match = re.search(r"([A-Za-z_]\w*)\s*(\[\s*\])?$", param)
  return match.group(1) if match else None


def parse_signature(code_after_comment):
  """Return (return_type, params, is_definition, name) or None if not a function."""
  open_idx = code_after_comment.find("(")
  if open_idx == -1:
    return None
  header_text = code_after_comment[:open_idx]
  # A blank line, preprocessor directive, or stray statement before the "("
  # means this isn't the signature immediately following the prologue (e.g.
  # a file-header banner comment followed by #include lines).
  if "\n\n" in header_text or re.search(r"[;{}#]", header_text):
    return None
  close_idx = find_matching_paren(code_after_comment, open_idx)
  if close_idx == -1:
    return None
  header = code_after_comment[:open_idx].strip()
  params_str = code_after_comment[open_idx + 1:close_idx]
  rest = code_after_comment[close_idx + 1:].lstrip()
  if not rest:
    return None
  is_definition = rest[0] == "{"
  if not is_definition and rest[0] != ";":
    return None

  name_match = re.search(r"([A-Za-z_]\w*)\s*$", header)
  if not name_match:
    return None
  if name_match.group(1) in C_KEYWORDS:
    # A control-flow statement (e.g. "for (;;) {") immediately after a
    # documentary comment on an internal label, not a function signature.
    return None
  return_type = header[: name_match.start()].strip()
  params = split_params(params_str)
  return return_type, params, is_definition, name_match.group(1)


def split_paragraphs(lines):
  """Split blank-separated comment lines into non-empty paragraphs."""
  paragraphs = []
  current = []
  for line in lines:
    if line == "":
      if current:
        paragraphs.append(current)
        current = []
    else:
      current.append(line)
  if current:
    paragraphs.append(current)
  return paragraphs


def check_function(filename, lineno, comment_lines, return_type, params, func_name):
  errors = []

  # The leading chqstate_t *state parameter is usually left undocumented,
  # but is sometimes given an explicit \param (e.g. when the function reads
  # or writes through it in a way worth calling out). Both are acceptable.
  has_state_param = bool(params) and "chqstate_t" in params[0]
  params_without_state = params[1:] if has_state_param else params

  raw = comment_lines
  for i, line in enumerate(raw):
    if line != "*/" and not line.startswith("* ") and line != "*":
      errors.append("comment line %d not formatted as ' * ...': %r" % (i, line))

  content = []
  for line in raw:
    if line in ("*/",):
      continue
    if line.startswith("* "):
      content.append(line[2:])
    elif line == "*":
      content.append("")

  if not content:
    errors.append("empty prologue")
    return errors

  for i in range(1, len(content)):
    if content[i] == "" and content[i - 1] == "":
      errors.append("repeated empty comment line at row %d" % i)

  for p in split_paragraphs(content):
    if len(CONV_NOTE_RE.findall(" ".join(p))) > 1:
      errors.append("multiple 'Conv:' notes in one paragraph, split into separate paragraphs: %r" % p)

  if content[0].startswith("$"):
    if not TITLE_RE.match(content[0]):
      errors.append("title line missing '$XXXX: description' format: %r" % content[0])
    title_prefix = content[0].split(":", 1)[0]
    if re.search(r"\b%s\b" % re.escape(func_name), title_prefix):
      errors.append(
        "title line repeats function name before the colon: %r" % content[0]
      )
  elif not PLAIN_TITLE_RE.match(content[0]):
    errors.append("empty title line")
  errors.extend(check_prose_lines(content[:1]))

  idx = 1
  if idx >= len(content) or content[idx] != "":
    errors.append("missing blank line after title")
  else:
    idx += 1

  # Description: one or more paragraphs (blank-line separated, e.g. a Conv:
  # note), ending where \param or \return lines begin.
  desc_start = idx
  while (idx < len(content)
         and not content[idx].startswith("\\param")
         and not content[idx].startswith("\\return")):
    idx += 1
  description_lines = content[desc_start:idx]
  if not any(l.strip() for l in description_lines):
    errors.append("missing description paragraph")
  elif idx < len(content) and description_lines[-1] != "":
    errors.append("missing blank line before \\param/\\return section")
  errors.extend(check_prose_lines(description_lines))

  expected_names = [param_name(p) for p in params_without_state]
  expected_names_with_state = (
    [param_name(params[0])] + expected_names if has_state_param else expected_names
  )
  found_names = []
  while idx < len(content) and content[idx].startswith("\\param"):
    m = PARAM_RE.match(content[idx])
    if not m:
      errors.append("malformed \\param line: %r" % content[idx])
    else:
      found_names.append(m.group(2))
    idx += 1
    # Continuation lines (ReflowComments is off, long param descriptions wrap).
    while (idx < len(content) and content[idx] != ""
           and not content[idx].startswith("\\param")
           and not content[idx].startswith("\\return")):
      idx += 1

  missing_names = [n for n in expected_names if n not in found_names]
  if missing_names:
    errors.append(
      "missing \\param for parameter(s): %s" % ", ".join(missing_names)
    )
  elif found_names != expected_names and found_names != expected_names_with_state:
    errors.append(
      "\\param names %r do not match signature params %r (state optional)"
      % (found_names, expected_names)
    )

  while idx < len(content) and content[idx] == "":
    idx += 1

  return_tokens = re.sub(r"\s+", " ", return_type).strip().split(" ")
  is_void = return_tokens[-1] == "void"  # last token, past any static/extern
  remaining = [l for l in content[idx:] if l != ""]

  if is_void:
    tail_paragraphs = split_paragraphs(content[idx:])
    stray = [p for p in tail_paragraphs if not p[0].startswith("Conv:")]
    if stray:
      errors.append("void function has trailing prologue content: %r" % stray[0])
  else:
    if not remaining:
      errors.append("non-void function missing \\return line")
    elif not RETURN_RE.match(remaining[0]):
      errors.append("expected \\return line, got: %r" % remaining[0])
    else:
      errors.extend(check_prose_lines(remaining[:1]))
      tail_paragraphs = split_paragraphs(content[idx:])[1:]
      stray = [p for p in tail_paragraphs if not p[0].startswith("Conv:")]
      if stray:
        errors.append("trailing content after \\return: %r" % stray[0])

  if content[idx:]:
    tail_lines = [l for p in split_paragraphs(content[idx:]) if p[0].startswith("Conv:") for l in p]
    errors.extend(check_prose_lines(tail_lines))

  return errors


def extract_functions(filename):
  """Return (lineno, func_name, comment_lines, return_type, params) for every
  prologue-documented function definition, in file order."""
  text = open(filename).read()
  functions = []
  for m in re.finditer(r"/\*\*.*?\*/", text, re.DOTALL):
    raw_lines = [l.strip() for l in m.group(0).splitlines()]
    if raw_lines[0] != "/**" or raw_lines[-1] != "*/":
      continue
    comment_lines = raw_lines[1:]  # drop opening "/**"; keep trailing "*/" marker
    lineno = text.count("\n", 0, m.start()) + 1

    after = text[m.end():]
    after = after.lstrip("\n")
    parsed = parse_signature(after)
    if parsed is None:
      continue
    return_type, params, is_definition, func_name = parsed
    if not is_definition:
      continue

    functions.append((lineno, func_name, comment_lines, return_type, params))
  return functions


def check_file(filename):
  results = []
  for lineno, func_name, comment_lines, return_type, params in extract_functions(filename):
    errors = check_function(filename, lineno, comment_lines, return_type, params, func_name)
    results.append((lineno, errors))
  return results


ADDR_RE = re.compile(r"^\$([0-9A-Fa-f]{4})")


def title_address(comment_lines):
  """First Z80 address named in a prologue's title line, or None for a
  C-only helper with no address of its own."""
  for line in comment_lines:
    if line.startswith("* "):
      m = ADDR_RE.match(line[2:])
      return int(m.group(1), 16) if m else None
    if line == "*":
      return None
  return None


def check_order(filename):
  """Flag functions whose prologue address is lower than an earlier
  function's, i.e. not declared in original-game (address) order.

  Functions with no address in their title (Conv-only helpers) are skipped:
  they have no fixed position of their own and are expected to sit next to
  whichever addressed function uses them.
  """
  errors = []
  running_max = None
  running_max_name = None
  for lineno, func_name, comment_lines, _return_type, _params in extract_functions(filename):
    addr = title_address(comment_lines)
    if addr is None:
      continue
    if running_max is not None and addr < running_max:
      errors.append(
        "%s:%d: %s ($%04X) declared after %s ($%04X) -- out of original game order"
        % (filename, lineno, func_name, addr, running_max_name, running_max)
      )
    else:
      running_max = addr
      running_max_name = func_name
  return errors


def main(argv):
  files = argv[1:] if len(argv) > 1 else glob.glob(DEFAULT_GLOB)
  total_errors = 0
  total_checked = 0
  order_errors = 0
  for filename in sorted(files):
    for lineno, errors in check_file(filename):
      total_checked += 1
      if errors:
        total_errors += 1
        print("%s:%d: FAIL" % (filename, lineno))
        for e in errors:
          print("    %s" % e)
    for e in check_order(filename):
      order_errors += 1
      print(e)
  print("checked %d prologues, %d with errors" % (total_checked, total_errors))
  print("%d function(s) declared out of original game order" % order_errors)
  return 1 if (total_errors or order_errors) else 0


if __name__ == "__main__":
  sys.exit(main(sys.argv))
