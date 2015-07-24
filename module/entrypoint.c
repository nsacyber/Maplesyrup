#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>

#include "ms_debug.h"
#include "cmd_handler.h"

static
long
ms_ioctl(
    struct file *file,
    unsigned int cmd,
    unsigned long arg)
{
    TRACE("IOCTL received: cmd: 0x%x\n", cmd);
    switch(cmd)
    {
        case MS_IOCTL_CO:       
            call_cmd_co((void *)arg);
            break;
        default:
            break;
    }
    return 0;
}

static
int
ms_open(
    struct inode *inode,
    struct file *file)
{
    int result = -1;
    TRACE("Open called\n", 0);
    
    /* Register undef_instr hook */     
    if (ms_add_undef_instr_hook() != 0)
    {
        TRACE("Failed to add_undef_instr_hook: 0x%x\n", result);
        goto done;
    }
    TRACE("Added undefined instruction hook\n", 0);
    result = 0;
done:
    return result;
}

static
int
ms_release(
    struct inode *inode,
    struct file *file)
{
    TRACE("Release called\n", 0);
    ms_del_undef_instr_hook();
    return 0;
}


static const struct file_operations ms_ops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = ms_ioctl,
    .open = ms_open,
    .release = ms_release,
    .llseek = no_llseek,
};

static struct miscdevice ms_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "maplesyrup",
    .fops = &ms_ops,
    .mode = 0666,
};

static
int
__init maplesyrup_init(void)
{
    int result = -1;
    
    /* Find undef_hooks */  
    if (ms_find_undef_hook() != 0)
    {
        TRACE("Failed to find_undef_hook\n", 0);
        goto done;
    }
        
    /* Register misc device */      
    if (misc_register(&ms_device) != 0)
    {
        TRACE("Failed to misc_register\n", 0);
        goto done;
    }
    printk("Registered Maplesyrup\n");  

    result = 0;
done:
    return result;
}

static
void
__exit maplesyrup_exit(void)
{
    printk("Unregistering Maplesyrup\n");
    misc_deregister(&ms_device);    
    return;
}


module_init(maplesyrup_init);
module_exit(maplesyrup_exit);

MODULE_AUTHOR("Maplesyrup");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Maplesyrup Register Display Tool");
MODULE_VERSION("1.0");
