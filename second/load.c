/*
 *
 * (c) 2004,2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#define __NO_INLINE__

#include <stdio.h>
#include <malloc.h>
#include <elf.h>
#include <string.h>
#include <sys/types.h>

#include <macos/types.h>
#include <macos/devices.h>
#include <libui.h>
#include <libstream.h>

#include "console.h"
#include "bank.h"
#include "misc.h"

#include "load.h"

#define PAGE_SHIFT      12
#define PAGE_SIZE       (1UL << PAGE_SHIFT)

#define BAR_STEP	40

static int bar_read(stream_t *stream, emile_progressbar_t *pg,
		    char*buffer, int size, 
		    int current, int total_size)
{
	int read = 0;
	int blksize = (total_size + BAR_STEP - 1) / BAR_STEP;
	int ret;

	while (size)
	{
		if (blksize > size)
			blksize = size;
		ret = stream_read(stream, buffer, blksize);
		read += ret;
		if (ret != blksize)
			break;
		emile_progressbar_value(pg, current + read);
		buffer += ret;
		size -= ret;
	}

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
	emile_window_t win;
	emile_progressbar_t *pg;
	int l, c;

	console_get_size(&l, &c);

	win.l = l - 6;
	win.h = 1;

	win.w = (c * 75 + 50) / 100;
	win.c = (c - win.w + 1) / 2;

	console_set_cursor_position(win.l - 2, win.c + (win.w - strlen("Loading kernel")) / 2);
	printf("Loading kernel");

	stream = stream_open(path);
	if (stream == NULL)
	{
		printf("Cannot open kernel\n");
		return NULL;
	}

	stream_uncompress(stream);

	ret = stream_read(stream, &elf_header, sizeof(Elf32_Ehdr));
	if (ret != sizeof(Elf32_Ehdr))
	{
		printf("Cannot read kernel ELF header\n");
		stream_close(stream);
		return NULL;
	}

#ifdef ARCH_M68K
	if  (elf_header.e_machine != EM_68K)
	{
		printf( "Not MC680x0 architecture\n");
		stream_close(stream);
		return NULL;
	}
#endif /* ARCH_M68K */
#ifdef ARCH_PPC
	if  (elf_header.e_machine != EM_PPC)
	{
		printf( "Not PowerPC architecture\n");
		stream_close(stream);
		return NULL;
	}
#endif /* ARCH_PPC */

	if (elf_header.e_type != ET_EXEC)
	{
		printf( "Not an executable file\n");
		stream_close(stream);
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
		if (program_header[i].p_memsz == 0)
			continue;
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

	kernel = (char*)malloc_contiguous(kernel_size + PAGE_SIZE + bootstrap_size);
	kernel = (char*)(((unsigned long)kernel + PAGE_SIZE) & ~(PAGE_SIZE - 1));
	if (!check_full_in_bank((unsigned long)kernel, kernel_size))
	{
		printf("Kernel between two banks, contact maintainer\n");
		free(kernel);
		stream_close(stream);
		return NULL;
	}

	memset(kernel, 0, kernel_size);
	read = 0;
	pg = emile_progressbar_create(&win, to_read);
	for (i = 0; i < elf_header.e_phnum; i++)
	{
		ret = stream_lseek(stream, program_header[i].p_offset, SEEK_SET);
		if (ret != program_header[i].p_offset)
		{
			printf("Cannot seek\n");
			stream_close(stream);
			free(kernel);
			return NULL;
		}
		ret = bar_read( stream, pg,
				kernel + program_header[i].p_vaddr - min_addr,
				program_header[i].p_filesz,
				read, to_read);
		if (ret != program_header[i].p_filesz)
		{
			printf("Read %d instead of %d\n", 
					ret, program_header[i].p_filesz);
			printf("Cannot load\n");
			free(kernel);
			stream_close(stream);
			return NULL;
		}
		read += ret;
	}
	emile_progressbar_value(pg, to_read);
	emile_progressbar_delete(pg);
	
	ret = stream_close(stream);

	return kernel;
}

char *load_ramdisk(char* path, unsigned long *ramdisk_size)
{
	stream_t *stream;
	char *ramdisk_start;
	char *ramdisk;
	struct stream_stat stat;
	int ret;
	emile_window_t win;
	emile_progressbar_t *pg;
	int l, c;

	console_get_size(&l, &c);

	win.l = l - 2;
	win.h = 1;

	win.w = (c * 75 + 50) / 100;
	win.c = (c - win.w + 1) / 2;

	stream = stream_open(path);
	if (stream == NULL)
	{
		printf("Cannot open ramdisk\n");
		return NULL;
	}

	stream_fstat(stream, &stat);

	ramdisk = (char*)malloc_top(stat.st_size + 4);
	ramdisk_start = (char*)(((unsigned long)ramdisk + 3) & 0xFFFFFFFC);

	if (!check_full_in_bank((unsigned long)ramdisk_start, stat.st_size))
	{
		printf("ramdisk between two banks, contact maintainer\n");
		free(ramdisk);
		stream_close(stream);
		return NULL;
	}

	console_set_cursor_position(win.l - 2, win.c + (win.w - strlen("Loading RAMDISK")) / 2);
	printf("Loading RAMDISK");

	pg = emile_progressbar_create(&win, stat.st_size);
	ret = bar_read(stream, pg, ramdisk_start, stat.st_size, 0, stat.st_size);
	emile_progressbar_delete(pg);
	putchar('\n');
	if (ret != stat.st_size)
	{
		printf("Cannot load\n");
		free(ramdisk);
		stream_close(stream);
	}
	stream_close(stream);

	*ramdisk_size = stat.st_size;

	return ramdisk_start;
}
