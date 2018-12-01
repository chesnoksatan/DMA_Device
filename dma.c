#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>
#include <linux/dma-mapping.h>

#include "dma.h"

#define DEVICE_NAME "DMA_TEST"// Имя создаваемого устройства, расположенного в /dev/
#define BUF_LEN 80            // Длина буфера

#define SIZE 1024 * 1024 * 4  // 4Mb на один буфер
#define ARR_SIZE 1//256          // 1Gb на весь буфер

static int Device_Open = 0;
static int MAJOR = 0;

static char msg[BUF_LEN];
static char *msg_Ptr;

static void *kbuf[ARR_SIZE];
static dma_addr_t handle[ARR_SIZE];

static int counter;

static int sin = -1;

// Определение функций, поддерживаемых устройствоом
// Инициализация устройства
static int dma_open(struct inode *, struct file *);
// Деинициализация устройства
static int dma_release(struct inode *, struct file *);
// Получение данных от устройства
static ssize_t dma_read(struct file *, char *, size_t, loff_t *);
// Отправление данных на устройство
static ssize_t dma_write(struct file *, const char *, size_t, loff_t *);

static long dma_ioctl(struct file *, unsigned int,unsigned long);

// Файловые операции поддерживаемые устройством
static struct file_operations fops_dma = {
    .owner = THIS_MODULE,
    .read = dma_read,
    .write = dma_write,
    .unlocked_ioctl = dma_ioctl,
    .open = dma_open,
    .release = dma_release
};

// Функция инициализации модуля
static int __init dma_test(void)
{
    MAJOR = register_chrdev(0, DEVICE_NAME, &fops_dma);
    if (MAJOR < 0)
    {
        printk(KERN_ALERT "EBBChar failed to register a major number\n");
        return MAJOR;
    }

    for ( counter = 0; counter < ARR_SIZE; counter++)
    {
      if ((kbuf[counter] = dma_alloc_coherent( NULL, SIZE, &handle[counter], GFP_KERNEL )) == NULL)
      {
        printk(KERN_ALERT "The allocation failed");
        //return 0;
      } else
    printk(KERN_INFO "DMA in normal state. Handle = %lld", handle[counter]);
    }

    printk(KERN_INFO "");

    return 0;
}

// Функция выгрузки модуля
static void __exit cleanup_dma(void)
{
  for ( counter = 0; counter < ARR_SIZE; counter++)
      dma_free_coherent(NULL, SIZE, kbuf[counter], handle[counter]);

  unregister_chrdev(MAJOR, DEVICE_NAME);
  printk( KERN_INFO "UNLOADING MODULE" );
  printk(KERN_INFO " ");
}

static int dma_open(struct inode *inode, struct file *file)
{

  if (Device_Open)
      return -EBUSY;

  Device_Open++;

  msg_Ptr = msg;
  try_module_get(THIS_MODULE);

  return 0;
}

static int dma_release(struct inode *inode, struct file *file)
{
    Device_Open--;

    module_put(THIS_MODULE);

    return 0;
}

static ssize_t dma_read(struct file *filp, char *buffer, size_t length,
                                                                loff_t * offset)
{
    int bytes_read = 0;


    if (*msg_Ptr == 0)
        return 0;

    while (length && *msg_Ptr) {

        put_user(*(msg_Ptr++), buffer++);

        length--;
        bytes_read++;
    }

    return bytes_read;
}

static ssize_t dma_write(struct file *filp, const char *buff, size_t len,
                                                                   loff_t * off)
{
    int nbytes = len - copy_from_user(msg_Ptr + *off, buff, len);
    *off += nbytes;

    return nbytes;
}

static long dma_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
  int ret = 0;
   switch (cmd) {
     case DMA_WRITE_SIN:
          ret = write_sin_dma((struct gen_sin *)arg);
          break;
     /*case :
          ret = ;*/
    /*
        Для других функций
    */
     default:
          return -EINVAL;
   }

   return ret;
}

int write_sin_dma ( struct gen_sin *users_sin){

  for ( counter = 0; counter < users_sin->byte; counter++){
    kbuf[counter] = users_sin->A * sinus();
    //double A, w, t
  }

  return 0;
}

int sinus( int sinu ){
    switch (sin) {
      case -1: {
        sin += 1;
        return -1;
        break;
      }
      case 0: {
        sin += 1;
        return 0;
        break;
      }
      case 1: {
        sin = -1;
        return 1;
        break;
      }
      default:
           return -EINVAL;
    }
    return 0;
}

module_init(dma_test);
module_exit(cleanup_dma);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Chesnokov Evgeny");
MODULE_DESCRIPTION("Linux test DMA module.");
MODULE_VERSION("1.0");
