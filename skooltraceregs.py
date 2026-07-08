#!/usr/bin/env python3
"""Z80 register lifetime tracker for SkoolKit skool files.

Usage: python3 skooltraceregs.py <label-or-$addr> [--dot] [--skool file ...]
"""

import re
import sys
import argparse
import glob
import os

# ---------------------------------------------------------------------------
# Skool parser
# ---------------------------------------------------------------------------

_FUNC_PAT = re.compile(r"^c\$([0-9A-Fa-f]{4})\s+(.*)")
_INSTR_PAT = re.compile(r"^[c *]\$([0-9A-Fa-f]{4})\s+(.*)")
_LABEL_PAT = re.compile(r"^@label=(\w+)")


_SM_PAT = re.compile(r"<self.mod", re.IGNORECASE)
_OVERRIDE_PAT = re.compile(r"\{([A-Za-z][A-Za-z0-9']*?)=([A-Za-z][A-Za-z0-9_]*)\}")


def parse_skool(paths):
    """Return (instrs, labels, funcs, stars, sm_addrs, overrides).

    instrs:    list of (addr:int, raw:str)
    labels:    dict name -> addr
    funcs:     dict addr -> index into instrs (c$ primary entry points)
    stars:     dict addr -> index into instrs (*$ secondary entry points)
    sm_addrs:  set of addresses whose immediate operand is self-modified at runtime
    overrides: dict addr -> {reg: name} from '{Reg=name}' comment annotations
    """
    instrs = []
    labels = {}
    funcs = {}
    stars = {}
    sm_addrs = set()
    overrides = {}
    pending_label = None

    for path in paths:
        with open(path) as fh:
            for line in fh:
                line = line.rstrip("\n")

                m = _LABEL_PAT.match(line)
                if m:
                    pending_label = m.group(1)
                    continue

                m = _INSTR_PAT.match(line)
                if m:
                    addr = int(m.group(1), 16)
                    body = m.group(2)
                    parts = body.split(";", 1)
                    raw = parts[0].strip()
                    comment = parts[1] if len(parts) > 1 else ""
                    if _SM_PAT.search(comment):
                        sm_addrs.add(addr)
                    ann = {reg: name for reg, name in _OVERRIDE_PAT.findall(comment)}
                    if ann:
                        overrides[addr] = ann
                    if _FUNC_PAT.match(line):
                        funcs[addr] = len(instrs)
                    elif line.startswith("*"):
                        stars[addr] = len(instrs)
                    if pending_label is not None:
                        labels[pending_label] = addr
                        pending_label = None
                    instrs.append((addr, raw))
                else:
                    pending_label = None

    return instrs, labels, funcs, stars, sm_addrs, overrides


def extract_function(instrs, funcs, stars, start_addr):
    idx = funcs.get(start_addr) if start_addr in funcs else stars.get(start_addr)
    if idx is None:
        return None
    result = []
    for i in range(idx, len(instrs)):
        addr, raw = instrs[i]
        if i != idx and addr in funcs:
            break
        result.append((addr, raw))
    return result


# ---------------------------------------------------------------------------
# Z80 -> (reads, writes, note)
# ---------------------------------------------------------------------------

_PAIRS = ("BC", "DE", "HL", "IX", "IY", "SP", "AF")
_PAIR_HIGH = {"BC": "B", "DE": "D", "HL": "H", "AF": "A", "IX": "IXh", "IY": "IYh"}
_PAIR_LOW = {"BC": "C", "DE": "E", "HL": "L", "AF": "F", "IX": "IXl", "IY": "IYl"}
# Inverse: component register -> its parent pair
_COMP_PAIR = {v: k for k, v in _PAIR_HIGH.items()}
_COMP_PAIR.update({v: k for k, v in _PAIR_LOW.items()})
_SHADOWS = ("BC'", "DE'", "HL'", "AF'", "A'", "F'")
_SINGLES = ("IXh", "IXl", "IYh", "IYl", "A", "B", "C", "D", "E", "H", "L", "F")
_ALL_REGS = _PAIRS + _SHADOWS + _SINGLES
# scan order: longest/most specific first to avoid substring false matches
_SCAN_ORDER = (
    "IXh",
    "IXl",
    "IYh",
    "IYl",
    "IX",
    "IY",
    "SP",
    "BC'",
    "DE'",
    "HL'",
    "AF'",
    "A'",
    "F'",
    "BC",
    "DE",
    "HL",
    "AF",
    "A",
    "B",
    "C",
    "D",
    "E",
    "H",
    "L",
    "F",
)
_CONDS = frozenset(("NZ", "Z", "NC", "C", "PO", "PE", "P", "M"))
_PAIR_SET = frozenset(_PAIRS)


def _regs_in(s):
    """Registers directly (non-indirectly) referenced in operand string s."""
    regs = set()
    # indirect pair addresses: (HL) (BC) (DE) (SP)
    for pair in ("BC", "DE", "HL", "SP"):
        if "(%s)" % pair in s:
            regs.add(pair)
    # indirect IX/IY offsets
    if re.search(r"\(IX[+-]", s):
        regs.add("IX")
    if re.search(r"\(IY[+-]", s):
        regs.add("IY")
    # direct references: strip parenthetical content and hex literals first
    plain = re.sub(r"\([^)]*\)", "", s)
    plain = re.sub(r"\$[0-9A-Fa-f]+", "", plain)
    for reg in _SCAN_ORDER:
        if re.search(r"(?<![A-Za-z\'])" + re.escape(reg) + r"(?![A-Za-z\'])", plain):
            regs.add(reg)
    return regs


def _dest_reg(s):
    """The single register written by a destination operand (or None)."""
    s = s.strip()
    if "(" in s:
        return None  # indirect: no register written
    for reg in _SCAN_ORDER:
        if re.fullmatch(re.escape(reg), s):
            return reg
    return None


def reg_rw(raw):
    """Return (reads:set, writes:set, note:str) for one instruction."""
    parts = raw.split(None, 1)
    if not parts:
        return set(), set(), ""
    mnem = parts[0].upper().rstrip(",")
    ops = parts[1].strip() if len(parts) > 1 else ""

    # split on top-level comma (not inside parens)
    operands = []
    depth = 0
    cur = []
    for ch in ops:
        if ch == "(":
            depth += 1
        elif ch == ")":
            depth -= 1
        if ch == "," and depth == 0:
            operands.append("".join(cur).strip())
            cur = []
        else:
            cur.append(ch)
    if cur:
        operands.append("".join(cur).strip())
    dst = operands[0].upper() if operands else ""
    src = operands[1].upper() if len(operands) > 1 else ""

    # --- EXX / EX AF,AF' ---
    if mnem == "EXX":
        return set(), set(), "EXX"
    if mnem == "EX":
        if "AF'" in ops.upper():
            return set(), set(), "EX AF,AF'"
        if "DE" in ops.upper() and "HL" in ops.upper():
            return set(), set(), "EX DE,HL"
        # EX (SP),HL — stack exchange; treat as mutual read+write of HL and SP
        return (
            _regs_in(dst) | _regs_in(src) | {"SP"},
            _regs_in(dst) | _regs_in(src) | {"SP"},
            "",
        )

    # --- trivial ---
    if mnem in ("NOP", "HALT", "DI", "EI"):
        return set(), set(), ""
    if mnem in ("SCF", "CCF"):
        return set(), {"F"}, ""
    if mnem == "RLCA":
        return {"A"}, {"A", "F"}, ""
    if mnem == "RRCA":
        return {"A"}, {"A", "F"}, ""
    if mnem == "RLA":
        return {"A", "F"}, {"A", "F"}, ""
    if mnem == "RRA":
        return {"A", "F"}, {"A", "F"}, ""
    if mnem == "CPL":
        return {"A"}, {"A", "F"}, ""
    if mnem == "NEG":
        return {"A"}, {"A", "F"}, ""
    if mnem == "DAA":
        return {"A", "F"}, {"A", "F"}, ""

    # --- block moves ---
    if mnem in ("LDIR", "LDDR", "LDI", "LDD"):
        return {"HL", "DE", "BC"}, {"HL", "DE", "BC", "F"}, ""
    if mnem in ("CPIR", "CPDR", "CPI", "CPD"):
        return {"A", "HL", "BC"}, {"HL", "BC", "F"}, ""
    if mnem in ("INIR", "INDR", "INI", "IND"):
        return {"B", "C", "HL"}, {"B", "HL", "F"}, ""
    if mnem in ("OTIR", "OTDR", "OUTI", "OUTD"):
        return {"B", "C", "HL"}, {"B", "HL", "F"}, ""

    # --- LD ---
    if mnem == "LD":
        writes = set()
        dr = _dest_reg(dst)
        if dr:
            writes.add(dr)
        dst_addr_reads = _regs_in(dst) - (writes if dr else set())
        src_reads = _regs_in(src)
        reads = src_reads | dst_addr_reads
        return reads, writes, ""

    # --- arithmetic ---
    if mnem in ("ADD", "ADC", "SUB", "SBC"):
        if dst in ("HL", "IX", "IY"):
            r = _regs_in(src) | {dst}
            if mnem in ("ADC", "SBC"):
                r.add("F")
            return r, {dst, "F"}, ""
        # 8-bit: A is implicit when only one operand
        arg = src if src else dst
        r = _regs_in(arg) | {"A"}
        if mnem in ("ADC", "SBC"):
            r.add("F")
        return r, {"A", "F"}, ""
    if mnem in ("AND", "OR", "XOR"):
        arg = src if src else dst
        return _regs_in(arg) | {"A"}, {"A", "F"}, ""
    if mnem == "CP":
        arg = src if src else dst
        return _regs_in(arg) | {"A"}, {"F"}, ""
    if mnem == "SUB":
        arg = src if src else dst
        return _regs_in(arg) | {"A"}, {"A", "F"}, ""

    # --- INC / DEC ---
    if mnem in ("INC", "DEC"):
        r = _regs_in(dst)
        dr = _dest_reg(dst)
        is_pair = dr in _PAIR_SET if dr else False
        w = r.copy()
        if not is_pair:
            w.add("F")
        if "(" in dst:  # indirect (HL)/(IX+d): only address reg read, F written
            w = {"F"}
        return r, w, ""

    # --- stack ---
    if mnem == "PUSH":
        return _regs_in(dst) | {"SP"}, {"SP"}, ""
    if mnem == "POP":
        dr = _dest_reg(dst)
        return {"SP"}, ({dr} if dr else set()) | {"SP"}, ""

    # --- rotate/shift ---
    if mnem in ("RLC", "RRC", "RL", "RR", "SLA", "SRA", "SRL", "SLL"):
        arg = src if src else dst  # CB prefix: may be SET r,(IX+d)
        r = _regs_in(arg)
        if mnem in ("RL", "RR"):
            r.add("F")
        return r, r | {"F"}, ""

    # --- bit ops ---
    if mnem == "BIT":
        arg = src if src else dst
        return _regs_in(arg), {"F"}, ""
    if mnem in ("SET", "RES"):
        arg = src if src else dst
        r = _regs_in(arg)
        return r, r, ""

    # --- jumps ---
    if mnem in ("JP", "JR"):
        if dst in _CONDS:
            return {"F"} | _regs_in(src), set(), ""
        return _regs_in(dst), set(), ""
    if mnem == "DJNZ":
        return {"B", "F"}, {"B", "F"}, ""

    # --- calls / returns ---
    if mnem in ("CALL", "RST"):
        r = {"SP"}
        if dst in _CONDS:
            r.add("F")
        return r, {"SP"}, ""
    if mnem == "RET":
        if dst:
            return {"F", "SP"}, {"SP"}, ""
        return {"SP"}, {"SP"}, ""
    if mnem == "RETI":
        return {"SP"}, {"SP"}, ""
    if mnem == "RETN":
        return {"SP", "IFF2"}, {"SP"}, ""

    # --- I/O ---
    if mnem == "IN":
        dr = _dest_reg(dst)
        return {"A", "C"}, ({dr} if dr else set()) | {"F"}, ""
    if mnem == "OUT":
        return {"A", "C"} | _regs_in(src), set(), ""

    # --- misc ---
    if mnem in ("LD", "EX"):
        pass  # handled above; shouldn't reach here

    return {"?"}, {"?"}, ""


# ---------------------------------------------------------------------------
# Variable naming — one unique name per write site
# ---------------------------------------------------------------------------

_VERBS = [
    "bite",
    "blend",
    "boil",
    "core",
    "crush",
    "eat",
    "grate",
    "grind",
    "juice",
    "mash",
    "peel",
    "pit",
    "poach",
    "slice",
    "zest",
]
_FRUITS = [
    "apple",
    "bergamot",
    "cherry",
    "damson",
    "fig",
    "guava",
    "kiwi",
    "kumquat",
    "lemon",
    "lime",
    "mango",
    "melon",
    "orange",
    "peach",
    "pear",
    "plum",
    "prune",
    "quince",
    "satsuma",
    "tangerine",
]


def _var_name(addr, reg):
    import hashlib

    h = hashlib.md5(("%04X%s" % (addr, reg)).encode()).digest()
    return "%s_%s" % (_VERBS[h[0] % len(_VERBS)], _FRUITS[h[1] % len(_FRUITS)])


def _imm_value(src):
    """Return the immediate string if src is a pure immediate operand, else None."""
    s = src.strip()
    if re.fullmatch(r"\$[0-9A-Fa-f]+", s):
        return s
    if re.fullmatch(r"\d+", s):
        return s
    return None


def _parse_const(name):
    """Parse '$XX', '$XXXX', or decimal variable name to int, else None."""
    if not name:
        return None
    m = re.fullmatch(r"\$([0-9A-Fa-f]+)", name)
    if m:
        return int(m.group(1), 16)
    m = re.fullmatch(r"\d+", name)
    if m:
        return int(m.group(0))
    return None


def _fmt_const(val, bits):
    """Format an integer as '$XX' (8-bit) or '$XXXX' (16-bit)."""
    return ("$%04X" if bits == 16 else "$%02X") % (val & ((1 << bits) - 1))


def _fold_consts(mnem, oplist, live_vars):
    """Return {reg: const_name} for writes that produce a statically-known constant.

    Only the registers whose result can be determined without runtime data are
    included; F is never folded (callers assign it a fresh name as usual).
    """
    result = {}
    dst_op = oplist[0] if oplist else ""
    src_op = oplist[1] if len(oplist) > 1 else ""

    # XOR A / SUB A always zero A regardless of its prior value.
    if mnem in ("XOR", "SUB") and dst_op == "A" and not src_op:
        result["A"] = "$00"
        return result

    # INC / DEC on a constant register (8-bit or 16-bit).
    if mnem in ("INC", "DEC"):
        dr = _dest_reg(dst_op)
        if dr:
            bits = 16 if dr in _PAIR_SET else 8
            cur = _parse_const(live_vars.get(dr))
            if cur is not None:
                result[dr] = _fmt_const(cur + (1 if mnem == "INC" else -1), bits)
        return result

    # Rotate / complement / negate on constant A.
    if mnem == "RLCA":
        cur = _parse_const(live_vars.get("A"))
        if cur is not None:
            result["A"] = _fmt_const((cur << 1) | (cur >> 7), 8)
        return result
    if mnem == "RRCA":
        cur = _parse_const(live_vars.get("A"))
        if cur is not None:
            result["A"] = _fmt_const((cur >> 1) | (cur << 7), 8)
        return result
    if mnem == "CPL":
        cur = _parse_const(live_vars.get("A"))
        if cur is not None:
            result["A"] = _fmt_const(~cur, 8)
        return result
    if mnem == "NEG":
        cur = _parse_const(live_vars.get("A"))
        if cur is not None:
            result["A"] = _fmt_const(-cur, 8)
        return result

    # Logical / arithmetic on A with an immediate operand.
    if mnem in ("AND", "OR", "XOR", "ADD", "SUB"):
        arg_op = src_op if src_op else dst_op
        imm_str = _imm_value(arg_op)
        if imm_str:
            imm_val = _parse_const(imm_str)
            cur = _parse_const(live_vars.get("A"))
            if cur is not None and imm_val is not None:
                ops_map = {
                    "AND": cur & imm_val,
                    "OR": cur | imm_val,
                    "XOR": cur ^ imm_val,
                    "ADD": cur + imm_val,
                    "SUB": cur - imm_val,
                }
                result["A"] = _fmt_const(ops_map[mnem], 8)
        return result

    # ADD HL/IX/IY, pair when both operands are constants.
    if mnem == "ADD" and dst_op in ("HL", "IX", "IY"):
        cur_dst = _parse_const(live_vars.get(dst_op))
        cur_src = _parse_const(live_vars.get(src_op))
        if cur_dst is not None and cur_src is not None:
            result[dst_op] = _fmt_const(cur_dst + cur_src, 16)
        return result

    return result


def _var_colour(vname):
    import hashlib

    h = hashlib.md5(vname.encode()).digest()
    hue = (h[0] * 256 + h[1]) / 65536.0
    return '"%0.3f 0.75 0.85"' % hue


# ---------------------------------------------------------------------------
# Lifetime engine
# ---------------------------------------------------------------------------


def compute(instructions, sm_addrs=None, overrides=None, jump_targets=None):
    """Return (rows, param_vars).

    rows: list of (addr, raw, reads, writes, live_before, note, read_vars, write_vars).
    param_vars: dict var_name -> reg for registers read before any write (inbound params).

    read_vars:  dict reg -> var_name for each register read this instruction.
    write_vars: dict reg -> var_name for each register written this instruction.

    sm_addrs:  set of addresses whose immediate operand is self-modified at runtime.
    overrides: dict addr -> {reg: name} from '{Reg=name}' comment annotations;
               these take priority over all computed names.
    """
    if sm_addrs is None:
        sm_addrs = set()
    if overrides is None:
        overrides = {}
    if jump_targets is None:
        jump_targets = set()
    live = set()
    live_vars = {}  # reg -> current var name
    param_vars = {}  # var_name -> reg (inbound params, assigned on first read)
    var_stack = []  # pushed variable names for PUSH/POP tracking
    rows = []

    def _swap_vars(main, shadow):
        vm = live_vars.pop(main, None)
        vs = live_vars.pop(shadow, None)
        if vm:
            live_vars[shadow] = vm
        if vs:
            live_vars[main] = vs

    for addr, raw in instructions:
        reads, writes, note = reg_rw(raw)

        if note == "EXX":
            rows.append((addr, raw, set(), set(), frozenset(live), note, {}, {}))
            for main, shadow in (("BC", "BC'"), ("DE", "DE'"), ("HL", "HL'")):
                had_main = main in live
                had_shadow = shadow in live
                live.discard(main)
                live.discard(shadow)
                if had_main:
                    live.add(shadow)
                if had_shadow:
                    live.add(main)
                _swap_vars(main, shadow)
            continue

        if note == "EX AF,AF'":
            rows.append((addr, raw, set(), set(), frozenset(live), note, {}, {}))
            for main, shadow in (("A", "A'"), ("F", "F'")):
                had_main = main in live
                had_shadow = shadow in live
                live.discard(main)
                live.discard(shadow)
                if had_main:
                    live.add(shadow)
                if had_shadow:
                    live.add(main)
                _swap_vars(main, shadow)
            continue

        if note == "EX DE,HL":
            rows.append((addr, raw, set(), set(), frozenset(live), note, {}, {}))
            for a, b in (("DE", "HL"), ("D", "H"), ("E", "L")):
                had_a = a in live
                had_b = b in live
                live.discard(a)
                live.discard(b)
                if had_a:
                    live.add(b)
                if had_b:
                    live.add(a)
                _swap_vars(a, b)
            continue

        # At jump targets (starred addresses), accumulated constants are unreliable
        # because we don't know which path reached this point.  Clear them before
        # processing so folding has no stale constants to propagate.
        is_jump_target = addr in jump_targets
        if is_jump_target:
            for reg in [r for r, n in list(live_vars.items()) if _is_constant(n)]:
                live.discard(reg)
                live_vars.pop(reg)

        # Parse mnemonic for copy and stack detection.
        _parts = raw.split(None, 1)
        _mnem = _parts[0].upper() if _parts else ""
        _ops = _parts[1].strip().upper() if len(_parts) > 1 else ""
        _oplist = [o.strip() for o in _ops.split(",", 1)]

        # Copy propagation: LD dest_reg, src_reg — dest inherits src's var name.
        # Immediate detection: LD dest_reg, $XX — dest gets the constant as its name.
        # Constant folding: arithmetic on known-constant registers.
        is_sm = addr in sm_addrs
        copy_from = {}
        imm_for = {}  # reg -> immediate string (e.g. "$E0")
        # Suppress folding at SM addresses and jump targets; fresh immediate
        # loads (imm_for) are always valid so only sm suppresses those.
        const_result = {} if (is_sm or is_jump_target) else _fold_consts(_mnem, _oplist, live_vars)
        if _mnem == "LD" and len(_oplist) == 2:
            dr = _dest_reg(_oplist[0])
            sr = _dest_reg(_oplist[1])
            if dr and sr:
                copy_from[dr] = sr
            elif dr and not is_sm:
                imm = _imm_value(_oplist[1])
                if imm:
                    imm_for[dr] = imm

        # PUSH: record pair's variable name on the stack before processing writes.
        if _mnem == "PUSH":
            pair = _ops.strip()
            pushed = (
                live_vars.get(pair)
                or live_vars.get(_PAIR_HIGH.get(pair))
                or live_vars.get(_PAIR_LOW.get(pair))
            )
            var_stack.append(pushed)

        # POP: retrieve variable name for the destination pair.
        pop_var = None
        pop_dest = None
        if _mnem == "POP" and var_stack:
            pop_var = var_stack.pop()
            pop_dest = _ops.strip()

        live_before = frozenset(live)
        read_vars = {}
        for r in reads:
            if r not in live_vars:
                parent = _COMP_PAIR.get(r)
                hi = _PAIR_HIGH.get(r)
                lo = _PAIR_LOW.get(r)
                if parent and parent in live_vars:
                    # Component read with no prior write: inherit from parent pair.
                    live_vars[r] = live_vars[parent]
                elif hi or lo:
                    hi_name = live_vars.get(hi) if hi else None
                    lo_name = live_vars.get(lo) if lo else None
                    if lo_name and _parse_const(hi_name) == 0:
                        # Zero-extension widening: high byte is $00, pair is
                        # the low-byte value zero-extended to 16 bits.
                        live_vars[r] = lo_name
                    elif hi_name:
                        live_vars[r] = hi_name
                    elif lo_name:
                        live_vars[r] = lo_name
                    else:
                        pname = _var_name(0xFFFF, r)
                        live_vars[r] = pname
                        param_vars[pname] = r
                else:
                    pname = _var_name(0xFFFF, r)
                    live_vars[r] = pname
                    param_vars[pname] = r
            read_vars[r] = live_vars[r]
        write_vars = {}
        for r in writes:
            if r in const_result:
                live.discard(r)
                live_vars.pop(r, None)
                name = const_result[r]  # constant folding
            elif r in reads:
                name = live_vars.get(r) or _var_name(addr, r)  # mutation
            elif r in copy_from:
                name = live_vars.get(copy_from[r]) or _var_name(addr, r)  # copy
            elif pop_var and r == pop_dest:
                name = pop_var  # stack pop
            elif r in imm_for:
                live.discard(r)
                live_vars.pop(r, None)
                name = imm_for[r]  # immediate constant
            else:
                live.discard(r)
                live_vars.pop(r, None)
                name = _var_name(addr, r)  # fresh
            write_vars[r] = name
            live_vars[r] = name
        # Pair↔component write coherence.
        # Any write to a pair (fresh or mutation) kills its components — their
        # names are now stale relative to the updated pair.
        # Any write to a component kills its parent pair (always stale after that).
        for r in writes:
            hi = _PAIR_HIGH.get(r)
            lo = _PAIR_LOW.get(r)
            if hi:
                for comp in (hi, lo):
                    live.discard(comp)
                    live_vars.pop(comp, None)
            parent = _COMP_PAIR.get(r)
            if parent:
                live.discard(parent)
                live_vars.pop(parent, None)
        live |= writes

        # Apply {Reg=name} comment overrides — highest priority, beats everything.
        ann = overrides.get(addr)
        if ann:
            for reg, name in ann.items():
                old = live_vars.get(reg)
                live_vars[reg] = name
                live.add(reg)
                if reg in write_vars:
                    write_vars[reg] = name
                elif reg in read_vars:
                    read_vars[reg] = name
                # If this register was a param under its old name, retitle it.
                if old and old in param_vars:
                    param_vars[name] = param_vars.pop(old)

        rows.append((addr, raw, reads, writes, live_before, "", read_vars, write_vars))

    return rows, param_vars


# ---------------------------------------------------------------------------
# Output
# ---------------------------------------------------------------------------

_REG_ORDER = [
    "A",
    "A'",
    "F",
    "F'",
    "AF",
    "AF'",
    "B",
    "C",
    "BC",
    "BC'",
    "D",
    "E",
    "DE",
    "DE'",
    "H",
    "L",
    "HL",
    "HL'",
    "IXh",
    "IXl",
    "IX",
    "IYh",
    "IYl",
    "IY",
    "SP",
]


def _fmt(s):
    if not s:
        return "-"
    ordered = [r for r in _REG_ORDER if r in s]
    rest = sorted(s - set(ordered))
    return " ".join(ordered + rest)


def _fmt_vars(regs, var_map):
    if not regs:
        return "-"
    ordered = [r for r in _REG_ORDER if r in regs]
    rest = sorted(regs - set(ordered))
    parts = []
    for r in ordered + rest:
        v = var_map.get(r)
        parts.append("%s:%s" % (r, v) if v else r)
    return " ".join(parts)


def print_table(rows, jump_targets=None):
    if jump_targets is None:
        jump_targets = set()
    w = max((len(r[1]) for r in rows), default=20)
    w = max(w, 20)
    hdr = "%-7s %-*s  %-22s  %-22s  %s" % (
        "ADDR",
        w,
        "INSTRUCTION",
        "READS",
        "WRITES",
        "LIVE BEFORE",
    )
    print(hdr)
    print("-" * len(hdr))
    for addr, raw, reads, writes, live_before, note, read_vars, write_vars in rows:
        prefix = "*" if addr in jump_targets else " "
        if note:
            print("%s$%04X %-*s  --- %s ---" % (prefix, addr, w, raw, note))
        else:
            print(
                "%s$%04X %-*s  %-22s  %-22s  %s"
                % (
                    prefix,
                    addr,
                    w,
                    raw,
                    _fmt_vars(reads, read_vars),
                    _fmt_vars(writes, write_vars),
                    _fmt(live_before),
                )
            )


_FLAG_REGS = frozenset(("F", "F'"))


def _is_constant(vname):
    return bool(re.fullmatch(r"\$[0-9A-Fa-f]+|\d+", vname))


def _html(s):
    return (
        s.replace("&", "&amp;")
        .replace("<", "&lt;")
        .replace(">", "&gt;")
        .replace('"', "&quot;")
    )


def print_dot(rows, label, param_vars):
    # Collect write site, read sites, and source register per variable (two-pass).
    # Write/read sites are port names ("n0", "n1", …) within the single table node.
    var_write = {}  # vname -> port id
    var_reads = {}  # vname -> [port id, ...]
    var_reg = {}  # vname -> register name

    if param_vars:
        for vname, reg in param_vars.items():
            var_write[vname] = "params"
            var_reg[vname] = reg

    for i, (
        _addr,
        _raw,
        _reads,
        _writes,
        _live,
        _note,
        read_vars,
        write_vars,
    ) in enumerate(rows):
        port = "n%d" % i
        for reg, vname in write_vars.items():
            var_write[vname] = port
            var_reg[vname] = reg
        for vname in read_vars.values():
            if vname in var_write and var_write[vname] != port:
                var_reads.setdefault(vname, []).append(port)

    live_vars = {v for v, readers in var_reads.items() if readers}

    print('digraph "%s" {' % label)
    print("  rankdir=LR;")
    print("  node [fontname=Courier fontsize=10];")

    # PARAMS node.
    if param_vars:
        reg_list = "\\n".join(
            "%s: %s" % (reg, vname)
            for vname, reg in sorted(param_vars.items(), key=lambda kv: kv[1])
        )
        print(
            '  params [label="PARAMS\\n%s" shape=oval style=filled fillcolor=lightgreen];'
            % reg_list
        )

    # Single HTML table node for all instructions.
    print("  instr [shape=none margin=0 label=<")
    print('    <TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0" CELLPADDING="3">')
    for i, (addr, raw, _r, _w, _live, note, _rv, _wv) in enumerate(rows):
        bg = ' BGCOLOR="lightyellow"' if note else ""
        print(
            '      <TR><TD PORT="n%d" ALIGN="LEFT"%s>$%04X&nbsp;&nbsp;%s</TD></TR>'
            % (i, bg, addr, _html(raw))
        )
    print("    </TABLE>>];")

    # Variable nodes and their edges.
    print("  node [fontsize=8 width=1 height=1 fixedsize=true];")
    for vname in sorted(live_vars):
        if _is_constant(vname):
            col = '"0.000 0.000 0.850"'  # light grey
            shape = "box"
        elif var_reg.get(vname) in _FLAG_REGS:
            col = _var_colour(vname)
            shape = "circle"
        else:
            col = _var_colour(vname)
            shape = "diamond"
        vid = "v_%s" % re.sub(r"[^A-Za-z0-9]", "_", vname)
        print(
            '  %s [label="%s" shape=%s style=filled fillcolor=%s];'
            % (vid, vname, shape, col)
        )
        src = var_write[vname]
        src_port = ("instr:%s:e" % src) if src != "params" else "params"
        print("  %s -> %s [color=%s];" % (src_port, vid, col))
        for dst in var_reads[vname]:
            print("  %s -> instr:%s:e [color=%s];" % (vid, dst, col))

    print("}")


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------


def main():
    ap = argparse.ArgumentParser(
        description="Show Z80 register lifetimes for one function from a skool file."
    )
    ap.add_argument("target", help="Label name or $XXXX address")
    ap.add_argument(
        "--skool",
        action="append",
        metavar="FILE",
        help="Skool file to search; repeat for multiple files (default: *.skool in script dir)",
    )
    ap.add_argument(
        "--dot", action="store_true", help="Emit Graphviz dot instead of text table"
    )
    args = ap.parse_args()

    if args.skool:
        paths = args.skool
    else:
        here = os.path.dirname(os.path.abspath(__file__))
        paths = sorted(glob.glob(os.path.join(here, "*.skool")))
        if not paths:
            sys.exit("No .skool files found in %s" % here)

    instrs, labels, funcs, stars, sm_addrs, overrides = parse_skool(paths)

    target = args.target
    if target.startswith("$"):
        try:
            addr = int(target[1:], 16)
        except ValueError:
            sys.exit("Bad address: %s" % target)
    elif target in labels:
        addr = labels[target]
    else:
        close = [k for k in labels if target.lower() in k.lower()][:8]
        hint = ("  Similar: " + ", ".join(close)) if close else ""
        sys.exit("Label not found: %s.%s" % (target, hint))

    fn = extract_function(instrs, funcs, stars, addr)
    if fn is None:
        sys.exit(
            "No function entry at $%04X (is it a c$ or *$ line in the skool?)" % addr
        )

    rows, param_vars = compute(fn, sm_addrs, overrides, set(stars))

    if args.dot:
        print_dot(rows, target, param_vars)
    else:
        print_table(rows, set(stars))


if __name__ == "__main__":
    main()
