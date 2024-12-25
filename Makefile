build:
	nasm -f elf32 kernel.asm -o kasm.o
	gcc -m32 -c kernel.c -o kc.o -lioperm
	ld -m elf_i386 -T link.ld -o kernel kasm.o kc.o

run:
	qemu-system-i386 -kernel kernel -machine pcspk-audiodev=spk -audiodev id=spk,driver=pa

clean:
	rm -f kasm.o kc.o kernel
