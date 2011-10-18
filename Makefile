OUT      ?= test
OBJ       = $(OUT).o asmlib.o

OPTIMIZE  = -O2
WARNINGS  =
LIBS      = 
SIM_OBJ   = $(patsubst %.o,%_sim.o,$(OBJ))
SIM_DEV   = atmega128
SIM_WIO   = 0x30
SIM_RIO   = 0x0E
SIM_DEFS  = -DSIMULAVR -DSIM_WIO=$(SIM_WIO)
SIM_FLAGS   = -MMD -g -mmcu=$(SIM_DEV) $(OPTIMIZE) $(WARNINGS) $(SIM_DEFS)
SIM_ASFLAGS = 
SIM_LDFLAGS = -Wl,-Map,$(OUT)_sim.map

PC_OBJ    = 

MCU_CC    = avr-gcc
OBJDUMP   = avr-objdump

simul: $(OUT)_sim.elf $(OUT)_sim.lst 
	simulavr -d $(SIM_DEV) -W $(SIM_WIO),- -f $(OUT)_sim.elf -T exit

%_sim.o: %.c
	$(MCU_CC) $(SIM_FLAGS) -o $@ -c $<

%_sim.o: %.S
	$(MCU_CC) -mmcu=$(SIM_DEV) $(SIM_ASFLAGS) -o $@ -c $<

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

all: simul
	echo make simul   for avr simulation

$(SIM_OBJ):Makefile

$(OUT)_sim.elf: $(SIM_OBJ)
	$(MCU_CC) $(SIM_FLAGS) $(SIM_LDFLAGS) -o $@ $^ $(LIBS)
	
	
DEPS := $(wildcard *.d)
ifneq ($(DEPS),)
include $(DEPS)
endif	