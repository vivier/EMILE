/*
 *
 * (c) 2004,2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdio.h>
#include <malloc.h>
#include <elf.h>
#include <string.h>

#include <macos/types.h>
#include <macos/devices.h>
#include <libstream.h>

#include "bank.h"
#include "misc.h"

#include "load.h"

#define PAGE_SHIFT      12
#define PAGE_SIZE       (1UL << PAGE_SHIFT)

char* load_kernel(char* path, int bootstrap_size,
		  unsigned long *base, unsigned long *entry, unsigned long *size)
{
	Elf32_Ehdr elf_header;
	Elf32_Phdr *program_header;
	int ret;
	unsigned long min_addr, max_addr, kernel_size;
	int i;
	char *kernel;
	stream_t *stream;

	stream = stream_open(path);
	if (stream == NULL)
		return NULL;

	stream_uncompress(stream);

	ret = stream_read(stream, &elf_header, sizeof(Elf32_Ehdr));
	if (ret != sizeof(Elf32_Ehdr))
		error("Cannot read\n");

	if  (elf_header.e_machine != EM_68K)
	{
		printf( "Not MC680x0 architecture\n");
		return NULL;
	}

	if (elf_header.e_type != ET_EXEC)
	{
		printf( "Not an executable file\n");
		return NULL;
	}

	program_header = (Elf32_Phdr *)malloc(elf_header.e_phnum *
					      sizeof (Elf32_Phdr));
	if (program_header == NULL)
	{
		printf( "Cannot allocate memory\n");
		return NULL;
	}

	ret = stream_lseek(stream, elf_header.e_phoff, SEEK_SET);

	ret = stream_read(stream, program_header, elf_header.e_phnum * sizeof(Elf32_Phdr));

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
	printf( "Kernel memory footprint: %ld\n", kernel_size);
	printf( "Base address: 0x%lx\n", min_addr);
	printf( "Entry point: 0x%lx\n", (unsigned long)elf_header.e_entry);

	*base = min_addr;
	*entry = elf_header.e_entry;
	*size = kernel_size;

	kernel = (char*)malloc_contiguous(kernel_size + 4 + bootstrap_size);
	kernel = (unsigned char*)(((unsigned long)kernel + 3) & 0xFFFFFFFC);
	if (!check_full_in_bank((unsigned long)kernel, kernel_size))
		error("Kernel between two banks, contact maintainer\n");
	printf("Loading at address %p\n", kernel);

	memset(kernel, 0, kernel_size);
	for (i = 0; i < elf_header.e_phnum; i++)
	{
		printf("Reading Program Section #%d, "
		       "offset 0x%lx, (0x%lx,%lx)\n", 
		       i, (long)program_header[i].p_offset, 
		       program_header[i].p_vaddr - PAGE_SIZE, 
		       (long)program_header[i].p_filesz);
		ret = stream_lseek(stream, program_header[i].p_offset, SEEK_SET);
		ret = stream_read(stream, 
			     kernel + program_header[i].p_vaddr - PAGE_SIZE, 
			     program_header[i].p_filesz);
	}
	
	ret = stream_close(stream);

	return kernel;
}

char *load_ramdisk(char* path, int *ramdisk_size)
{
	stream_t *stream;
	char *ramdisk_start;
	struct stream_stat stat;

	stream = stream_open(path);
	if (stream == NULL)
		return NULL;

	stream_fstat(stream, &stat);

	printf("RAMDISK size is %d Bytes\n", (int)stat.st_size);

	ramdisk_start = (char*)malloc_top(stat.st_size + 4);
	ramdisk_start = (char*)(((unsigned long)ramdisk_start + 3) & 0xFFFFFFFC);

	printf("RAMDISK base at %p\n", ramdisk_start);

	if (!check_full_in_bank((unsigned long)ramdisk_start, stat.st_size))
		error("ramdisk between two banks, contact maintainer\n");

	printf("Loading RAMDISK...\n");

	stream_read(stream, ramdisk_start, stat.st_size);
	stream_close(stream);

	*ramdisk_size = stat.st_size;

	return ramdisk_start;
}
