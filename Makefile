CFLAGS ?= -Wall -Wextra -Werror

ps: ps.c
	gcc $(CFLAGS) $^ -o build/$@

file_mon: file_mon.c
	gcc $(CFLAGS) $^ -o build/$@

all: file_mon ps

clean:
	rm -f ps
	rm -f file_mon
