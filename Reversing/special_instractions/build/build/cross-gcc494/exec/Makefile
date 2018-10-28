#BASEDIR = /usr/local
#BASEDIR = $(HOME)
BASEDIR = $(HOME)/20181027-moxie
SUBDIR = cross-gcc494

PREFIX = $(BASEDIR)/$(SUBDIR)
BINDIR = $(PREFIX)/bin

# native architectures.
#TARGETS_NATIVE += i386-elf.d

# major architectures.
#TARGETS_SIM += arm-elf.d h8300-elf.d mips-elf.d powerpc-elf.d sh-elf.d

# available on new gcc.
#TARGETS_SIM += aarch64-elf.d bfin-elf.d cr16-elf.d m32c-elf.d microblaze-elf.d
#TARGETS_SIM += moxie-elf.d msp430-elf.d rl78-elf.d rx-elf.d
TARGETS_SIM += moxie-elf.d

# other architectures.
#TARGETS_SIM += avr-elf.d cris-elf.d frv-elf.d m32r-elf.d mcore-elf.d
#TARGETS_SIM += mn10300-elf.d sh64-elf.d sparc-elf.d v850-elf.d

# use compiler for main architecture.
#TARGETS_SIM += arm16-elf.d mips16-elf.d

TARGETS = $(TARGETS_NATIVE) $(TARGETS_SIM)

SRCS = $(TARGETS:.d=.c)
ASMS = $(TARGETS:.d=.s)
OBJS = $(TARGETS:.d=.o)
EXES = $(TARGETS:.d=.x)

NOUT = $(TARGETS_NATIVE:.d=.not)
SOUT = $(TARGETS_SIM:.d=.sot)

SOURCE = sample.c

UNAME = `uname -s | sed 's/[\_\-].*//'`

# common options.
GFLAGS   = -fno-builtin -nostdinc -nostdlib -static -O -Wall
#GFLAGS  += -g
GFLAGS  += -D___$(UNAME)___
CFLAGS   = -fverbose-asm
CFLAGS  += -fomit-frame-pointer
#CFLAGS += -fno-omit-frame-pointer
CFLAGS  += -fno-inline
CFLAGS  += -DARCH=\"$*\"
AFLAGS   = 
LFLAGS   = -Wl,-Tld.scr
LFLAGS  += lib-$*.S
DFLAGS   = -d
#DFLAGS += -S
XFLAGS   =

FGCC-arm16-elf  = arm16-elf
TGCC-arm16-elf  = arm-elf
FGCC-avr8-elf   = avr8-elf
TGCC-avr8-elf   = avr-elf
FGCC-h8300h-elf = h8300h-elf
TGCC-h8300h-elf = h8300-elf
FGCC-m6811s-elf = m6811s-elf
TGCC-m6811s-elf = m6811-elf
FGCC-mips16-elf = mips16-elf
TGCC-mips16-elf = mips-elf

FGDB-arm16-elf  = arm16-elf
TGDB-arm16-elf  = arm-elf

# general options.
GFLAGS-arm16-elf       += -mthumb -mthumb-interwork
GFLAGS-avr8-elf        += -mint8 -w
GFLAGS-h8300h-elf      += -mh -mint32
GFLAGS-m32c-elf        += -memregs=0
GFLAGS-m6811s-elf      += -mshort
#GFLAGS-m68k-elf       += -mshort
GFLAGS-mips16-elf      += -mips16
GFLAGS-mips64-elf      += -mips64 -march=mips64 -mtune=mips64 -mlong64
GFLAGS-mmix-elf        += -melf
GFLAGS-msp430-elf      += -mlarge
#GFLAGS-pdp11-aout     += -mint32
GFLAGS-powerpc64-linux += -mpowerpc64 -mcpu=powerpc64 -mtune=powerpc64
GFLAGS-sh64-elf        += -m5-64media
GFLAGS-x86_64-linux    += -m64

# options without symbols defined on linker script.
#GFLAGS-avr-elf    += -minit-stack=0xfe1c00
#GFLAGS-avr8-elf   += -minit-stack=0xfe1c00
#GFLAGS-frv-elf    += -G 0
#GFLAGS-hppa-linux += -mfast-indirect-calls
#GFLAGS-m6811-elf  += -msoft-reg-count=0
#GFLAGS-m6811s-elf += -msoft-reg-count=0
#GFLAGS-mips-elf   += -G 0
#GFLAGS-mips16-elf += -G 0
#GFLAGS-mips64-elf += -G 0
#GFLAGS-v850-elf   += -mno-prolog-function
#GFLAGS-xtensa-elf += -mtext-section-literals

# to avoid debug option.
NOGFLAGS-h8300-elf  += -g
NOGFLAGS-h8300h-elf += -g
NOGFLAGS-i960-elf   += -g
NOGFLAGS-m6811-elf  += -g
NOGFLAGS-m6811s-elf += -g
NOGFLAGS-microblaze-elf += -g
NOGFLAGS-pdp11-aout += -g

# compile options.
NOCFLAGS-i386-elf += -fverbose-asm
CFLAGS-i960-elf   += -DNO_DIRECT
CFLAGS-ia64-elf   += -DNO_DIRECT -DNO_BINARY
CFLAGS-mmix-elf   += -DNO_DIRECT

# to use system call wrapper functions.
CFLAGS-m32r-elf    += -DUSE_SYSCALL_WRAPPER
CFLAGS-mn10300-elf += -DUSE_SYSCALL_WRAPPER
CFLAGS-sh-elf      += -DUSE_SYSCALL_WRAPPER
CFLAGS-sh64-elf    += -DUSE_SYSCALL_WRAPPER
CFLAGS-v850-elf    += -DUSE_SYSCALL_WRAPPER

# assemble options.
AFLAGS-arc-elf  += -Wa,-marc5
AFLAGS-ia64-elf += -mgnu-as

# link options.
LFLAGS-ia64-elf      += -mgnu-ld
#LFLAGS-ia64-elf     += -Wl,-r
#NOLFLAGS-ia64-elf   += -Wl,-Tld.scr
LFLAGS-rl78-elf      += -Wl,-Ttext -Wl,0x1400
LFLAGS-rl78-elf      += -Wl,--no-gc-sections
LFLAGS-rx-elf        += -N
LFLAGS-spu-elf       += -Wl,-Ttext -Wl,0x1400
LFLAGS-x86_64-linux  += -Wl,-n
LFLAGS-xstormy16-elf += -Wl,-Ttext -Wl,0x1400

# disassemble options.
#DFLAGS-arm-elf    += -M reg-names-raw
#DFLAGS-arm16-elf  += -M reg-names-raw
#DFLAGS-mips-elf   += -M reg-names=numeric
#DFLAGS-mips16-elf += -M reg-names=numeric
#DFLAGS-mips64-elf += -M reg-names=numeric
#DFLAGS-mips-elf   += -M no-aliases
#DFLAGS-mips64-elf += -M no-aliases

# execute options.
XFLAGS-bfin-elf    += --environment user
XFLAGS-powerpc-elf += -e linux
#XFLAGS-rl78-elf   += -r 65536

# need to build runnable objects.
#CFLAGS-avr-elf   += -minit-stack=_estack
CFLAGS-frv-elf    += -mno-cond-exec
CFLAGS-m6811-elf  += -DRSHIFT=___lshrsi3
CFLAGS-msp430-elf += -DRSHIFT=__mspabi_srll_4
CFLAGS-rl78-elf   += -DRSHIFT=__lshrsi3

# need to build runnable objects. (set text address)
LFLAGS-avr-elf    += -Wl,--section-start -Wl,.rodata=0x00801800
LFLAGS-i386-elf   += -Wl,-Ttext -Wl,0x08048400
LFLAGS-mips-elf   += -Wl,-Ttext -Wl,0x80000000
LFLAGS-mips16-elf += -Wl,-Ttext -Wl,0x7fff8000
LFLAGS-sparc-elf  += -Wl,-Ttext -Wl,0x02001400

.SUFFIXES:
.SUFFIXES: .not .sot .d .x .o .s .c
.PRECIOUS: $(SRCS) $(ASMS) $(OBJS) $(EXES)

all :		$(TARGETS)

$(SRCS) :	$(SOURCE)
		@echo ---------------- $* ----------------
		@echo ---- create source file \($(SOURCE) =\> $@\)
		cp $(SOURCE) $@

$(ASMS) :	Makefile syscall.h

$(EXES) :	ld.scr lib-*.S

.c.s :
		@echo ---- compile \($< =\> $@\)
		$(BINDIR)/$(*:$(FGCC-$*)=$(TGCC-$*))-gcc \
			$(GFLAGS:$(NOGFLAGS-$*)=) $(GFLAGS-$*) \
			$(CFLAGS:$(NOCFLAGS-$*)=) $(CFLAGS-$*) -o $@ -S $<

.s.o :
		@echo ---- assemble \($< =\> $@\)
		$(BINDIR)/$(*:$(FGCC-$*)=$(TGCC-$*))-gcc \
			$(GFLAGS:$(NOGFLAGS-$*)=) $(GFLAGS-$*) \
			$(AFLAGS:$(NOAFLAGS-$*)=) $(AFLAGS-$*) -o $@ -c $<

.o.x :
		@echo ---- link \($< =\> $@\)
		$(BINDIR)/$(*:$(FGCC-$*)=$(TGCC-$*))-gcc \
			$(GFLAGS:$(NOGFLAGS-$*)=) $(GFLAGS-$*) \
			$(LFLAGS:$(NOLFLAGS-$*)=) $(LFLAGS-$*) -o $@ $<

.x.d :
		@echo ---- disassemble \($< =\> $@\)
		$(BINDIR)/$(*:$(FGCC-$*)=$(TGCC-$*))-objdump \
			$(DFLAGS:$(NODFLAGS-$*)=) $(DFLAGS-$*) $< > $@

.x.sot :
		@echo ---- simulate \($< =\> $@\)
		$(BINDIR)/$(*:$(FGDB-$*)=$(TGDB-$*))-run \
			$(XFLAGS:$(NOXFLAGS-$*)=) $(XFLAGS-$*) $< > $@
		@cat $@

m6811-elf.sot :	m6811-elf.x
		@echo ---- simulate \($< =\> $@\)
		$(BINDIR)/$(*:$(FGDB-$*)=$(TGDB-$*))-run \
			$(XFLAGS:$(NOXFLAGS-$*)=) $(XFLAGS-$*) $< > $@ 0>&1
		@cat $@

microblaze-elf.sot :	microblaze-elf.x
		@echo ---- simulate \($< =\> $@\)
		$(BINDIR)/$(*:$(FGDB-$*)=$(TGDB-$*))-gdb \
			$(XFLAGS:$(NOXFLAGS-$*)=) $(XFLAGS-$*) $< \
			-x cmd-$*.txt \
			| grep Hello | perl -pe 's/.*\"(.*)\\n\".*/$$1/' > $@
		@cat $@

sparc-elf.sot :	sparc-elf.x
		@echo ---- simulate \($< =\> $@\)
		$(BINDIR)/$(*:$(FGDB-$*)=$(TGDB-$*))-run \
			$(XFLAGS:$(NOXFLAGS-$*)=) $(XFLAGS-$*) $< \
			-c cmd-$*.txt \
			| grep Hello > $@
		@cat $@

.x.not :
		@echo ---- execute \($< =\> $@\)
		$(BINDIR)/$(*:$(FGCC-$*)=$(TGCC-$*))-elfedit \
			--output-osabi $(UNAME) $<
		./$< > $@
		@cat $@

# clear default .c.o rule
# (useless because of priority definition of .SUFFIXES)

# for BSD make
.c.o :

# for GNU make
%.o :		%.c

run-native :	$(NOUT)

run-sim :	$(SOUT)

run :		run-native run-sim

clean :
		rm -f $(SRCS) $(ASMS) $(OBJS) $(EXES) $(TARGETS) $(NOUT) $(SOUT)
