#!/usr/bin/env python3
#
# CHQStage.py
#
# Chase H.Q. data decoder
#

# Create binary using:
# skool2bin.py ChaseHQ-128K-bank-1.skool -S 49152 -E 57344
#

import getopt
import os
import sys

def usage():
    print('Usage: CHQStage.py --stage STAGE --base BASEADDR LEVELDATA')
    sys.exit(1)

def main(args):

    BASE = 0x5C00  # once relocated
    LEN  = 0x2000  # 8K per level

    """ Add a block at 0x5C00+ """
    def add(blocktype, addr, desc, nbytes = -1, widthbytes = -1):
        if BASE <= addr < BASE + LEN:
            if blocktype.islower():
                prefix = "[Stage %d] " % (STAGE)
            else:
                prefix = ""
            locations.append((blocktype, addr, prefix + desc, nbytes, widthbytes))
        else:
            # error
            print(hex(addr), desc)
            assert False

    def addjob(kind, addr):
        if not (kind, addr) in jobs:
            jobs.append((kind, addr))

    def byteat(addr):
        assert BASE <= addr < BASE + LEN
        return ram[addr - BASE]

    def wordat(addr):
        return byteat(addr + 0) + byteat(addr + 1) * 256

    def topbitstring(addr):
        s = ""
        while (byteat(addr) < 0x80):
          s += chr(byteat(addr))
          addr += 1
        s += chr(byteat(addr) - 0x80)
        return s

    def topbitstrlen(addr):
        c = 0
        while (byteat(addr) < 0x80):
          assert c < 100
          c += 1
          addr += 1
        return c + 1

    def decode_nibble_rle(base, typename, names):
        runlength = 0
        lasttype = -1
        basep = base
        commentaddr = basep
        while 1:
            b = byteat(basep)
            basep = basep + 1
            if b == 0: # Escape
                if lasttype != -1:
                    add("B", commentaddr, "%s for %d units" % (names[lasttype], runlength))
                    commentaddr = basep - 1
                b = byteat(basep)
                basep = basep + 1
                if b == 0: # Jump
                    loopdest = wordat(basep)
                    if loopdest == base:
                        add("B", commentaddr, "<Esc> Loop")
                    else:
                        add("B", commentaddr, "<Esc> Jump")
                    add("W", commentaddr + 2, "Target")
                elif b == 1: # Fork End
                    add("B", commentaddr, "<Esc> Fork End")
                elif b == 2: # Split
                    leftdest  = wordat(basep + 0)
                    rightdest = wordat(basep + 2)
                    add("B", commentaddr, "<Esc> Split")
                    add("W", commentaddr + 2, "Left target")
                    add("W", commentaddr + 4, "Right target")
                else:
                    assert False
                return
            else:
                count = (b & 0xF0) >> 4
                type_ = (b & 0x0F)
                if type_ == lasttype or lasttype == -1:
                    lasttype = type_
                    runlength += count
                else:
                    add("B", commentaddr, "%s for %d units" % (names[lasttype], runlength))
                    commentaddr = basep - 1
                    runlength = count
                    lasttype = type_
        
    def decode_count_rle(base, typename, names):
        runlength = 0
        lasttype = -1
        basep = base
        commentaddr = basep
        while 1:
            b = byteat(basep)
            basep = basep + 1
            if b == 0: # Escape
                if lasttype != -1:
                    add("B", commentaddr, "%s for %d units" % (names.get(lasttype, "[%2X]" % lasttype), runlength))
                    commentaddr = basep - 1
                b = byteat(basep)
                basep = basep + 1
                if b == 0: # Jump
                    loopdest = wordat(basep)
                    if loopdest == base:
                        add("B", commentaddr, "<Esc> Loop")
                    else:
                        add("B", commentaddr, "<Esc> Jump")
                    add("W", commentaddr + 2, "Target")
                elif b == 1: # Fork End
                    add("B", commentaddr, "<Esc> Fork End")
                elif b == 2: # Split
                    leftdest  = wordat(basep + 0)
                    rightdest = wordat(basep + 2)
                    add("B", commentaddr, "<Esc> Split")
                    add("W", commentaddr + 2, "Left target")
                    add("W", commentaddr + 4, "Right target")
                else:
                    assert False
                return
            else:
                count = b
                type_ = byteat(basep)
                basep = basep + 1
                if type_ == lasttype or lasttype == -1:
                    lasttype = type_
                    runlength += count
                else:
                    add("B", commentaddr, "%s for %d units" % (names.get(lasttype, "[%2X]" % lasttype), runlength))
                    commentaddr = basep - 2
                    runlength = count
                    lasttype = type_

    def decode_hazards(base, typename, names):
        count = 0
        basep = base
        commentaddr = basep
        while 1:
            b = byteat(basep)
            basep = basep + 1
            if b == 0: # Escape
                if count > 0:
                    add("B", commentaddr, "Wait for %d units" % (count))
                    commentaddr = basep + 1
                b = byteat(basep)
                basep = basep + 1
                if b == 0: # Jump
                    loopdest = wordat(basep)
                    if loopdest == base:
                        add("B", commentaddr, "<Esc> Loop")
                    else:
                        add("B", commentaddr, "<Esc> Jump")
                    add("W", commentaddr + 2, "Target")
                elif b == 1: # Fork End
                    add("B", commentaddr, "<Esc> Fork End")
                elif b == 2: # Split
                    leftdest  = wordat(basep + 0)
                    rightdest = wordat(basep + 2)
                    add("B", commentaddr, "<Esc> Split")
                    add("W", commentaddr + 2, "Left target")
                    add("W", commentaddr + 4, "Right target")
                elif b == 3: # TBD Stop Spawning Barriers?
                    add("B", commentaddr, "Stop Spawning Barriers 3?")
                elif b == 6: # TBD Stop Spawning Barriers?
                    add("B", commentaddr, "Stop Spawning Barriers 6?")
                elif b == 7: # Start Spawning Barriers Left
                    add("B", commentaddr, "Start Spawning Barriers Left")
                elif b == 8: # Start Spawning Barriers Right
                    add("B", commentaddr, "Start Spawning Barriers Right")
                elif b == 9: # Start Spawning Two Barriers
                    add("B", commentaddr, "Start Spawning Two Barriers")
                elif b == 10: # Set Floating Arrow Off
                    add("B", commentaddr, "Set Floating Arrow Off")
                elif b == 11: # Set Floating Arrow to Left
                    add("B", commentaddr, "Set Floating Arrow to Left")
                elif b == 12: # Set Floating Arrow to Right
                    add("B", commentaddr, "Set Floating Arrow to Right")
                elif b == 13: # Enable Car Spawning
                    add("B", commentaddr, "Enable Car Spawning")
                elif b == 14: # Disable Car Spawning
                    add("B", commentaddr, "Disable Car Spawning")
                else:
                    add("B", commentaddr, "Unknown command %X" % (b))
                if b <= 2:
                    return
                commentaddr = basep
            else:
                count = b
                add("B", commentaddr, "Wait for %d units" % (count))
                commentaddr = basep
                count = 0

    try:
        opts,files = getopt.getopt(args, 's:b:', ['stage=', 'base='])
    except getopt.GetoptError:
        usage()

    STAGE = -1
    ORIGBASE = -1

    for o, a in opts:
        if o in ("-s", "--stage"):
            STAGE = eval(a)
        elif o in ("-b", "--base"):
            ORIGBASE = eval(a)
        else:
            assert False, "unhandled option "+o

    if STAGE < 0 or ORIGBASE < 0:
        usage()

    if len(files) != 1:
        usage()

    jobs = []
    locations = []

    # load 8K blob of stage data
    with open(files[0], mode='rb') as file:
        ram = file.read()

    # check length

    # add funcs for reading its bytes (assuming it lives at $5C00)

    # insert some root entries in the job queue
    jobs.append(("horizon_graphic", 0x5C00))
    jobs.append(("per_stage_data", 0x5CF0))
    jobs.append(("table_of_lods", 0x5D0C))
    jobs.append(("difficulty", 0x5D1A))
    jobs.append(("setup_data", 0x5D1D))
    jobs.append(("attract_data", 0x5D2B))

    # iterate over job queue
    while jobs:
        job,addr = jobs.pop(0)
        match job:
            case "horizon_graphic":
                add("b", addr, "Horizon graphic")

            case "per_stage_data":
                add("b", addr, "Per-stage data")
                add("W", addr + 0, "Address of PERP's mugshot attributes")
                add("W", addr + 2, "Address of PERP's mugshot bitmap")
                add("W", addr + 4, "Screen attributes used for the ground colour (a pair of matching bytes)")
                add("W", addr + 6, "Address of table of LODs for tumbleweeds, barriers.")
                add("W", addr + 8, "points to objects?")
                add("W", addr + 10, "points to objects?")
                add("W", addr + 12, "points to objects?")
                add("W", addr + 14, "Address of turn sign arg and handler address")
                add("W", addr + 16, "Address of graphics entry 10")
                add("W", addr + 18, "Address of graphics entry 12")
                add("W", addr + 20, "Address of Nancy's perp description")
                add("W", addr + 22, "Address of arrest messages")
                add("W", addr + 24, "Helicopter related 1")
                add("W", addr + 26, "Helicopter related 2")
                
                addjob("hittable_objects", wordat(addr + 6))
                addjob("nancy_perp_desc",  wordat(addr + 20))
                addjob("arrest_messages",  wordat(addr + 22))

            case "table_of_lods":
                add("w", addr, "Table of addresses of LODs")
                add("W", addr + 0, "Address of LOD of stone/dust?")
                add("W", addr + 2, "Address of LOD of stone/dust?")
                add("W", addr + 4, "Address of LOD of car (perp's car)")
                add("W", addr + 6, "Address of LOD of lambo?")
                add("W", addr + 8, "Address of LOD of truck?")
                add("W", addr + 10, "Address of LOD of lambo again?")
                add("W", addr + 12, "Address of LOD of car (generic car)?")

                for lod_addr in range(addr + 0, addr + 14, 2):
                    lod = wordat(lod_addr)
                    if lod > 0:
                        addjob("lod", lod)
            
            case "difficulty":
                add("b", addr, "Per-stage difficulty settings")
                add("B", addr + 0, "How often cars spawn. Lower values spawn cars more often.")
                add("B", addr + 1, "smash related")
                add("B", addr + 2, "smash related")

            case "setup_data":
                add("w", addr, "Per-stage setup data")
                add("W", addr + 0, "road_pos")
                add("W", addr + 2, "-> Start stretch, curvature")  # to add to jobs
                add("W", addr + 4, "-> Start stretch, height")  # to add to jobs
                add("W", addr + 6, "-> Start stretch, lanes")  # to add to jobs
                add("W", addr + 8, "-> Start stretch, right-side objects")  # to add to jobs
                add("W", addr + 10, "-> Start stretch, left-side objects")  # to add to jobs
                add("W", addr + 12, "-> Start stretch, hazards")  # to add to jobs

                addjob("map_curvature", wordat(addr +  2) + 1)
                addjob("map_height",    wordat(addr +  4) + 1)
                addjob("map_lanes",     wordat(addr +  6) + 1)
                addjob("map_rightobjs", wordat(addr +  8) + 1)
                addjob("map_leftobjs",  wordat(addr + 10) + 1)
                addjob("map_hazards",   wordat(addr + 12) + 1)

            case "attract_data":
                add("w", addr, "Per-stage attract mode data")
                add("W", addr + 0, "road_pos")
                add("W", addr + 2, "-> Loop section, curvature")  # to add to jobs
                add("W", addr + 4, "-> Loop section, height")  # to add to jobs
                add("W", addr + 6, "-> Loop section, lanes")  # to add to jobs
                add("W", addr + 8, "-> Loop section, right-side objects")  # to add to jobs
                add("W", addr + 10, "-> Loop section, left-side objects")  # to add to jobs
                add("W", addr + 12, "-> Loop section, hazards")  # to add to jobs

                addjob("map_curvature", wordat(addr +  2) + 1)
                addjob("map_height",    wordat(addr +  4) + 1)
                addjob("map_lanes",     wordat(addr +  6) + 1)
                addjob("map_rightobjs", wordat(addr +  8) + 1)
                addjob("map_leftobjs",  wordat(addr + 10) + 1)
                addjob("map_hazards",   wordat(addr + 12) + 1)

            case "hittable_objects":
                add("N", addr + 0, "Hittable hazards")

            case "nancy_perp_desc":
                who = byteat(addr)
                add("b", addr, "character id, e.g. nancy")
                addr += 1
                while 1:
                    desc = wordat(addr)
                    if desc & 0xFF == 0xFE:  # unclear
                        add("b", addr, "terminator?")
                        break
                    add("w", addr, "perp desc ptr")
                    addjob("string", wordat(addr))
                    addr += 2
                # TODO final pointer always random choice?

            case "string": # top bit terminated
                decodedstring = topbitstring(addr)
                add("T", addr, "\"%s\"" % decodedstring)

            case "arrest_messages":
                add("b", addr, "frame delay until first message?")
                addr += 1
                for _ in range(1,4):
                    add("B", addr + 0, "frame delay until next message?")
                    add("B", addr + 1, "flags")
                    add("B", addr + 2, "attribute")
                    add("W", addr + 3, "back buffer addr")
                    add("W", addr + 5, "attribute addr")
                    addr += 7
                    addjob("string", addr)
                    addr += topbitstrlen(addr)
                add("B", addr, "frame delay until next message?")
                addr += 1

            case "map_curvature":
                add("b", addr, "Map curvature data")
                curvemap = {0: "Curve Straight",
                            1: "Curve Right",
                            2: "Curve Right Hard",
                            3: "Curve Right Very Hard",
                            4: "Curve 4 XXX",
                            5: "Curve 5 XXX",
                            6: "Curve 6 XXX",
                            7: "Curve 7 XXX",
                            8: "Curve Straight (alt)",
                            9: "Curve Left",
                            10: "Curve Left Hard",
                            11: "Curve Left Very Hard",
                            12: "Curve 12 XXX",
                            13: "Curve 13 XXX",
                            14: "Curve 14 XXX",
                            15: "Curve 15 XXX"}
                decode_nibble_rle(addr, "curvature", curvemap)

            case "map_height":
                add("b", addr, "Map height data")
                heightmap = {0: "Going Up 8 XXX",
                             1: "Going Up 7",
                             2: "Going Up 6 XXX",
                             3: "Going Up 5",
                             4: "Going Up 4 XXX",
                             5: "Going Up 3",
                             6: "Going Up 2 XXX",
                             7: "Going Up 1",  # up or down?
                             8: "Level Road",
                             9: "Going Down 1",
                             10: "Going Down 2 XXX",
                             11: "Going Down 3",
                             12: "Going Down 4 XXX",
                             13: "Going Down 5",
                             14: "Going Down 6 XXX",
                             15: "Going Down 7"}
                decode_nibble_rle(addr, "height", heightmap)

            case "map_rightobjs":
                add("b", addr, "Map right object data")
                omap = {0: "Nothing",
                        1: "Tunnel Light",
                        2: "Unknown (2)",
                        3: "Short Pole",
                        4: "Tree",
                        5: "Bush",
                        6: "Street Lamp",
                        7: "Telegraph Pole",
                        8: "Turn Sign, Pointing Left",
                        9: "Turn Sign, Pointing Right",
                        10: "Unknown (10)",
                        11: "Unknown (11)",
                        12: "Unknown (12)",
                        13: "Unknown (13)",
                        14: "Unknown (14)",
                        15: "Unknown (15)"}
                decode_nibble_rle(addr, "right objects", omap)

            case "map_leftobjs":
                add("b", addr, "Map left object data")
                omap = {0: "Nothing",
                        1: "Tunnel Light",
                        2: "Unknown (2)",
                        3: "Short Pole",
                        4: "Tree",
                        5: "Bush",
                        6: "Street Lamp",
                        7: "Telegraph Pole",
                        8: "Turn Sign, Pointing Left",
                        9: "Turn Sign, Pointing Right",
                        10: "Unknown (10)",
                        11: "Unknown (11)",
                        12: "Unknown (12)",
                        13: "Unknown (13)",
                        14: "Unknown (14)",
                        15: "Unknown (15)"}
                decode_nibble_rle(addr, "left objects", omap)

            case "map_lanes":
                add("b", addr, "Map lanes data")
                # L/M/R is the left/middle/right alignment of the road with respect to
                # the default four-lane road.
                #
                # If not noted then the respective byte is used by Stage 1. If "Poke"
                # then it was discovered by altering the game.
                #
                #                                  [  * ] is the car's default position
                lanesmap = {0x00: r'4 Lanes              [||||] {00}',
                            0x01: r'2 Lanes L            [||]   {01}', # Poke
                            0x02: r'2 Lanes M             [||]  {02}', # Poke
                            0x03: r'2 Lanes R              [||] {03}', # Stage 5
                            0x06: r'3-2 Narrowing L      [/||]  {06}', # Poke
                            0x0D: r'3-2 Narrowing X     [/||]   {0D}', # Poke - Invalid: left side flickers
                            0x0F: r'3-2 Narrowing R       [/||] {0F}',
                            0x1F: r'2-3 Widening R        [\||] {1F}',
                            0x2D: r'2-3 Widening L       [\||]  {2D}', # used in fork exit
                            0x45: r'Tunnel start                {45}', # tunnels always two lanes?
                            0x59: r'Tunnel cont/end?            {59}', # TBD
                            0x81: r'3 Lanes L            [|||]  {81}',
                            0x82: r'3 Lanes R             [|||] {82}',
                            0x8E: r'4-3 Narrowing R      [/|||] {8E}',
                            0x9E: r'3-4 Widening R       [\|||] {9E}',
                            0xAD: r'3-4 Widening L       [|||/] {AD}',
                            0xBD: r'4-3 Narrowing L      [|||\] {BD}',
                            0xC1: r'4 Lanes dirt track   [||||] {C1}',
                            0xC2: r'3 Lanes dirt track R  [|||] {C2}', # Poke
                            0xC3: r'2 Lanes dirt track R   [||] {C3}'} # Poke (stones on verge)
                decode_count_rle(addr, "lanes", lanesmap)

            case "map_hazards":
                add("b", addr, "Map hazards data")
                hmap = {0x00: "xxx"}
                decode_hazards(addr, "hazards", hmap)

            case "lod":
                for _ in range(0,6):
                    add("N", addr + 0, "LOD")
                    add("B", addr + 0, "Width (bytes)")
                    add("B", addr + 1, "Flags")
                    add("B", addr + 2, "Height (pixels)")
                    add("W", addr + 3, "Bitmap address")
                    add("W", addr + 5, "Pre-shifted bitmap address")

                    widthbytes    = byteat(addr + 0)
                    height        = byteat(addr + 2)
                    bitmap        = wordat(addr + 3)
                    shiftedbitmap = wordat(addr + 5)
                    nbytes        = widthbytes * height

                    add("B", bitmap, "Bitmap data %d bytes x %d" % (widthbytes, height), nbytes, widthbytes)
                    if bitmap != shiftedbitmap:
                        add("B", shiftedbitmap, "Pre-shifted bitmap data %d bytes x %d" % (widthbytes, height), nbytes, widthbytes)

                    addr += 7

            case _:
                print("Unhandled: " + job, file=sys.stderr)

    # sort locations
    locations.sort(key=lambda entry: entry[1])

    # at end dump all the locations out (correcting the addresses)
    for loc in locations:
        blocktype, addr, desc, nbytes, widthbytes = loc
        nbytesstr = ""
        if nbytes >= 0:
            nbytesstr = ",%d" % (nbytes)

        widthstr = ""
        if widthbytes < 0:
            if blocktype == "B":
                widthstr = ",1"
            elif blocktype == "W":
                widthstr = ",2"
        else:
            widthstr = ",%d" % widthbytes
        print("%s $%4X%s%s %s" % (blocktype, addr + ORIGBASE - BASE, nbytesstr, widthstr, desc))

if __name__ == '__main__':
    main(sys.argv[1:])
