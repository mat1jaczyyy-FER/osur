#define _K_FS_C_

#include <kernel/errno.h>
#include <kernel/memory.h>
#include <types/io.h>
#include <lib/list.h>
#include <lib/string.h>
#include "fs.h"
#include "device.h"
#include "time.h"
#include "memory.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

static kdevice_t *disk;
#define DISK_WRITE(buffer, blocks, first_block) \
k_device_send(buffer, (first_block << 16) | blocks, 0, disk);

#define DISK_READ(buffer, blocks, first_block) \
k_device_recv(buffer, (first_block << 16) | blocks, 0, disk);

static struct fs_table *ft;
static size_t ft_size;
static struct kfile_desc *last_check;
static list_t open_files;

int k_fs_init(char *disk_device, size_t bsize, size_t blocks)
{
	disk = k_device_open ( disk_device, 0 );
	assert(disk);

	//initialize disk
	ft_size = sizeof(struct fs_table) + blocks;
	ft_size = (ft_size + bsize - 1) / bsize;
	ft = kmalloc(ft_size * bsize);
	memset(ft, 0, ft_size * bsize);
	ft->file_system_type = FS_TYPE;
	strcpy(ft->partition_name, disk_device);
	ft->block_size = bsize;
	ft->blocks = blocks;
	ft->max_files = MAXFILESONDISK;

	int i;
	for (i = ft_size; i < ft->blocks; i++)
		ft->free[i] = 1;

	DISK_WRITE(ft, ft_size, 0);

	list_init(&open_files);

	last_check = NULL;

	return 0;
}

int k_fs_is_file_open(descriptor_t *desc)
{
	kobject_t *kobj;

	kobj = desc->ptr;
	if (kobj && list_find(&kobjects, &kobj->list) &&
		(kobj->flags & KTYPE_FILE) != 0)
	{
		struct kfile_desc *fd = kobj->kobject;
		if (fd && fd->id == desc->id &&
			list_find(&open_files, &fd->list))
		{
			last_check = fd;
			return 0;
		}
	}

	return -1;
}

int k_fs_open_file(char *pathname, int flags, mode_t mode, descriptor_t *desc)
{
	struct fs_node *tfd = NULL;
	char *fname = &pathname[5];

	//check for conflicting flags
	if (	((flags & O_RDONLY) && (flags & O_WRONLY)) ||
		((flags & O_RDONLY) && (flags & O_RDWR))   ||
		((flags & O_WRONLY) && (flags & O_RDWR))
	)
		return -EINVAL;

	//check if file already open
	struct kfile_desc *fd = list_get(&open_files, FIRST);
	while (fd != NULL) {
		if (strcmp(fd->tfd->node_name, fname) == 0) {
			//already open!
			//if its open for reading and O_RDONLY is set in flags
			if ((fd->flags & O_RDONLY) == (flags & O_RDONLY))
				tfd = fd->tfd;//fine, save pointer to descriptor
			else
				return -EADDRINUSE; //not fine

		}
		fd = list_get_next(&fd->list);
	}

	if (!tfd) {
		//not already open; check if such file exists in file table
		int i;
		for (i = 0; i < ft->max_files; i++) {
			if (strcmp(ft->fd[i].node_name, fname) == 0) {
				tfd = &ft->fd[i];
				break;
			}
		}
	}

	if (!tfd) {
		//file doesn't exitst
		if ((flags & (O_CREAT | O_WRONLY)) == 0)
			return -EINVAL;

		//create fs_node in fs_table
		//1. find unused descriptor
		int i;
		for (i = 0; i < ft->max_files; i++) {
			if (ft->fd[i].node_name[0] == 0) {
				tfd = &ft->fd[i];
				break;
			}
		}
		if (!tfd)
			return -ENFILE;

		//2. initialize descriptor
		strcpy(tfd->node_name, fname);
		tfd->id = i;
		timespec_t t;
		kclock_gettime (CLOCK_REALTIME, &t);
		tfd->tc = tfd->ta = tfd->tm = t;
		tfd->size = 0;
		tfd->blocks = 0;
	}

	//create kobject and a new struct kfile_desc
	kobject_t *kobj = kmalloc_kobject(sizeof(struct kfile_desc));
	kobj->flags = KTYPE_FILE;
	fd = kobj->kobject;
	fd->tfd = tfd;
	fd->flags = flags;
	fd->fp = 0;
	fd->id = k_new_id ();
	list_append(&open_files, fd, &fd->list);

	//fill desc
	desc->id = fd->id;
	desc->ptr = kobj;

	return 0;
}

int k_fs_close_file(descriptor_t *desc)
{
	struct kfile_desc *fd = last_check;
	kobject_t *kobj;

	kobj = desc->ptr;
	/* - already tested!
	if (!kobj || !list_find(&kobjects, &kobj->list))
		return -EINVAL;

	fd = kobj->kobject;
	if (!fd || fd->id != desc->id)
		return -EINVAL;
	*/
	if (!list_find_and_remove(&open_files, &fd->list))
		return -EINVAL;

	kfree_kobject ( kobj );

	return 0;
}

//op = 0 => write, otherwise =>read
int k_fs_read_write(descriptor_t *desc, void *buffer, size_t size, int op)
{
	//desc already checked, use "last_check";
	struct kfile_desc *fd = last_check;

	//sanity check
	if ((op && (fd->flags & O_WRONLY)) || (!op && (fd->flags & O_RDONLY)))
		return -EPERM;

	uint8 buf[ft->block_size];
	size_t todo = size;

	if (op) {
		size_t from_buf = fd->fp % ft->block_size;
		size_t to_buffer = 0;
		todo = MIN(fd->tfd->size - fd->fp, todo);
		size_t sz = MIN(ft->block_size - from_buf, todo);

		while (todo > 0 && fd->fp < fd->tfd->size) {
			DISK_READ(buf, 1, fd->tfd->block[fd->fp / ft->block_size]);
			memcpy(buffer + to_buffer, buf + from_buf, sz);
			to_buffer += sz;
			from_buf = 0;
			todo -= sz;
			fd->fp += sz;
			sz = MIN(ft->block_size, todo);
		}

		timespec_t t;
		kclock_gettime (CLOCK_REALTIME, &t);
		fd->tfd->ta = t;

		return to_buffer;
	}
	else {
		size_t to_buf = fd->fp % ft->block_size;
		size_t from_buffer = 0;
		size_t sz = MIN(ft->block_size - to_buf, todo);

		while (todo > 0) {
			size_t nb = fd->fp / ft->block_size;
			if (nb >= fd->tfd->blocks) {
				for (int i = ft_size; i < ft->blocks; i++) {
					if (ft->free[i]) {
						ft->free[i] = 0;
						fd->tfd->block[nb] = i;
						fd->tfd->blocks++;
						break;
					}
				}
				if (nb >= fd->tfd->blocks)
					break;
			}
			DISK_READ(buf, 1, fd->tfd->block[nb]);
			memcpy(buf + to_buf, buffer + from_buffer, sz);
			DISK_WRITE(buf, 1, fd->tfd->block[nb]);
			to_buf = 0;
			from_buffer += sz;
			todo -= sz;
			fd->fp += sz;
			sz = MIN(ft->block_size, todo);
		}

		fd->tfd->size = MAX(fd->tfd->size, fd->fp);

		timespec_t t;
		kclock_gettime (CLOCK_REALTIME, &t);
		fd->tfd->tm = t;

		DISK_WRITE(ft, ft_size, 0);

		return size - todo;
	}

	return 0;
}
