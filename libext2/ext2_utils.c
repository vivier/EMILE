/*
 *
 * (c) 2008 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <string.h>

#include "libext2.h"
#include "ext2_utils.h"

void ext2_read_block(ext2_VOLUME* volume, unsigned int fsblock)
{
	off_t block;
	int blocksize;

	if (fsblock == volume->current)
		return;

	volume->current = fsblock;
	blocksize = volume->device->get_blocksize(volume->device->data);
	block = fsblock * (EXT2_BLOCK_SIZE(volume->super) / blocksize);
	volume->device->read_sector(volume->device->data, block,
				    volume->buffer,
				    EXT2_BLOCK_SIZE(volume->super));
}

void ext2_get_group_desc(ext2_VOLUME* volume,
		   int group_id, struct ext2_group_desc *gdp)
{
	unsigned int block, offset;

	block = 1 + volume->super->s_first_data_block;
	block += group_id / EXT2_DESC_PER_BLOCK(volume->super);
	ext2_read_block(volume,  block);

	offset = group_id % EXT2_DESC_PER_BLOCK(volume->super);
	offset *= sizeof(*gdp);

	memcpy(gdp, volume->buffer + offset, sizeof(*gdp));
}

int ext2_get_inode(ext2_VOLUME* volume,
		    unsigned int ino, struct ext2_inode *inode)
{
	struct ext2_group_desc desc;
	unsigned int block;
	unsigned int group_id;
	unsigned int offset;

	ino--;

	group_id = ino / EXT2_INODES_PER_GROUP(volume->super);
	ext2_get_group_desc(volume, group_id, &desc);

	ino %= EXT2_INODES_PER_GROUP(volume->super);

	block = desc.bg_inode_table;
	block += ino / (EXT2_BLOCK_SIZE(volume->super) /
			EXT2_INODE_SIZE(volume->super));
	ext2_read_block(volume, block);

	offset = ino % (EXT2_BLOCK_SIZE(volume->super) /
			EXT2_INODE_SIZE(volume->super));
	offset *= EXT2_INODE_SIZE(volume->super);

	memcpy(inode, volume->buffer + offset, sizeof(*inode));

	return 0;
}

unsigned int ext2_get_block_addr(ext2_VOLUME* volume, struct ext2_inode *inode,
				 unsigned int logical)
{
	unsigned int physical;
	unsigned int addr_per_block;

	/* direct */

	if (logical < EXT2_NDIR_BLOCKS) {
		physical = inode->i_block[logical];
		return physical;
	}

	/* indirect */
	
	logical -= EXT2_NDIR_BLOCKS;

	addr_per_block = EXT2_ADDR_PER_BLOCK (volume->super);
	if (logical < addr_per_block) {
		ext2_read_block(volume, inode->i_block[EXT2_IND_BLOCK]);
		physical = ((unsigned int *)volume->buffer)[logical];
		return physical;
	}

	/* double indirect */

	logical -=  addr_per_block;

	if (logical < addr_per_block * addr_per_block) {
		ext2_read_block(volume, inode->i_block[EXT2_DIND_BLOCK]);
		physical = ((unsigned int *)volume->buffer)
						[logical / addr_per_block];
		ext2_read_block(volume, physical);
		physical = ((unsigned int *)volume->buffer)
						[logical % addr_per_block];
		return physical;
	}

	/* triple indirect */

	logical -= addr_per_block * addr_per_block;
	ext2_read_block(volume, inode->i_block[EXT2_DIND_BLOCK]);
	physical = ((unsigned int *)volume->buffer)
				[logical / (addr_per_block * addr_per_block)];
	ext2_read_block(volume, physical);
	logical = logical % (addr_per_block * addr_per_block);
	physical = ((unsigned int *)volume->buffer)[logical / addr_per_block];
	ext2_read_block(volume, physical);
	physical = ((unsigned int *)volume->buffer)[logical % addr_per_block];
	return physical;
}

int ext2_read_data(ext2_VOLUME* volume, struct ext2_inode *inode,
		   off_t offset, char *buffer, size_t length)
{
	unsigned int logical, physical;
	int blocksize = EXT2_BLOCK_SIZE(volume->super);
	int shift;
	size_t read;

	if (offset >= inode->i_size)
		return -1;

	if (offset + length >= inode->i_size)
		length = inode->i_size - offset;

	read = 0;
	logical = offset / blocksize;
	shift = offset % blocksize;

	if (shift) {
		physical = ext2_get_block_addr(volume, inode, logical);
		ext2_read_block(volume, physical);

		if (length < blocksize - shift) {
			memcpy(buffer, volume->buffer + shift, length);
			return length;
		}
		read += blocksize - shift;
		memcpy(buffer, volume->buffer + shift, read);

		buffer += read;
		length -= read;
		logical++;
	}

	while (length) {
		physical = ext2_get_block_addr(volume, inode, logical);
		ext2_read_block(volume, physical);

		if (length < blocksize) {
			memcpy(buffer, volume->buffer, length);
			read += length;
			return read;
		}
		memcpy(buffer, volume->buffer, blocksize);

		buffer += blocksize;
		length -= blocksize;
		read += blocksize;
		logical++;
	}

	return read;
}

off_t ext2_dir_entry(ext2_VOLUME *volume, struct ext2_inode *inode,
		     off_t index, struct ext2_dir_entry_2 *entry)
{
	int ret;

	ret = ext2_read_data(volume, inode, index,
			     (char*)entry, sizeof(*entry));
	if (ret == -1)
		return -1;

	return index + entry->rec_len;
}

unsigned int ext2_seek_name(ext2_VOLUME *volume, char *name)
{
	struct ext2_inode inode;
	int ret;
	unsigned int ino;
	off_t index;
	struct ext2_dir_entry_2 entry;

	ino = EXT2_ROOT_INO;
	while(*name) {
		if (*name == '/')
			name++;
		ret = ext2_get_inode(volume, ino, &inode);
		if (ret == -1)
			return 0;
		index = 0;
		while (1) {
			index = ext2_dir_entry(volume, &inode, index, &entry);
			if (index == -1)
				return 0;
			ret = strncmp(name, entry.name, entry.name_len);
			if (ret == 0  &&
			    (name[entry.name_len] == 0 ||
			     name[entry.name_len] == '/')) {
			     	ino = entry.inode;
				break;
			}
		}
		name += entry.name_len;
	}

	return ino;
}
