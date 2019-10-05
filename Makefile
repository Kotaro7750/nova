TARGET = kernel.bin
CFLAGS = -Wall -Wextra -nostdinc -nostdlib -fno-builtin -fno-common 
LDFLAGS = -Map kernel.map -s -x -T kernel.ld

$(TARGET):main.o fb.o 
	ld $(LDFLAGS) -o $@ $+

%.o: %.c
	gcc $(CFLAGS) -c -o $@ $<

run:$(TARGET)
	mv $(TARGET) ./fs/
	qemu-system-x86_64 -m 4G -bios OVMF.fd -hda fat:rw:fs -boot c
clean:
	rm -f *~ *.o *.map ./fs/$(TARGET) $(TARGET) include/*~

.PHONY: clean
