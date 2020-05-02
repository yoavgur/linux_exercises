CFLAGS ?= -Wall -Wextra -Werror

ps: ps.c
	gcc $(CFLAGS) $^ -o $@

file_mon: file_mon.c
	gcc $(CFLAGS) $^ -o $@

clean:
	rm -f ps
	rm -f file_mon