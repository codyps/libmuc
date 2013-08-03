all: t-usart-gen.o
.PHONY: all

ALL_CFLAGS += -I.. -I../..
ALL_CFLAGS += -Wl,--as-needed -Os
ALL_CFLAGS += -std=gnu99 -ggdb -g3
ALL_CFLAGS += -Wall -Wstrict-prototypes -Wno-main
ALL_CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
ALL_CFLAGS += -Wa,-adhlns=$(<:.c=.lst)
ALL_CFLAGS += -mmcu=$(MCU) -DF_CPU=$(F_CPU) $(CFLAGS)
ALL_CFLAGS += -flto
ALL_CFLAGS += -fstack-usage

ALL_ASFLAGS = -mmcu=$(MCU) -x assembler-with-cpp $(ASFLAGS)

ifndef V
	QUIET_CC   = @ echo '   CC  ' $@;
	QUIET_LINK = @ echo '   LINK' $@;
	QUIET_HEX  = @ echo '   HEX ' $@;
	QUIET_LSS  = @ echo '   LSS ' $@;
	QUIET_SYM  = @ echo '   SYM ' $@;
	QUIET_EEP  = @ echo '   EEP ' $@;
endif

MCU   = atmega644p
ADMCU = atmega644p
F_CPU = 8000000

#ASFLAGS = -Wa,-adhlns=$(<:.S=.lst),-ggdb

#Additional libraries.

# Minimalistic printf version
#PRINTF_LIB_MIN = -Wl,-u,vfprintf -lprintf_min
# Floating point printf version (requires MATH_LIB = -lm below)
#PRINTF_LIB_FLOAT = -Wl,-u,vfprintf -lprintf_flt
PRINTF_LIB =

# Minimalistic scanf version
#SCANF_LIB_MIN = -Wl,-u,vfscanf -lscanf_min
# Floating point + %[ scanf version (requires MATH_LIB = -lm below)
#SCANF_LIB_FLOAT = -Wl,-u,vfscanf -lscanf_flt
SCANF_LIB =

#MATH_LIB = -lm

LDMAP = $(LDFLAGS) -Wl,-Map=$@.map,--cref
ALL_LDFLAGS  = $(EXTMEMOPTS) $(LDMAP)
ALL_LDFLAGS += $(PRINTF_LIB) $(SCANF_LIB) $(MATH_LIB)
ALL_LDFLAGS += $(LDFLAGS)
ALL_LDFLAGS += $(ALL_CFLAGS)
ALL_LDFLAGS += -fuse-linker-plugin -fwhole-program

AVRDUDE_PROGRAMMER = usbtiny
AVRDUDE_BASIC = -p $(ADMCU) -c $(AVRDUDE_PROGRAMMER)
AVRDUDE_FLAGS = $(AVRDUDE_BASIC)
AVRDUDE_POWER = -t -p $(ADMCU) -c $(AVRDUDE_PROGRAMMER)

CROSS_COMPILE=avr-
CC   = $(CROSS_COMPILE)gcc
AS   = $(CC)
LINK = $(CC)
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
SIZE = $(CROSS_COMPILE)size
NM   = $(CROSS_COMPILE)nm
AVRDUDE = avrdude
RM   = rm -f
MV   = mv -f

.SECONDARY:
.PHONY: FORCE

### Detect prefix changes
## Use "#')" to hack around vim highlighting.
TRACK_CFLAGS = $(CC):$(subst ','\'',$(ALL_CFLAGS)) #')
.TRACK-CFLAGS: FORCE
	@FLAGS='$(TRACK_CFLAGS)'; \
	if test x"$$FLAGS" != x"`cat .TRACK-CFLAGS 2>/dev/null`" ; then \
		echo 1>&2 "    * new build flags or prefix"; \
		echo "$$FLAGS" >.TRACK-CFLAGS; \
	fi

TRACK_LDFLAGS = $(LINK):$(subst ','\'',$(ALL_LDFLAGS)) #')
.TRACK-LDFLAGS: FORCE
	@FLAGS='$(TRACK_LDFLAGS)'; \
	if test x"$$FLAGS" != x"`cat .TRACK-LDFLAGS 2>/dev/null`" ; then \
		echo 1>&2 "    * new link flags"; \
		echo "$$FLAGS" >.TRACK-LDFLAGS; \
	fi

TRACK_ASFLAGS = $(AS):$(subst ','\'',$(ALL_ASFLAGS)) #')
.TRACK-ASFLAGS: FORCE
	@FLAGS='$(TRACK_ASFLAGS)'; \
	if test x"$$FLAGS" != x"`cat .TRACK-ASFLAGS 2>/dev/null`" ; then \
		echo 1>&2 "    * new assembler flags"; \
		echo "$$FLAGS" >.TRACK-ASFLAGS; \
	fi

all: $(TARGETS)

.PHONY: %.flash
%.flash: %.hex %.eep
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U flash:w:$*.hex -U flash:w:$*.eep

ifdef SERIAL
.PHONY: picocom
picocom:
	picocom -b 57600 -p o -c $(SERIAL)
endif

.PHONY: power
power:
	$(AVRDUDE) $(AVRDUDE_POWER)

# Convert ELF to COFF for use in debugging / simulating in AVR Studio or VMLAB.
COFFCONVERT=$(OBJCOPY) --debugging \
--change-section-address .data-0x800000 \
--change-section-address .bss-0x800000 \
--change-section-address .noinit-0x800000 \
--change-section-address .eeprom-0x810000

%.cof: %.elf
	$(COFFCONVERT) -O coff-avr $< $@


%.extcof: %.elf
	$(COFFCONVERT) -O coff-ext-avr $< $@

%.hex: %.elf
	$(QUIET_HEX)$(OBJCOPY) -O ihex -R .eeprom $< $@

%.eep: %.elf
	$(QUIET_EEP)$(OBJCOPY) -j .eeprom \
	--set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 --no-change-warnings \
	-O ihex $< $@

%.elf.lss: %.elf
	$(QUIET_LSS)$(OBJDUMP) -h -S $< > $@

%.sym: %.elf
	$(QUIET_SYM)$(NM) -n $< > $@

%.elf: .TRACK-LDFLAGS .TRACK-CFLAGS
	$(QUIET_LINK)$(LINK) $(ALL_LDFLAGS) -o $@ \
		$(filter-out .TRACK-LDFLAGS,$(filter-out .TRACK-CFLAGS,$^))

$(TARGETS) : .TRACK-LDFLAGS .TRACK-CFLAGS
	$(QUIET_LINK)$(LINK) $(ALL_LDFLAGS) -o $@ \
		$(filter-out .TRACK-LDFLAGS,$(filter-out .TRACK-CFLAGS,$^))

.SECONDARY:
%.o: %.c .TRACK-CFLAGS
	$(QUIET_CC)$(CC) -MMD $(ALL_CFLAGS) -c -o $@ $<

%.s: %.c
	$(CC) -S $(ALL_CFLAGS) -c -o $@ $<

%.o: %.S .TRACK-ASFLAGS
	$(QUIET_AS)$(AS) -c $(ALL_ASFLAGS) $< -o $@

.PHONY: %.clean
%.clean :
	$(RM) $*.hex $*.eep $*.cof $*.elf $*.elf.map $*.sym $*.elf.lss

.PHONY: clean
obj-all = $(foreach target,$(TARGETS),$(obj-$(target)))
clean:	$(foreach target,$(TARGETS),$(target).clean)
	$(RM) $(obj-all) $(obj-all:.o=.lss) $(obj-all:.o=.lst)
	$(RM) $(obj-all:.o=.d)
	$(RM) .TRACK-CFLAGS .TRACK-ASFLAGS .TRACK-LDFLAGS

-include $(obj-all:.o=.d)
