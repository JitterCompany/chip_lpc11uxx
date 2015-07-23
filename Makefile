
TOOLCHAIN_PREFIX=arm-none-eabi-
CC=$(TOOLCHAIN_PREFIX)gcc
AR=$(TOOLCHAIN_PREFIX)ar
LD=$(TOOLCHAIN_PREFIX)ld
SZ=$(TOOLCHAIN_PREFIX)size
RM := rm -rf

LIBRARY_NAME = lpc_chip_11uxx
SRC_DIR = ./src/
BUILD_DIR = ./build/
ILIBS = -I./inc

CORE = m0
GLOBAL_DEFS = -D__LPC11U1X__ -DCORE_M0 
CFLAGS = -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -std=gnu99 -mcpu=cortex-$(CORE) -mthumb -MMD -MP

OBJS := \
adc_1125.o \
adc_11xx.o \
chip_11xx.o \
clock_11xx.o \
gpio_11xx_1.o \
gpio_11xx_2.o \
gpiogroup_11xx.o \
i2c_11xx.o \
iocon_11xx.o \
pinint_11xx.o \
pmu_11xx.o \
ring_buffer.o \
ssp_11xx.o \
sysctl_11xx.o \
sysinit_11xx.o \
timer_11xx.o \
uart_11xx.o \
wwdt_11xx.o


OBJS_F = $(addprefix $(BUILD_DIR), $(OBJS))
LIBRARY_FILE = "./lib$(LIBRARY_NAME).a"


all: post-build


.SECONDEXPANSION:
$(LIBRARY_FILE) : $$(OBJS_F)
	@mkdir -p "$(BUILD_DIR)"
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Archiver'
	 
	$(AR) -r  $@ $(OBJS_F) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	#$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(BUILD_DIR)
	-$(RM) $(LIBRARY_FILE)
	-@echo ' '


post-build: $(LIBRARY_FILE)
	-@echo 'Performing post-build steps'
	$(SZ) $(LIBRARY_FILE)
	-@echo ' '
	-@echo ' '
	-@echo ' '

$(BUILD_DIR)%.o: $(SRC_DIR)%.c
	mkdir -p '$(dir $@)'
	@echo 'Building file: $@ in $(BUILD_DIR) from $<'
	@echo 'Invoking: MCU C Compiler'
	@echo flags=$(CFLAGS)
	$(CC) $(GLOBAL_DEFS) $(ILIBS) $(CFLAGS) -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

.PHONY: all post-build clean dependents
.SECONDARY:

