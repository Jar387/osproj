#include <fs/vfs.h>
#include <printk.h>
#include <lib/list.h>
#include <lib/string.h>
#include <stddef.h>
#include <mm/slab.h>
#include <sched.h>
#include <asm/ring0.h>

#define PATH_LEN(s) (sizeof(s)-1)

list_node_t *mount_point_head;
list_node_t *fs_driver_head;

mount_point_t mount_point_root = {.driver_id = MINIX_FS_DRV,.path =
	    "/",.path_len = PATH_LEN("/") };
mount_point_t mount_point_dev = {.driver_id = DEVFS_FS_DRV,.path =
	    "/dev",.path_len = PATH_LEN("/dev") };

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
static int fs_type_swap;
static int
get_fs_type_callback(list_node_t * m)
{
	printk("%s\n", path_swap);
	mount_point_t *mp = (mount_point_t *) (m->data);
	if (!memcmp(mp->path, path_swap, mp->path_len)) {
		path_swap += mp->path_len;
		fs_type_swap = mp->driver_id;
		return 1;
	}
}

static int
get_fs_type(char *path, char **baked, int *fs_type)
{
	printk("aaa\n");
	char *buf;
	if (path[0] != '/') {
		// not a absolute path
		buf =
		    (char *) kmalloc(strlen(curr_task->pwd) + strlen(path) + 1);
		strcpy(buf, curr_task->pwd);
		strcat(buf, path);
	} else {
		printk("bbb\n");
		bp();
		buf = (char *) kmalloc(strlen(path) + 1);
		printk("%s %x %x\n", path, path, buf);
		strcpy(buf, path);
	}
	path_swap = buf;
	list_iter(mount_point_head, &get_fs_type_callback);
	*baked = path_swap;
	*fs_type = fs_type_swap;
}

void
fs_init()
{
	list_insert(mount_point_head, &mount_point_root);
	list_insert(mount_point_head, &mount_point_dev);

	list_insert(fs_driver_head, &minix_fs_drv);
	list_insert(fs_driver_head, &devfs_fs_drv);
	char *b;
	int id;
	get_fs_type("/dev/sdb", &b, &id);
	printk("%x %x\n", b, id);
}
