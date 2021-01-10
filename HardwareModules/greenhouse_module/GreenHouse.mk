######################################
# source
######################################
CURR_DIR = $(shell pwd)
STM32GenDir = $(CURR_DIR)/
STM32Makefile = $(CURR_DIR)/Makefile
include $(STM32Makefile)

BUILD_DIR = $(CURR_DIR)/build

LIB_DIR = $(CURR_DIR)/lib/
APP_DIR = $(CURR_DIR)/app/
SOURCES := $(shell find $(LIB_DIR)src -name '*.c')
SOURCES += $(shell find $(APP_DIR)src -name '*.c')
STM32_INCLUDES = $(C_INCLUDES:-I%=%)

C_SOURCES := $(addprefix $(STM32GenDir), $(C_SOURCES)) $(SOURCES)
ASM_SOURCES := $(addprefix $(STM32GenDir), $(ASM_SOURCES))
C_INCLUDES := $(addprefix -I$(STM32GenDir), $(STM32_INCLUDES)) -I$(LIB_DIR)inc -I$(APP_DIR)inc
LDSCRIPT := $(addprefix $(STM32GenDir), $(LDSCRIPT))

GreenHouse: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	
	
$(BUILD_DIR):
	mkdir $@	

#######################################
# flash
#######################################
flash: $(BUILD_DIR)/$(TARGET).elf
	st-flash write $(BUILD_DIR)/$(TARGET).bin 0x8000000
	