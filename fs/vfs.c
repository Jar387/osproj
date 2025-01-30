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
	    "/",.path_len = PATH_LEN("/")
};

mount_point_t mount_point_dev = {.driver_id = DEVFS_FS_DRV,.path =
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

static void
get_fs_type(char *path, char **baked, int *fs_type)
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
		return;
	}
	*baked = path + mp_swap->path_len;
	if ((**baked) == '/') {
		(*baked)++;
	}
	*fs_type = mp_swap->driver_id;
}

void
fs_init()
{
	mount_point_head = list_insert(NULL, &mount_point_root);
	list_insert(mount_point_head, &mount_point_dev);

	fs_driver_head = list_insert(fs_driver_head, &minix_fs_drv);
	list_insert(fs_driver_head, &devfs_fs_drv);
}
