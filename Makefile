
CC ?= gcc

target:
	$(CC) -Wall -o out main.cpp sysinfo.cpp

clean:
	rm -rf out
