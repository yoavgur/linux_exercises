CFLAGS += -Wall -Wextra -Werror
BUILD_DIR = build

all: file_mon ps

ps: $(BUILD_DIR)/ps

file_mon: $(BUILD_DIR)/file_mon

$(BUILD_DIR)/%: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR):
	mkdir -p $@

clean:
	$(RM) -r $(BUILD_DIR)

.PHONY: all ps file_mon clean 
