INPUTS = src/main.cpp vendor/soda/src/environment.cpp vendor/soda/src/strings.cpp
CC = g++
CFLAGS = -std=c++20 -Wall -Wextra -Wpedantic
OUTPUT = sayve

.PHONY: all

all: $(INPUTS)
	$(CC) $(CFLAGS) -o $(OUTPUT) $(INPUTS) && strip --strip-all $(OUTPUT)

install: $(OUTPUT)
	cp -v $(OUTPUT) /usr/local/bin/

uninstall:
	rm -v /usr/local/bin/$(OUTPUT)

clean:
	rm -v $(OUTPUT)
