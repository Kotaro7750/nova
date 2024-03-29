TARGET = kernel.bin
CFLAGS = -Wall -Wextra -nostdinc -nostdlib -fno-builtin -fno-common 
LDFLAGS = -Map kernel.map -s -x -T kernel.ld

$(TARGET):main.o fbcon.o fb.o font.o kbc.o x86.o intr.o pic.o acpi.o common.o hpet.o handler.o syscall.o lock.o shell.o sched.o
	ld $(LDFLAGS) -o $@ $+

%.o: %.c
	gcc $(CFLAGS) -c -o $@ $<

%.o: %.s
	gcc $(CFLAGS) -c -o $@ $<

run:$(TARGET)
	mv $(TARGET) ./fs/
	qemu-system-x86_64 -m 4G -bios OVMF.fd -hda fat:rw:fs -boot c

clean:
	rm -f *~ *.o *.map ./fs/$(TARGET) $(TARGET) include/*~

.PHONY: clean
