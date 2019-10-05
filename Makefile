TARGET = kernel.bin
CFLAGS = -Wall -Wextra -nostdinc -nostdlib -fno-builtin -fno-common
LDFLAGS = -Map kernel.map -s -x -T kernel.ld

$(TARGET): main.o
	ld $(LDFLAGS) -o $@ $+

%.o: %.c
	gcc $(CFLAGS) -c -o $@ $<

run:$(TARGET)
	mv $(TARGET) ./fs/
	qemu-system-x86_64 -bios OVMF.fd -hda fat:rw:fs -boot c
clean:
	rm -f *~ *.o *.map ./fs/$(TARGET) $(TARGET)

.PHONY: clean
