# ================================================================
#  Makefile — STM32U5 bare-metal project
#  工具链: arm-none-eabi-gcc (GNU Arm Embedded Toolchain 10.3+)
#  目标:   STM32U575xx, Cortex-M33, 160 MHz
# ================================================================

TARGET    := stm32u5_skeleton
BUILD_DIR := build

# --- Toolchain ---------------------------------------------------
CC        := arm-none-eabi-gcc
OBJCOPY   := arm-none-eabi-objcopy
OBJDUMP   := arm-none-eabi-objdump
SIZE      := arm-none-eabi-size

# --- Source files ------------------------------------------------
# Boot / system / drivers (bare-metal skeleton)
SKEL_SRC  := src/main.c \
             src/system_stm32u5xx.c \
             src/drivers/gpio.c \
             src/drivers/uart.c
SKEL_ASM  := src/startup_stm32u575xx.s

# Algorithm library (existing, platform-independent)
ALGO_SRC  := Core/Src/ahrs.c \
             Core/Src/fall_detect.c

ALL_C_SRCS := $(SKEL_SRC) $(ALGO_SRC)
ALL_S_SRCS := $(SKEL_ASM)

OBJS := $(addprefix $(BUILD_DIR)/, \
           $(ALL_C_SRCS:.c=.o) \
           $(ALL_S_SRCS:.s=.o))

# --- Compiler flags ----------------------------------------------
CPU       := -mcpu=cortex-m33
FPU       := -mfloat-abi=hard -mfpu=fpv5-sp-d16
INCLUDES  := -I include -I Core/Inc

CFLAGS    := $(CPU) $(FPU) -mthumb \
             -O0 -g3 -Wall -Wextra \
             $(INCLUDES) \
             -DSTM32U575xx \
             -ffunction-sections -fdata-sections \
             -fno-common

LDFLAGS   := $(CPU) $(FPU) -mthumb \
             -T linker/STM32U575ZITX_FLASH.ld \
             -nostartfiles \
             -Wl,--gc-sections \
             -Wl,-Map=$(BUILD_DIR)/$(TARGET).map \
             --specs=nosys.specs --specs=nano.specs \
             -lc -lm -lnosys

# --- Phony targets -----------------------------------------------
.PHONY: all clean flash disasm size

all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).bin $(BUILD_DIR)/$(TARGET).hex

# --- Create build tree -------------------------------------------
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)/src/drivers \
	         $(BUILD_DIR)/Core/Src

# --- Compile C ---------------------------------------------------
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# --- Assemble ----------------------------------------------------
$(BUILD_DIR)/%.o: %.s | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -x assembler-with-cpp -c $< -o $@

# --- Link --------------------------------------------------------
$(BUILD_DIR)/$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@
	$(SIZE) $@

# --- Binary images -----------------------------------------------
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf
	$(OBJCOPY) -O binary $< $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf
	$(OBJCOPY) -O ihex $< $@

# --- Disassembly -------------------------------------------------
disasm: $(BUILD_DIR)/$(TARGET).elf
	$(OBJDUMP) -d $< > $(BUILD_DIR)/$(TARGET).disasm

# --- Clean -------------------------------------------------------
clean:
	rm -rf $(BUILD_DIR)

# --- Flash (uncomment and adapt for your debug probe) -------------
# flash: all
# 	openocd -f board/st_nucleo_u5.cfg \
# 	        -c "program $(BUILD_DIR)/$(TARGET).elf verify reset exit"
