/*
 *
 * (c) 2004,2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#define __NO_INLINE__

#include <stdio.h>
#include <malloc.h>
#include <elf.h>
#include <string.h>

#include <macos/types.h>
#include <macos/devices.h>
#include <libstream.h>

#include "console.h"
#include "bank.h"
#include "misc.h"

#include "load.h"

#define PAGE_SHIFT      12
#define PAGE_SIZE       (1UL << PAGE_SHIFT)

#define BAR_STEP	40

static int bar_read(stream_t *stream, char*buffer, int size, 
		    int current, int total_size)
{
	int read = 0;
	int blksize = (total_size + BAR_STEP - 1) / BAR_STEP;
	int ret;

	console_cursor_restore();
	printf(" %d %%", ((current + read) * 100 + total_size / 2) / total_size);
	while (size)
	{
		if (blksize > size)
			blksize = size;
		ret = stream_read(stream, buffer, blksize);
		read += ret;
		if (ret != blksize)
			break;
		console_cursor_restore();
		printf("#");
		console_cursor_save();
		printf(" %d %%", ((current + read) * 100 + total_size / 2) / total_size);
		buffer += ret;
		size -= ret;
	}
	console_cursor_restore();
	printf(" %d %%", ((current + read) * 100 + total_size / 2) / total_size);

	return read;
}

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
	int read;
	int to_read;

	printf("Loading kernel  ");

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
	to_read = 0;
	for (i = 0; i < elf_header.e_phnum; i++)
	{
		min_addr = (min_addr > program_header[i].p_vaddr) ?
				program_header[i].p_vaddr : min_addr;
		max_addr = (max_addr < program_header[i].p_vaddr + program_header[i].p_memsz) ?
				program_header[i].p_vaddr + program_header[i].p_memsz: max_addr;
		to_read += program_header[i].p_filesz;
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

	*base = min_addr;
	*entry = elf_header.e_entry;
	*size = kernel_size;

	kernel = (char*)malloc_contiguous(kernel_size + 4 + bootstrap_size);
	kernel = (unsigned char*)(((unsigned long)kernel + 3) & 0xFFFFFFFC);
	if (!check_full_in_bank((unsigned long)kernel, kernel_size))
		error("Kernel between two banks, contact maintainer\n");

	memset(kernel, 0, kernel_size);
	read = 0;
	console_cursor_save();
	for (i = 0; i < elf_header.e_phnum; i++)
	{
		ret = stream_lseek(stream, program_header[i].p_offset, SEEK_SET);
		if (ret != program_header[i].p_offset)
		{
			error("Cannot seek");
		}
		ret = bar_read( stream, 
				kernel + program_header[i].p_vaddr - PAGE_SIZE,
				program_header[i].p_filesz,
				read, to_read);
		if (ret != program_header[i].p_filesz)
		{
			printf("Read %d instead of %d\n", 
					ret, program_header[i].p_filesz);
			error("Cannot load");
		}
		read += ret;
	}
	putchar('\n');
	
	ret = stream_close(stream);

	return kernel;
}

char *load_ramdisk(char* path, unsigned long *ramdisk_size)
{
	stream_t *stream;
	char *ramdisk_start;
	struct stream_stat stat;
	int ret;

	stream = stream_open(path);
	if (stream == NULL)
		return NULL;

	stream_fstat(stream, &stat);

	ramdisk_start = (char*)malloc_top(stat.st_size + 4);
	ramdisk_start = (char*)(((unsigned long)ramdisk_start + 3) & 0xFFFFFFFC);

	if (!check_full_in_bank((unsigned long)ramdisk_start, stat.st_size))
		error("ramdisk between two banks, contact maintainer\n");

	printf("Loading RAMDISK ");

	console_cursor_save();
	ret = bar_read(stream, ramdisk_start, stat.st_size, 0, stat.st_size);
	putchar('\n');
	if (ret != stat.st_size)
		error("Cannot load");
	stream_close(stream);

	*ramdisk_size = stat.st_size;

	return ramdisk_start;
}
