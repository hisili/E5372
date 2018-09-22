
#if (FEATURE_UPDATEONLINE == FEATURE_ON)

#include "os_platform.h"

#ifdef WIN32
   
FILE *  os_file_open(const char __user *filename)
{
   FILE *stream =NULL;
   if( (stream  = fopen( filename, "rb" )) == NULL )
      {
        printf( "os_file_open  fopen error\n" );
      }
     else
      {
          printf( "os_file_open fopen  ok\n" );
      } 

     return stream;
}

FILE * get_current_path(void)
{
    FILE* fd =NULL;
    if(NULL == (fd = fopen("www.bin","w")))
      {
            printf("fopen error\n");
      }

    fclose(fd);
    
    return fd;    
}

int os_file_lseek(FILE* fd,long offset)
{
       /*将指针指向指定的文件位置*/
    if( OSAL_ERROR == fseek(fd, offset, SEEK_SET))
    {
        printf("os_file_lseek: fseek  error!\n");
        return OSAL_ERROR;
    } 

    return OSAL_OK;
}

long os_file_read(FILE * fd, void *buf, size_t count)
{
  int ulRet =0;
  if (count != (ulRet = fread(buf,sizeof(char),count,fd)))
    {
        printf("os_file_read:fread fail ret[%d]!\n",ulRet);
        return OSAL_ERROR;
    }
  
  return  ulRet;
}

int os_file_close(FILE * fd)
{
   /*关闭文件*/
    if (OSAL_OK!= fclose(fd))
    {
        printf("update_UpDateInfoGet:fclose fail!\n");;
        return OSAL_ERROR;
    }

   return OSAL_OK;
}

UINT32 os_file_size(FILE * fd)
{
    UINT32 ret =0;
    /*将指针指向文件末尾*/
    if( OSAL_ERROR == fseek(fd, 0, SEEK_END) )
    {
        
        printf("os_file_size: fseek error!\n");
        
        return OSAL_ERROR;
    }
    
    /*获取该文件长度*/
    if( OSAL_ERROR == (ret = ftell(fd)))
    {

        printf("os_file_size: ftell error!\n");
        
        return OSAL_ERROR;
    }

    return ret;
}

FILE * os_filp_open(const char *filename)
{
    return os_file_open(filename);
}

long os_vfs_read(struct file *file, char __user *buf, size_t count)
{
    return os_file_read(file,buf,count);
}

void os_filp_close(FILE * fd)
{
    os_file_close(fd);
}


#else//板载

#include <linux/kernel.h>
#include <linux/syscalls.h>

int  os_file_open(const char *filename)
{
  int fd = 0;
  if((fd = sys_open(filename, O_RDONLY, S_IRUSR))<0)
     {
        printk( KERN_INFO "os_file_open:Open file error!\n");
        return OSAL_ERROR;
     }

  printk( KERN_DEBUG "os_file_open:Open file OK!\n");
  return fd;
}

int os_file_lseek(unsigned int fd,long offset)
{
    /*将指针指向指定的文件位置*/
    if( OSAL_ERROR == sys_lseek(fd, offset, SEEK_SET) )
    {
        printk(KERN_INFO "os_file_lseek:  error!\n");
        return OSAL_ERROR;
    }

    return OSAL_OK;
}


long os_file_read(unsigned int fd, char *buf, int count)
{
  if (0 > sys_read(fd,buf, count))
    {
        printk(KERN_INFO  "os_file_read: fail!\n");
        return OSAL_ERROR;
     }

  return OSAL_OK;
}

long os_file_write(unsigned int fd, char *buf, int count)
{
    if (0 > sys_write(fd,buf, count))
    {
        printk(KERN_INFO  "os_file_write: fail!\n");
        return OSAL_ERROR;
    }

    return OSAL_OK;
}

int os_file_close(unsigned int fd)
{
   /*关闭文件*/
   if (OSAL_OK!= sys_close(fd))
    {
        printk(KERN_INFO "update_UpDateInfoGet:sys_close fail!\n");
        return OSAL_ERROR;
    }

   return OSAL_OK;
}

unsigned int  os_file_size(unsigned int fd)
{
    int32_t   file_len;
    file_len = sys_lseek(fd, 0, SEEK_END);
    if( 0 > file_len )
    {
        sys_close(fd);
        printk(KERN_INFO "os_file_lseek: SEEK_END error!\n");
        return OSAL_ERROR;
    }

    return file_len;
}

/**********************************************************/
struct file * os_filp_open(const char *filename)
{
    struct file *fp = NULL; 
    /*判断文件是否存在*/
    fp = filp_open(filename, O_RDONLY, 0644); 
    
   if (IS_ERR(fp)) 
    { 
       printk("open file error:%s\n",filename); 
       return NULL;
    } 

   return fp;
}

//ssize_t vfs_read(struct file *file, char __user *buf, size_t count, loff_t *pos)
long os_vfs_read(struct file *file, char *buf, int count)
{
    loff_t pos =0; 
    ssize_t fsread = vfs_read(file, buf, count, &pos); 
    if (fsread != count)
        {
            filp_close(file, NULL);             
            printk("os_vfs_read  error!\n");
            return OSAL_ERROR;
        }

    return OSAL_OK;
}

void os_filp_close(struct file *filp)
{
    filp_close(filp, NULL); 
}

#endif

#endif

