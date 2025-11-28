# TFLM firmware for LiteX/VexRiscv
# Uses pre-built TFLM library

# Directories
BUILD_DIR = ../build/colorlight_i5
OUT = build
TFLM = tflm

# LiteX configuration
include $(BUILD_DIR)/software/include/generated/variables.mak
include $(SOC_DIRECTORY)/software/common.mak

# Toolchain override
CX = riscv-none-elf-g++
CC = riscv-none-elf-gcc

# ------------------------------------------------------------
# *** IMPORTANTE ***
# Removidos filtros que estavam quebrando o runtime newlib
# ------------------------------------------------------------

# TFLM defines
TFLM_DEFINES = -DTF_LITE_STATIC_MEMORY -DTF_LITE_DISABLE_X86_NEON
TFLM_DEFINES += -DGEMMLOWP_ALLOW_SLOW_SCALAR_FALLBACK
TFLM_DEFINES += -DTF_LITE_USE_GLOBAL_CMATH_FUNCTIONS
TFLM_DEFINES += -DTF_LITE_USE_GLOBAL_MAX -DTF_LITE_USE_GLOBAL_MIN

# TFLM includes
TFLM_INCLUDES = -I$(TFLM) -I$(TFLM)/third_party/flatbuffers/include
TFLM_INCLUDES += -I$(TFLM)/third_party/gemmlowp -I$(TFLM)/third_party/ruy

# Application flags
APP_FLAGS = $(CPUFLAGS) -Os -g -std=c++17
APP_FLAGS += -fno-rtti -fno-exceptions -ffunction-sections -fdata-sections -Wall
APP_FLAGS += -I. -I$(BUILDINC_DIRECTORY)
APP_FLAGS += -I$(SOC_DIRECTORY)/software/include -I$(SOC_DIRECTORY)/software/libbase
APP_FLAGS += -I$(CPU_DIRECTORY) -Iplatform
APP_FLAGS += $(TFLM_INCLUDES) $(TFLM_DEFINES)

# Sources
APP_SOURCES = main.cc tflm/examples/hello_world/models/hello_world_int8_model_data.cc
PLATFORM_C_SOURCES = platform/sbrk.c

# Objects
APP_OBJECTS = $(patsubst %.cc,$(OUT)/%.o,$(APP_SOURCES))
PLATFORM_C_OBJECTS = $(patsubst %.c,$(OUT)/%.o,$(PLATFORM_C_SOURCES))
OBJECTS = $(OUT)/crt0.o $(APP_OBJECTS) $(PLATFORM_C_OBJECTS)

# Pre-built TFLM library
TFLM_LIB = $(TFLM)/build/libtflm.a

# ------------------------------------------------------------
# libs CORRETAS para LiteX + newlib + C++
# ------------------------------------------------------------
STD_LIBS = -lstdc++ -lc -lm -lgcc

# Build
all: $(OUT)/main.bin

$(OUT)/main.bin: $(OUT)/main.elf
	$(OBJCOPY) -O binary $< $@
	@echo "Firmware ready: $@"

$(OUT)/main.elf: $(OBJECTS) $(TFLM_LIB)
	$(CX) $(LDFLAGS) -T linker.ld -N \
		-Wl,--start-group $^ \
		$(PACKAGES:%=-L$(BUILD_DIR)/software/%) \
		$(LIBS:lib%=-l%) $(STD_LIBS) \
		-Wl,--end-group \
		-Wl,--gc-sections -Wl,--allow-multiple-definition \
		-Wl,-Map,$@.map -o $@
	@echo "Firmware size:"
	@$(TRIPLE)-size $@

$(OUT)/platform/%.o: platform/%.cc
	@mkdir -p $(dir $@)
	$(CX) $(APP_FLAGS) -c $< -o $@

$(OUT)/%.o: %.cc
	@mkdir -p $(dir $@)
	$(CX) $(APP_FLAGS) -c $< -o $@

$(OUT)/platform/%.o: platform/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT)/crt0.o: $(CPU_DIRECTORY)/crt0.S
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OUT)

.PHONY: all clean
