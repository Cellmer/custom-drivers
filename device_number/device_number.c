#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>

// meta data
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Michal Cellmer");
MODULE_DESCRIPTION("Register device number and create simple callback functions");

// function called when the device file is opened
static int driver_open(struct inode *device_file, struct file *instance)
{
    printk("device_number - open was called\n")
    return 0;
}

// function called when the device file is closed
static int driver_close(struct indoe *device_file, struct file *instance)
{
    printk("device_number - close was called\n");
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = driver_open,
    .release = driver_close
};

// function called when the module is loaded into the kernel
static int __init driver_init(void)
{
    printk(KERN_ALERT "Hello world!\n");
    return 0;
}

// function called when the module is removed from the kernel
static void __exit driver_exit(void)
{
    printk(KERN_ALERT "Goodbye, cruel world\n");
}

module_init(hello_init);
module_exit(hello_exit);