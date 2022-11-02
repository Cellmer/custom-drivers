#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/types.h>

#define BUFFER_SIZE 255

// meta data
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Michal Cellmer");
MODULE_DESCRIPTION("Register device number and create simple callback functions");

// buffer for data
static char buffer[BUFFER_SIZE];
static int buffer_pointer;

// variables for device and device class
static dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;

#define DRIVER_NAME "read_write_driver"
#define DRIVER_CLASS "MyModuleClass"

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
static int __init driver_initialize(void)
{
    printk("Hello Kernel!\n");

    // allocate a device number
    if(alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME) < 0)
    {
        printk("Device number could not be allocated!\n");
    }
    printk("read_write - device number Major: %d, Minor %d was registered!\n", MAJOR(my_device_nr), MINOR(my_device_nr));

    // create device class
    if((my_class = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL)
    {
        printk("Device class could not be created!\n");
        goto ClassError;
    }

    // create device file
    if(device_create(my_class, NULL, my_device_nr, NULL, DRIVER_NAME) == NULL)
    {
        printk("could not create a device file!\n");
        goto FileError;
    }

    // initialize device file
    cdev_init(&my_device, &fops);

    // register device to kernel
    if(cdev_add(&my_device, my_device_nr, 1) == -1)
    {
        printk("Could not register device!\n");
        goto AddError;
    }

    return 0;

AddError:
    device_destroy(my_class, my_device_nr);
FileError:
    class_destroy(my_class);
ClassError:
    unregister_chrdev(my_device_nr, DRIVER_NAME);

    return -1;
}

// function called when the module is removed from the kernel
static void __exit driver_exit(void)
{
    cdev_del(&my_device);
    device_destroy(my_class, my_device_nr);
    class_destroy(my_class);
    unregister_chrdev(my_device_nr, DRIVER_NAME);
    printk(KERN_ALERT "Goodbye Kernel!\n");
}

module_init(driver_initialize);
module_exit(driver_exit);