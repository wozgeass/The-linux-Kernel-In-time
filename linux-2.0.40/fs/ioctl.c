/*
 *  linux/fs/ioctl.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

#include <asm/segment.h>

#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/file.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/stat.h>
#include <linux/termios.h>
#include <linux/fcntl.h> /* for f_flags values */

static int file_ioctl(struct file *filp,unsigned int cmd,unsigned long arg)
{
	int error;
	int block;

	switch (cmd) {
		case FIBMAP:
			if (filp->f_inode->i_op == NULL)
				return -EBADF;
		    	if (filp->f_inode->i_op->bmap == NULL)
				return -EINVAL;
			error = verify_area(VERIFY_WRITE,(void *) arg,4);
			if (error)
				return error;
			block = get_fs_long((long *) arg);
			block = filp->f_inode->i_op->bmap(filp->f_inode,block);
			put_fs_long(block,(long *) arg);
			return 0;
		case FIGETBSZ:
			if (filp->f_inode->i_sb == NULL)
				return -EBADF;
			error = verify_area(VERIFY_WRITE,(void *) arg,4);
			if (error)
				return error;
			put_fs_long(filp->f_inode->i_sb->s_blocksize,
			    (long *) arg);
			return 0;
		case FIONREAD:
			error = verify_area(VERIFY_WRITE,(void *) arg,sizeof(int));
			if (error)
				return error;
			put_fs_long(filp->f_inode->i_size - filp->f_pos,
			    (int *) arg);
			return 0;
	}
	if (filp->f_op && filp->f_op->ioctl)
		return filp->f_op->ioctl(filp->f_inode, filp, cmd, arg);
	return -ENOTTY;
}


asmlinkage int sys_ioctl(unsigned int fd, unsigned int cmd, unsigned long arg)
{	
	struct file * filp;
	int on;
	int retval = 0;

	filp = fget(fd);
	
	if(filp==NULL)
		return -EBADF;
		
	switch (cmd) {
		case FIOCLEX:
			FD_SET(fd, &current->files->close_on_exec);
			break;

		case FIONCLEX:
			FD_CLR(fd, &current->files->close_on_exec);
			break;

		case FIONBIO:
			retval = verify_area(VERIFY_READ, (unsigned int *)arg,
				sizeof(unsigned int));
			if(!retval)	
			{
				on = get_user((unsigned int *) arg);
				if (on)
					filp->f_flags |= O_NONBLOCK;
				else
					filp->f_flags &= ~O_NONBLOCK;
			}
			break;

		case FIOASYNC: /* O_SYNC is not yet implemented,
				  but it's here for completeness. */
			retval = verify_area(VERIFY_READ, (unsigned int *)arg,
				sizeof(unsigned int));
			if(!retval)	
			{
				on = get_user ((unsigned int *) arg);
				if (on)
					filp->f_flags |= O_SYNC;
				else
					filp->f_flags &= ~O_SYNC;
			}
			break;

		default:
			if (filp->f_inode && S_ISREG(filp->f_inode->i_mode))
				retval = file_ioctl(filp, cmd, arg);
			else if (filp->f_op && filp->f_op->ioctl)
				retval = filp->f_op->ioctl(filp->f_inode, filp, cmd, arg);
			else 
				retval = -ENOTTY;
	}
	fput(filp, filp->f_inode);
	return retval;
}
