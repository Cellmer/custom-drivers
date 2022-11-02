#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

#define MYMAJOR 90
#define BUFFER_SIZE 255

// meta data
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Michal Cellmer");
MODULE_DESCRIPTION("Register device number and create simple callback functions");

// buffer for data
static char buffer[BUFFER_SIZE];
static int buffer_pointer;

// function called when user want to read from the device file
static ssize_t driver_read(struct file *File, char *user_buffer, size_t count, loff_t *offs)
{
    int to_copy, not_copied, delta;

    to_copy = min(count, buffer_pointer);

    // copy data to user
    not_copied = copy_to_user(user_buffer, buffer, to_copy);

    delta = to_copy - not_copied;

    return delta;
}

// function called when user want to write to the device file
static ssize_t driver_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs)
{
    int to_copy, not_copied, delta;

    to_copy = min(count, sizeof(buffer));

    // copy data from user
    not_copied = copy_from_user(buffer, user_buffer, to_copy);
    buffer_pointer = to_copy;

    delta = to_copy - not_copied;

    return delta;
}

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
    .release = driver_close,
    .read = driver_read,
    .write = driver_write
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
        printk("device_number - registered device number Major: %d, Minor: %d\n", MYMAJOR, 0);
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