#!/usr/bin/env python3
"""Run format_signatures.py, align_declarations.py and check_prologues.py
over the same files in one pass.

check_prologues.py has no --fix: a missing \\param or wrong \\return needs a
human to write the content, so it always runs read-only, in both modes.

Usage: python3 format_all.py [--fix] [file.c ...]
Default: all *.c in libraries/ChaseHQ/Engine/, check-only unless --fix given.
"""

import sys

import align_declarations
import check_prologues
import format_signatures


def main(argv):
    fix = "--fix" in argv[1:]
    files = [a for a in argv[1:] if a != "--fix"]
    fix_argv = argv[:1] + (["--fix"] if fix else []) + files
    check_argv = argv[:1] + files

    rc = format_signatures.main(fix_argv)
    rc |= align_declarations.main(fix_argv)
    rc |= check_prologues.main(check_argv)
    return rc


if __name__ == "__main__":
    sys.exit(main(sys.argv))
