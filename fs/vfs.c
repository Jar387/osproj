#include <fs/vfs.h>
#include <printk.h>
#include <lib/list.h>
#include <lib/string.h>
#include <lib/bst.h>
#include <stddef.h>
#include <mm/slab.h>
#include <sched.h>
#include <errno.h>
#include <asm/ring0.h>

#define PATH_LEN(s) (sizeof(s)-1)

static list_node_t *mount_point_head;

static bst_node_t *fs_driver_head;

static bst_node_t *fd_head;

static mount_point_t mount_point_root = {.driver_id = MINIX_FS_DRV,.path =
	    "/",.path_len = PATH_LEN("/")
};

static mount_point_t mount_point_dev = {.driver_id = DEVFS_FS_DRV,.path =
	    "/dev",.path_len = PATH_LEN("/dev")
};

fs_driver_t minix_fs_drv = {
	.block_size = 1024,
	.check_type = NULL,
	.id = MINIX_FS_DRV,
	.is_mem = 0,
	.name = "minix",
	.ptrs = { NULL }
};

fs_driver_t devfs_fs_drv = {
	.block_size = 0,
	.is_mem = 1,
	.check_type = NULL,
	.id = DEVFS_FS_DRV,
	.name = "devfs",
	.ptrs = { NULL }
};

static char *path_swap;
static mount_point_t *mp_swap;
static int most_long;
static int
get_fs_type_callback(list_node_t * m)
{
	mount_point_t *mp = (mount_point_t *) (m->data);
	if (!memcmp(mp->path, path_swap, mp->path_len)) {
		if (mp->path_len > most_long) {
			most_long = mp->path_len;
			mp_swap = mp;
		}
	}
	return 0;
}

static int
get_fs_type(const char *path, const char **baked, int *fs_type)
{
	char *buf;
	mp_swap = NULL;
	most_long = 0;
	if (path[0] != '/') {
		// not a absolute path
		buf =
		    (char *) kmalloc(strlen(curr_task->pwd) + strlen(path) + 1);
		strcpy(buf, curr_task->pwd);
		strcat(buf, path);
	} else {
		buf = (char *) kmalloc(strlen(path) + 1);
		strcpy(buf, path);
	}
	path_swap = buf;

	list_iter(mount_point_head, &get_fs_type_callback);
	if (mp_swap == NULL) {
		*baked = NULL;
		*fs_type = 0;
		kfree(buf);
		return 1;
	}
	*baked = path + mp_swap->path_len;
	if ((**baked) == '/') {
		(*baked)++;
	}
	kfree(buf);
	*fs_type = mp_swap->driver_id;
	return 0;
}

static int
get_fd_info(int id, int *type, int *inode)
{
	fd_t *fd = (fd_t *) bst_search(fd_head, id);
	if (fd == NULL) {
		return 1;
	}
	*type = fd->driver_id;
	*inode = fd->internal_inode;
	return 0;
}

int
fs_close(int fd)
{
	int type, inode;
	if (get_fd_info(fd, &type, &inode)) {
		return -ENFILE;
	}
	int (*close)(int) =
	    ((fs_driver_t *) bst_search(fs_driver_head, type))->ptrs.close;
	if (close == NULL) {
		return -EPERM;
	}
	return close(inode);
}

int
fs_mkdir(const char *path, unsigned short mode)
{
	const char *baked;
	int type;
	if (get_fs_type(path, &baked, &type)) {
		return -ENOENT;
	}
	int (*mkdir)(const char *, unsigned short) =
	    ((fs_driver_t *) bst_search(fs_driver_head, type))->ptrs.mkdir;
	if (mkdir == NULL) {
		return -EPERM;
	}
	return mkdir(baked, mode);
}

int
fs_open(const char *path, int oflag)
{
	const char *baked;
	int type;
	if (get_fs_type(path, &baked, &type)) {
		return -ENOENT;
	}
	int (*open)(const char *, int) =
	    ((fs_driver_t *) bst_search(fs_driver_head, type))->ptrs.open;
	if (open == NULL) {
		return -EPERM;
	}
	return open(baked, oflag);
}

int
fs_read(int fd, void *buf, int nbytes)
{
	int type, inode;
	if (get_fd_info(fd, &type, &inode)) {
		return -ENFILE;
	}
	int (*read)(int, void *, int) =
	    ((fs_driver_t *) bst_search(fs_driver_head, type))->ptrs.read;
	if (read == NULL) {
		return -EPERM;
	}
	return read(inode, buf, nbytes);
}

int
fs_write(int fd, const void *buf, int nbytes)
{
	int type, inode;
	if (get_fd_info(fd, &type, &inode)) {
		return -ENFILE;
	}
	int (*write)(int, const void *, int) =
	    ((fs_driver_t *) bst_search(fs_driver_head, type))->ptrs.write;
	if (write == NULL) {
		return -EPERM;
	}
	return write(inode, buf, nbytes);
}

void
fs_init()
{
	list_insert(&mount_point_head, &mount_point_root);
	list_insert(&mount_point_head, &mount_point_dev);

	bst_insert(&fs_driver_head, &minix_fs_drv, MINIX_FS_DRV);
	bst_insert(&fs_driver_head, &devfs_fs_drv, DEVFS_FS_DRV);
}
