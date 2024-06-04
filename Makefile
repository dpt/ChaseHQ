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

REF=$(GAME).ref
TAP=$(BUILD)/$(GAME).tap
Z80=$(BUILD)/$(GAME).z80
GENERATED_CTL=$(BUILD)/$(GAME).ctl

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
skool: $(SKOOL) $(SKOOLBANK1) $(SKOOLBANK3) $(SKOOLBANK4) $(SKOOLBANK6) $(SKOOLBANK7)

$(SKOOL): $(PRISTINEZ80) $(CTL) $(CTLBANK1) $(CTLBANK3) $(CTLBANK4) $(CTLBANK6) $(CTLBANK7)
	mkdir -p $(BUILD)
	sna2skool.py $(OPTIONS) --ctl $(CTL) --page 0 $(PRISTINEZ80) > $@
	sna2skool.py $(OPTIONS) --ctl $(CTLBANK1) --page 1 --start 49152 --end 65536 $(PRISTINEZ80) > $(SKOOLBANK1)
	sna2skool.py $(OPTIONS) --ctl $(CTLBANK3) --page 3 --start 49152 --end 65536 $(PRISTINEZ80) > $(SKOOLBANK3)
	sna2skool.py $(OPTIONS) --ctl $(CTLBANK4) --page 4 --start 49152 --end 65536 $(PRISTINEZ80) > $(SKOOLBANK4)
	sna2skool.py $(OPTIONS) --ctl $(CTLBANK6) --page 6 --start 49152 --end 65536 $(PRISTINEZ80) > $(SKOOLBANK6)
	sna2skool.py $(OPTIONS) --ctl $(CTLBANK7) --page 7 --start 49152 --end 65536 $(PRISTINEZ80) > $(SKOOLBANK7)

.PHONY: disasm
disasm: $(SKOOL)
	skool2html.py $(OPTIONS) --asm-labels --rebuild-images --output-dir $(BUILD) $(SKOOL) $(REF)

.PHONY: asm
asm: $(ASM)

$(ASM): $(SKOOL)
	mkdir -p $(BUILD)
	skool2asm.py $(OPTIONS) --create-labels --no-warnings --ssub $< > $@

.PHONY: z80
z80: $(Z80)

$(Z80): $(SKOOL)
	skool2bin.py $< - | bin2sna.py --org 16384 --stack 23550 --start 23372 - $@

.PHONY: tap
tap: $(TAP)

$(TAP): $(SKOOL)
	skool2bin.py $< - | bin2tap.py --org 16384 --stack 23550 --start 23372 - $@

.PHONY: ctl
ctl: $(GENERATED_CTL)

$(GENERATED_CTL): $(SKOOL)
	mkdir -p $(BUILD)
	skool2ctl.py $(OPTIONS) $(SKOOL) > $@

# Brings changes from .skool back to .ctl
# Use when ready for commit
.PHONY: commit
commit: ctl
	cp $(GENERATED_CTL) $(BUILD)/../

.PHONY: clean
clean:
	@echo 'Cleaning...'
	-rm -rf $(BUILD)
