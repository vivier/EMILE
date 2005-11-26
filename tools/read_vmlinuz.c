#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <elf.h>

#include "zlib.h"

gzFile ZEXPORT gzopen (char*, char*);

#define PAGE_SHIFT      12
#define PAGE_SIZE       (1UL << PAGE_SHIFT)

int main(int argc, char **argv)
{
	gzFile in;
	Elf32_Ehdr elf_header;
	Elf32_Phdr *program_header;
	int ret;
	unsigned long min_addr, max_addr, kernel_size;
	int i;
	char * base;

	in = gzopen("vmlinux.gz", "rb");
	ret = gzread(in, &elf_header, sizeof(Elf32_Ehdr));

	if  (elf_header.e_machine != EM_68K)
	{
		fprintf(stderr, "Not MC680x0 architecture\n");
		return 1;
	}

	if (elf_header.e_type != ET_EXEC)
	{
		fprintf(stderr, "Not an executable file\n");
		return 2;
	}

	program_header = (Elf32_Phdr *)malloc(elf_header.e_phnum *
					      sizeof (Elf32_Phdr));
	if (program_header == NULL)
	{
		fprintf(stderr, "Cannot allocate memory\n");
		return 3;
	}

	ret = gzseek(in, elf_header.e_phoff, SEEK_SET);

	ret = gzread(in, program_header, elf_header.e_phnum * sizeof(Elf32_Phdr));
	min_addr = 0xffffffff;
	max_addr = 0;
	for (i = 0; i < elf_header.e_phnum; i++)
	{
		min_addr = (min_addr > program_header[i].p_vaddr) ?
				program_header[i].p_vaddr : min_addr;
		max_addr = (max_addr < program_header[i].p_vaddr + program_header[i].p_memsz) ?
				program_header[i].p_vaddr + program_header[i].p_memsz: max_addr;
	}
	if (min_addr == 0)
	{
		min_addr = PAGE_SIZE;
		program_header[0].p_vaddr += PAGE_SIZE;
		program_header[0].p_offset += PAGE_SIZE;
		program_header[0].p_filesz -= PAGE_SIZE;
		program_header[0].p_memsz -= PAGE_SIZE;
	}
	kernel_size = max_addr - min_addr;
	fprintf(stderr, "Kernel memory footprint: %ld\n", kernel_size);
	fprintf(stderr, "Base address: 0x%lx\n", min_addr);
	fprintf(stderr, "Entry point: 0x%lx\n", (long)elf_header.e_entry);

	base = (char*)malloc(kernel_size);
	memset(base, 0, kernel_size);
	for (i = 0; i < elf_header.e_phnum; i++)
	{
		fprintf(stderr, "Reading Program Section #%d, "
				"offset 0x%lx, (0x%lx,%lx)\n",
				i, (long)program_header[i].p_offset,
				program_header[i].p_vaddr - PAGE_SIZE,
				(long)program_header[i].p_filesz);
		ret = gzseek(in, program_header[i].p_offset, SEEK_SET);
		ret = gzread(in, base + program_header[i].p_vaddr - PAGE_SIZE, program_header[i].p_filesz);
	}
	write(STDOUT_FILENO, base, kernel_size);
	
	ret = gzclose(in);

	return 0;
}
