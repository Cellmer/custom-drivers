#include <linux/init.h>
#include <linux/module.h>

// meta data
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Michal Cellmer");
MODULE_DESCRIPTION("Simple hello world module");

// function called when the module is loaded into the kernel
static int __init hello_init(void)
{
    printk(KERN_ALERT "Hello world!\n");
    return 0;
}

// function called when the module is removed from the kernel
static void __exit hello_exit(void)
{
    printk(KERN_ALERT "Goodbye, cruel world\n");
}

module_init(hello_init);
module_exit(hello_exit);