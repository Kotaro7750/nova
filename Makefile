TARGET = kernel.bin
CFLAGS = -Wall -Wextra -nostdinc -nostdlib -fno-builtin -fno-common -g -no-pie
LDFLAGS = -Map kernel.map -s -x -T kernel.ld

$(TARGET):main.o fbcon.o fb.o font.o kbc.o x86.o intr.o pic.o handler.o paging.o
	ld $(LDFLAGS) -o $@ $+

%.o: %.c
	gcc $(CFLAGS) -c -o $@ $<

%.o: %.s
	gcc $(CFLAGS) -c -o $@ $<

run:$(TARGET)
	mv $(TARGET) ./fs/
	qemu-system-x86_64 -m 4G -bios OVMF.fd -hda fat:rw:fs -boot c -no-reboot -no-shutdown -monitor stdio

clean:
	rm -f *~ *.o *.map ./fs/$(TARGET) $(TARGET) include/*~

elf:main.c fbcon.c fb.c font.c kbc.c x86.c intr.c pic.c handler.s paging.c
	gcc -o elf -no-pie $+

.PHONY: clean
