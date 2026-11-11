#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/string.h>

#define PROC_NAME "Kernel_Lab"
#define BUFFER_SIZE 1024
static int counter = 0;

static ssize_t my_read(struct file *file, char __user *buffer, size_t count, loff_t *offset);

static ssize_t my_write(struct file *file, const char __user *user_buf, size_t count, loff_t *ppos); 

static const struct proc_ops my_fops = {
    .proc_read = my_read,
    .proc_write = my_write,
};


static struct proc_dir_entry *entry = NULL; 


/* fonction d'initialisation */
static int __init kernel_lab_init(void)
{
    entry = proc_create(PROC_NAME, 0666, NULL, &my_fops);
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
    char kernel_data[32];

    int data_len;
    data_len = scnprintf(kernel_data, sizeof(kernel_data), "%d\n", counter);
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

static ssize_t my_write(struct file *file, const char __user *user_buf, size_t count, loff_t *ppos)
{
    if (count == 0)
        return 0;

    size_t copied;
    unsigned long not_copied;
    char command[8];

    if (count >= sizeof(command))
        return -EINVAL;

    not_copied = copy_from_user(command, user_buf, count);

    if (not_copied != 0)
        return -EFAULT;

    copied = count;
    command[copied] = '\0';

    if (command[copied - 1] == '\n')
        command[copied - 1] = '\0';

    if (strcmp(command, "inc") == 0)
    {
        counter++;
    }
    else if (strcmp(command, "reset") == 0)
    {
        counter = 0;
    }
    else
    {
        return -EINVAL;
    }

    return count;
}

/* déclaration des fonctions d'entrée et de sortie */
module_init(kernel_lab_init);
module_exit(kernel_lab_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rahma");
MODULE_DESCRIPTION("First Linux kernel module");