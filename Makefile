TARGET = kernel.bin
CFLAGS = -Wall -Wextra -nostdinc -nostdlib -fno-builtin -fno-common 
LDFLAGS = -Map kernel.map -s -x -T kernel.ld

$(TARGET):main.o fbcon.o fb.o font.o kbc.o x86.o intr.o pic.o acpi.o common.o hpet.o handler.o syscall.o lock.o shell.o sched.o fs.o
	ld $(LDFLAGS) -o $@ $+

%.o: %.c
	gcc $(CFLAGS) -c -o $@ $<

%.o: %.s
	gcc $(CFLAGS) -c -o $@ $<

run:$(TARGET)
	echo -n "HELLO\0" > ./fs/HELLO.TXT
	echo -n "FOO\0" > ./fs/FOO.TXT
	echo -n "HOGE\0" > ./fs/HOGE.TXT
	./fs/create_fs.sh HELLO.TXT FOO.TXT HOGE.TXT
	mv $(TARGET) ./fs/
	qemu-system-x86_64 -m 4G -bios OVMF.fd -hda fat:rw:fs -boot c

clean:
	rm -f *~ *.o *.map ./fs/$(TARGET) $(TARGET) include/*~ ./fs/*TXT

.PHONY: clean
