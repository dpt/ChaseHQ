# Makefile for Chase H.Q. disassembly
#

NAME="Chase H.Q."
GAME=ChaseHQ-128K
BUILD?=build
OPTIONS=--hex

CTL=$(GAME).ctl
CTLBANK1=$(GAME)-bank-1.ctl
CTLBANK3=$(GAME)-bank-3.ctl
CTLBANK4=$(GAME)-bank-4.ctl
CTLBANK6=$(GAME)-bank-6.ctl
CTLBANK7=$(GAME)-bank-7.ctl

ASM=$(BUILD)/$(GAME).asm
BIN=$(BUILD)/$(GAME).bin
PRISTINEZ80=$(BUILD)/$(GAME).pristine.z80

SKOOL=$(GAME).skool
SKOOLBANK1=$(GAME)-bank-1.skool
SKOOLBANK3=$(GAME)-bank-3.skool
SKOOLBANK4=$(GAME)-bank-4.skool
SKOOLBANK6=$(GAME)-bank-6.skool
SKOOLBANK7=$(GAME)-bank-7.skool

SKOOLBANKS=$(SKOOLBANK1) $(SKOOLBANK3) $(SKOOLBANK4) $(SKOOLBANK6) $(SKOOLBANK7)

REF=$(GAME).ref ChaseHQChangelog.ref
TAP=$(BUILD)/$(GAME).tap
Z80=$(BUILD)/$(GAME).z80

# The main skool file's @bank directives pull in the bank skool files, so
# skool2bin --banks emits a 128K image (banks 0-7, 16K each) rather than a 48K
# one. Everything below is about turning that into something bootable.
#
# ENTRY is entry_128k ($E816), which the game's own tape loader jumps to once
# all the banks are in place. It expects interrupts disabled, so the snapshot
# sets iff=0.
#
# A tape can't do that: bin2tap's RAM bank loader finishes with EI before it
# jumps. So we poke a "DI; JP entry_128k" stub into the stage 1 spare space at
# $7636 (dead once the banks are loaded) and start the tape there instead.
#
# The tape omits $4000-$5FFF (BEGIN): that's the loading screen (re-added with
# -S), the game's tape loader and the head of the stage 1 payload, all of which
# are dead by the time the game runs, and BASIC needs somewhere to live while
# the bank loader is running. CLEAR leaves $5FB4-$5FFF for that loader.
ENTRY=59414
STUB=30262
STUBPOKES=-P 30262,243 -P 30263,195 -P 30264,22 -P 30265,232
CLEAR=24500
BEGIN=24576

GENERATED_CTL=$(BUILD)/$(CTL)
GENERATED_CTLBANK1=$(BUILD)/$(CTLBANK1)
GENERATED_CTLBANK3=$(BUILD)/$(CTLBANK3)
GENERATED_CTLBANK4=$(BUILD)/$(CTLBANK4)
GENERATED_CTLBANK6=$(BUILD)/$(CTLBANK6)
GENERATED_CTLBANK7=$(BUILD)/$(CTLBANK7)

.PHONY: usage
usage:
	@echo "Supported targets:"
	@echo "  usage		Show this help"
	@echo "  all		Build virtually everything"
	@echo "  pristine	Fetch a tape image of $(NAME) and convert it into a pristine Z80 snapshot"
	@echo "  skool		Build a skool file from the control file and the snapshot"
	@echo "  disasm	Build the $(NAME) disassembly"
	@echo "  asm		Build assembly sources from the skool file"
	@echo "  z80		Build a Z80 snapshot from the skool file"
	@echo "  tap		Build a TAP file from the skool file"
	@echo "  ctl		Build a new control file from the skool file"
	@echo "  commit		Rebuild the main control file from the skool file"
	@echo "  clean		Clean a previous build"
	@echo ""
	@echo "Environment variables:"
	@echo "  BUILD          directory in which to build the disassemblies (default: build)"

# .PHONY rules are always run.

.PHONY: all
all: pristine skool disasm asm z80 tap

.PHONY: pristine
pristine: $(PRISTINEZ80)

$(PRISTINEZ80):
	tap2sna.py --output-dir $(BUILD) @$(GAME).t2s && mv $(BUILD)/$(GAME).z80 $(PRISTINEZ80)

.PHONY: skool
skool: $(SKOOL)

$(SKOOL): $(PRISTINEZ80) $(CTL) $(CTLBANK1) $(CTLBANK3) $(CTLBANK4) $(CTLBANK6) $(CTLBANK7)
	mkdir -p $(BUILD)
	sna2skool.py $(OPTIONS) --ctl $(CTL) --page 0 $(PRISTINEZ80) > $@
	sna2skool.py $(OPTIONS) --ctl $(CTLBANK1) --page 1 --start 49152 --end 65536 $(PRISTINEZ80) > $(SKOOLBANK1)
	sna2skool.py $(OPTIONS) --ctl $(CTLBANK3) --page 3 --start 49152 --end 65536 $(PRISTINEZ80) > $(SKOOLBANK3)
	sna2skool.py $(OPTIONS) --ctl $(CTLBANK4) --page 4 --start 49152 --end 65536 $(PRISTINEZ80) > $(SKOOLBANK4)
	sna2skool.py $(OPTIONS) --ctl $(CTLBANK6) --page 6 --start 49152 --end 65536 $(PRISTINEZ80) > $(SKOOLBANK6)
	sna2skool.py $(OPTIONS) --ctl $(CTLBANK7) --page 7 --start 49152 --end 65536 $(PRISTINEZ80) > $(SKOOLBANK7)

.PHONY: disasm
disasm: $(SKOOL) $(SKOOLBANK1) $(SKOOLBANK3) $(SKOOLBANK4) $(SKOOLBANK6) $(SKOOLBANK7)
	skool2html.py $(OPTIONS) --asm-labels --rebuild-images --output-dir $(BUILD) $(SKOOL) $(REF)

.PHONY: asm
asm: $(ASM)

$(ASM): $(SKOOL)
	mkdir -p $(BUILD)
	skool2asm.py $(OPTIONS) --create-labels --no-warnings --ssub $< > $@

.PHONY: z80
z80: $(Z80)

$(Z80): $(SKOOL) $(SKOOLBANKS)
	skool2bin.py --banks $< - | bin2sna.py --page 0 --stack 23550 --start $(ENTRY) --state iff=0 $(STUBPOKES) - $@

.PHONY: tap
tap: $(TAP)

$(TAP): $(Z80) $(PRISTINEZ80)
	bin2tap.py --7ffd 0 --clear $(CLEAR) --begin $(BEGIN) --start $(STUB) -S $(PRISTINEZ80) $< $@

.PHONY: ctl
ctl: $(GENERATED_CTL)

$(GENERATED_CTL): $(SKOOL) $(SKOOLBANK1) $(SKOOLBANK3) $(SKOOLBANK4) $(SKOOLBANK6) $(SKOOLBANK7)
	mkdir -p $(BUILD)
	skool2ctl.py $(OPTIONS) $(SKOOL) > $@
	skool2ctl.py $(OPTIONS) $(SKOOLBANK1) > $(GENERATED_CTLBANK1)
	skool2ctl.py $(OPTIONS) $(SKOOLBANK3) > $(GENERATED_CTLBANK3)
	skool2ctl.py $(OPTIONS) $(SKOOLBANK4) > $(GENERATED_CTLBANK4)
	skool2ctl.py $(OPTIONS) $(SKOOLBANK6) > $(GENERATED_CTLBANK6)
	skool2ctl.py $(OPTIONS) $(SKOOLBANK7) > $(GENERATED_CTLBANK7)

# Brings changes from .skool back to .ctl
# Use when ready for commit
.PHONY: commit
commit: ctl
	cp $(GENERATED_CTL) $(GENERATED_CTLBANK1) $(GENERATED_CTLBANK3) $(GENERATED_CTLBANK4) $(GENERATED_CTLBANK6) $(GENERATED_CTLBANK7) $(BUILD)/../

.PHONY: clean
clean:
	@echo 'Cleaning...'
	-rm -rf $(BUILD)
