#ifndef VFS_H
#define VFS_H

void fs_init();

typedef struct {		// vfs layer convert path like "/dev/bus/pci/1:0" to "bus/pci/1:0"
	//                       convert fd like 123 to driver specific inode 456
	// other params are copyed
	// when a posix call SHOULD return fd, drivers SHOULD return its own stuff on fd processing
	int (*access)(const char *path, int mode);
	int (*chmod)(const char *path, unsigned short mode);
	int (*chown)(const char *path, int owner, int group);
	int (*close)(int fd);
	int (*dup)(int fd);
	int (*dup2)(int fd, int fd2);
	int (*getdirentries)(int fd, char *buf, int nbytes, long *basep);
	int (*link)(const char *name1, const char *name2);
	int (*lseek)(int fd, int offset, int whence);
	int (*mkdir)(const char *path, unsigned short mode);
	int (*mkfifo)(const char *path, unsigned short mode);
	int (*open)(const char *path, int oflag);
	int (*pipe)(int fd[2]);
	int (*read)(int fd, void *buf, int nbytes);
	int (*rename)(const char *from, const char *to);
	int (*rmdir)(const char *path);
	int (*truncate)(const char *path, int length);
	int (*write)(int fd, const void *buf, int nbytes);
} posix_fs_interface_t;

// syscall interface
int fs_close(int fd);
int fs_mkdir(const char *path, unsigned short mode);
int fs_open(const char *path, int oflag);
int fs_read(int fd, void *buf, int nbytes);
int fs_write(int fd, const void *buf, int nbytes);

typedef struct {
	int driver_id;
	int internal_inode;
	int id;
} fd_t;

typedef struct {
	unsigned char is_mem;
	unsigned int block_size;
	char *name;
	unsigned char (*check_type)(char *superblock);
	posix_fs_interface_t ptrs;
	int id;
} fs_driver_t;

typedef struct {
	char *path;
	int path_len;		// for quicker comparsion
	int driver_id;
} mount_point_t;

// add new fs support here
#define MINIX_FS_DRV 1
#define DEVFS_FS_DRV 2

extern fs_driver_t minix_fs_drv;
extern fs_driver_t devfs_fs_drv;

#endif
