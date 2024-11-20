cc		:= gcc
cflags	:= -Wall

exec	:= chardev-read chardev-write
obj		:= $(foreach filename, $(exec), $(filename).o)

all: $(exec)

%: %.o
	$(cc) $(cflags) -o $^ $<

.PHONY: clean

clean:
	- rm $(exec) $(obj)
