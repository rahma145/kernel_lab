#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/errno.h>

#define PROC_NAME "Kernel_Lab"
static int counter = 0;

static ssize_t my_read(struct file *file, char __user *buffer, size_t count, loff_t *offset);

static const struct proc_ops my_fops = {
    .proc_read = my_read,
};


static struct proc_dir_entry *entry = NULL; 


/* fonction d'initialisation */
static int __init kernel_lab_init(void)
{
    entry = proc_create(PROC_NAME, 0444, NULL, &my_fops);
    if (!entry) {
        printk(KERN_ERR "Erreur lors de la création de l'entrée /proc/%s\n", PROC_NAME);
        return -ENOMEM;
    }
    return 0;
}

/* fonction de sortie */
static void __exit kernel_lab_exit(void)
{
    proc_remove(entry); 
    printk(KERN_INFO "l'entrée %s est supprimé \n ", PROC_NAME); 
}

static ssize_t my_read(struct file *file, char __user *buffer, size_t count, loff_t *offset)
{
    if (count == 0) return 0; 
    const char kernel_data[] = "Hello from Kernel Space!\n";

    size_t data_len = sizeof(kernel_data) - 1;
    size_t remaining;
    size_t data_to_copy;
    unsigned long not_copied;
    size_t copied;

    if (*offset >= data_len ) return 0; 
    remaining = data_len - *offset; 
    data_to_copy = min(remaining, count); 

    
    not_copied = copy_to_user(buffer, kernel_data + *offset, data_to_copy); 
    copied = data_to_copy - not_copied; 
    *offset += copied; 
    if (copied == 0) return -EFAULT; 
    return copied; 
}

/* déclaration des fonctions d'entrée et de sortie */
module_init(kernel_lab_init);
module_exit(kernel_lab_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rahma");
MODULE_DESCRIPTION("First Linux kernel module");