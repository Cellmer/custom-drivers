#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>

#define MYMAJOR 90

// meta data
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Michal Cellmer");
MODULE_DESCRIPTION("Register device number and create simple callback functions");

// function called when the device file is opened
static int driver_open(struct inode *device_file, struct file *instance)
{
    printk("device_number - open was called\n");
    return 0;
}

// function called when the device file is closed
static int driver_close(struct inode *device_file, struct file *instance)
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
    int retval;
    printk("Hello Kernel!\n");

    // register device number
    retval = register_chrdev(MYMAJOR, "my_dev_nr", &fops);
    if(retval == 0)
    {
        printk("device_numer - registered device number Major: %d, Minor: %d\n", MYMAJOR, 0);
    }
    else if(retval > 0)
    {
        printk("device_number - registered device number Major: %d, Minor: %d\n", retval>>20, retval&0xfffff);
    }
    else
    {
        printk("Could not register device number!\n");
        return -1;
    }
    return 0;
}

// function called when the module is removed from the kernel
static void __exit driver_exit(void)
{
    unregister_chrdev(MYMAJOR, "my_dev_nr");
    printk(KERN_ALERT "Goodbye Kernel!\n");
}

module_init(driver_init);
module_exit(driver_exit);