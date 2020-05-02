CFLAGS ?= -Wall -Wextra -Werror

ps: ps.c
	gcc $(CFLAGS) $^ -o $@

clean:
	rm -f ps