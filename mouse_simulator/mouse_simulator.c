#include <linux/module.h>
#include <linux/keyboard.h>
#include <linux/input.h>
 
MODULE_LICENSE("GPL");

struct input_dev *mouse_simulator_input_dev;

void simulate_mouse_events(int keycode)
{
    switch(keycode)
    {
        // LEFT CTRL
        case KEY_LEFTCTRL:
            input_report_key(mouse_simulator_input_dev, BTN_LEFT, 1);
            input_report_key(mouse_simulator_input_dev, BTN_LEFT, 0);
            input_sync(mouse_simulator_input_dev);
            break;
        // LEFT ALT
        case KEY_LEFTALT:
            input_report_key(mouse_simulator_input_dev, BTN_RIGHT, 1);
            input_report_key(mouse_simulator_input_dev, BTN_RIGHT, 0);
            input_sync(mouse_simulator_input_dev);
            break;
        // UP
        case KEY_UP:
            input_report_rel(mouse_simulator_input_dev, REL_Y, -10);
            input_sync(mouse_simulator_input_dev);
            break;
        // LEFT
        case KEY_LEFT:
            input_report_rel(mouse_simulator_input_dev, REL_X, -10);
            input_sync(mouse_simulator_input_dev);
            break;
        // RIGHT
        case KEY_RIGHT:
            input_report_rel(mouse_simulator_input_dev, REL_X, 10);
            input_sync(mouse_simulator_input_dev);
            break;
        // DOWN
        case KEY_DOWN:
            input_report_rel(mouse_simulator_input_dev, REL_Y, 10);
            input_sync(mouse_simulator_input_dev);
            break;

    }
}
 
int keyboard_event_handler(struct notifier_block *nblock, unsigned long code, void *_param)
{
    char keybuf[12] = {0};
    struct keyboard_notifier_param *param = _param;
 
    if (!(param->down)) return NOTIFY_OK;
 
    simulate_mouse_events(param->value);
 
    return NOTIFY_OK;
}
 
static struct notifier_block keysniffer_blk = {
    .notifier_call = keyboard_event_handler,
};
 
static int __init keylogger_init(void)
{
    printk(KERN_INFO "mouse_simulator: initializing...");
    mouse_simulator_input_dev = input_allocate_device();
    if(!mouse_simulator_input_dev)
    {
            printk("Error in input_allocate_device()!\n");
    }

    set_bit(EV_REL, mouse_simulator_input_dev->evbit);
    set_bit(REL_X, mouse_simulator_input_dev->relbit);
    set_bit(REL_Y, mouse_simulator_input_dev->relbit);
    set_bit(EV_KEY, mouse_simulator_input_dev->evbit);
    set_bit(BTN_LEFT, mouse_simulator_input_dev->keybit);
    set_bit(BTN_RIGHT, mouse_simulator_input_dev->keybit);

    if(input_register_device(mouse_simulator_input_dev))
    {
            printk("Error in input_register_device()!\n");
    }
    register_keyboard_notifier(&keysniffer_blk);
    printk(KERN_INFO "mouse_simulator: initialization complete.");
    return 0;
}
 
static void __exit keylogger_exit(void)
{
    unregister_keyboard_notifier(&keysniffer_blk);
    input_unregister_device(mouse_simulator_input_dev);
    printk(KERN_INFO "mouse_simulator: unloaded.");
}
 
module_init(keylogger_init);
module_exit(keylogger_exit);