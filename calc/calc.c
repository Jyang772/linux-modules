#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

MODULE_AUTHOR("Quaxton")

//designated initializers
//defined in linux/fs.h
static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = ioctl,
	.open = open,
	.release = close
};
