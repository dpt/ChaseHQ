# Makefile for the Sinclair User Megatape 23 Chase H.Q. demo disassembly
#

NAME="Chase H.Q. demo (Megatape 23)"
GAME=demo
BUILD?=build
OPTIONS=--hex

CTL=$(GAME).ctl
ASM=$(BUILD)/$(GAME).asm
PRISTINEZ80=$(BUILD)/$(GAME).pristine.z80
SKOOL=$(GAME).skool
REF=$(GAME).ref

GENERATED_CTL=$(BUILD)/$(CTL)

.PHONY: usage
usage:
	@echo "Supported targets:"
	@echo "  usage		Show this help"
	@echo "  all		Build virtually everything"
	@echo "  pristine	Convert the tape image into a pristine Z80 snapshot"
	@echo "  skool		Build a skool file from the control file and the snapshot"
	@echo "  disasm	Build the $(NAME) disassembly"
	@echo "  asm		Build assembly sources from the skool file"
	@echo "  ctl		Build a new control file from the skool file"
	@echo "  commit		Rebuild the main control file from the skool file"
	@echo "  clean		Clean a previous build"
	@echo ""
	@echo "Environment variables:"
	@echo "  BUILD          directory in which to build the disassembly (default: build)"

.PHONY: all
all: pristine skool disasm asm

.PHONY: pristine
pristine: $(PRISTINEZ80)

$(PRISTINEZ80):
	tap2sna.py --output-dir $(BUILD) @$(GAME).t2s && mv $(BUILD)/$(GAME).z80 $(PRISTINEZ80)

.PHONY: skool
skool: $(SKOOL)

$(SKOOL): $(PRISTINEZ80) $(CTL)
	mkdir -p $(BUILD)
	sna2skool.py $(OPTIONS) --ctl $(CTL) $(PRISTINEZ80) > $@

.PHONY: disasm
disasm: $(SKOOL)
	skool2html.py $(OPTIONS) --asm-labels --rebuild-images --output-dir $(BUILD) $(SKOOL) $(REF)

.PHONY: asm
asm: $(ASM)

$(ASM): $(SKOOL)
	mkdir -p $(BUILD)
	skool2asm.py $(OPTIONS) --create-labels --no-warnings --ssub $< > $@

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
