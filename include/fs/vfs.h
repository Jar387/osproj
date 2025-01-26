#ifndef VFS_H
#define VFS_H

void fs_init();

typedef struct {
	unsigned char is_mem;
	unsigned int block_size;
	char *name;
	unsigned char (*check_type)(char *superblock);
} fs_driver_t;

typedef struct {
	char *path;
	int driver_id;
} mount_point_t;

#endif
