/*
 * This file contains the procedures for the handling of select and poll
 *
 * Created for Linux based loosely upon Mathius Lattner's minix
 * patches by Peter MacDonald. Heavily edited by Linus.
 *
 *  4 February 1994
 *     COFF/ELF binary emulation. If the process has the STICKY_TIMEOUTS
 *     flag set in its personality we do *not* modify the given timeout
 *     parameter to reflect time remaining.
 *
 *  24 January 2000
 *     Changed sys_poll()/do_poll() to use PAGE_SIZE chunk-based allocation
 *     of fds to overcome nfds < 16390 descriptors limit (Tigran Aivazian).
 */

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/syscalls.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/poll.h>
#include <linux/personality.h>
#include <linux/file.h>
#include <linux/fdtable.h>
#include <linux/fs.h>
#include <linux/rcupdate.h>
#include <linux/hrtimer.h>
#include <linux/statfs.h>

#include <asm/uaccess.h>
#include "bsp_fs_api.h"

long bsp_sys_creat(const char *pathname, int mode)
{
    return sys_creat(pathname,mode);
}
EXPORT_SYMBOL(bsp_sys_creat);

long bsp_sys_open(const char  *filename,int flags, int mode)
{
    return sys_open(filename,flags,mode);
}
EXPORT_SYMBOL(bsp_sys_open);

long bsp_sys_close(unsigned int fd)
{
    return sys_close(fd);
}
EXPORT_SYMBOL(bsp_sys_close);

long bsp_sys_lseek(unsigned int fd, unsigned int offset,unsigned int origin)
{
    return sys_lseek(fd,offset,origin);
}
EXPORT_SYMBOL(bsp_sys_lseek);

long bsp_sys_read(unsigned int fd, char  *buf, unsigned int count)
{
    return sys_read(fd,buf,count);
}
EXPORT_SYMBOL(bsp_sys_read);

long bsp_sys_write(unsigned int fd, const char  *buf,unsigned int count)
{
    return sys_write(fd,buf,count);
}
EXPORT_SYMBOL(bsp_sys_write);

long bsp_sys_mkdir(const char  *pathname, int mode)
{
    return sys_mkdir(pathname,mode);
}
EXPORT_SYMBOL(bsp_sys_mkdir);

long bsp_sys_rmdir(const char  *pathname)
{
    return sys_rmdir(pathname);
}
EXPORT_SYMBOL(bsp_sys_rmdir);

long bsp_sys_unlink(const char  *pathname)
{
    return sys_unlink(pathname);
}
EXPORT_SYMBOL(bsp_sys_unlink);


long bsp_sys_getdents(unsigned int fd,
          void *dirent,
          unsigned int count)
{
    return sys_getdents(fd,dirent,count);
}
EXPORT_SYMBOL(bsp_sys_getdents);

long bsp_sys_statfs(const char  * path,
          struct statfs  *buf)
{
    return sys_statfs(path,buf);
}
EXPORT_SYMBOL(bsp_sys_statfs);

long bsp_sys_sync(void)
{
    return sys_sync();
}
EXPORT_SYMBOL(bsp_sys_sync);


long bsp_sys_sync_file(void *file)
{
   return sys_fsync(file);
}

EXPORT_SYMBOL(bsp_sys_sync_file);





