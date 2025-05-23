BIN := superclock
BUILD_DIR := ./build
SRC_DIRS := ./src
MCU ?= atmega32a
F_CPU ?= 16000000UL

CC := avr-gcc
CXX := avr-g++
OBJCOPY := avr-objcopy

SRCS := \
  $(shell find $(SRC_DIRS) -name "*.cpp") \
  $(shell find $(SRC_DIRS) -name "*.c")
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)
FONT_SCRIPT := ./src/driver/display/font/generate_font.py

INC_DIRS := $(SRC_DIRS)
INC_FLAGS := $(addprefix -I,$(INC_DIRS)) -MMD -MP

COMMON_FLAGS := $(INC_FLAGS) -mmcu=$(MCU) -Wall -Werror -fno-exceptions \
  -fdata-sections -ffunction-sections -fno-threadsafe-statics -g -Os -flto \
  -DF_CPU=$(F_CPU) 
CFLAGS := $(COMMON_FLAGS) -std=c11
CXXFLAGS := $(COMMON_FLAGS) -std=c++17 
LDFLAGS := -mmcu=$(MCU) -Wl,-Map,$(BUILD_DIR)/$(BIN).map -Wl,--gc-sections -flto -lm

$(BUILD_DIR)/${BIN}.hex: $(BUILD_DIR)/${BIN}.elf
	${OBJCOPY} -O ihex $< $@

$(BUILD_DIR)/${BIN}.elf: ${OBJS}
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)
	avr-size --format=avr --mcu=$(MCU) $@
	avr-objdump -h -S $@ > $(BUILD_DIR)/${BIN}.lss

$(BUILD_DIR)/%.c.o: %.c Makefile
	mkdir -p $(dir $@)
	$(CC) $(EXTRA_FLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.cpp.o: %.cpp Makefile
	mkdir -p $(dir $@)
	$(CXX) $(EXTRA_FLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)

.PHONY: fuse
fuse:
	avrdude -B 32kHz -F -V -c usbasp -p $(MCU) -U hfuse:w:0xD1:m -U lfuse:w:0xBF:m

.PHONY: flash
flash: $(BUILD_DIR)/${BIN}.hex
	avrdude -B 1.5Mhz -F -V -c usbasp -p $(MCU) -U flash:w:$<:i

.PHONY: font
font:
	python3 $(FONT_SCRIPT) || python $(FONT_SCRIPT)

-include $(DEPS)
