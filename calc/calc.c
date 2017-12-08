#include "calc.h"

#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/io.h>

MODULE_AUTHOR("Quaxton");
MODULE_LICENSE("GPL");
static int major;

static struct class* calcClass = NULL;
static struct device* calcDevice = NULL;

static long sample_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
static int  sample_open(struct inode *inode, struct file *filp);
static int  sample_close(struct inode *inode, struct file *filp);


//designated initializers
//defined in linux/fs.h
static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = sample_ioctl,
	.open = sample_open,
	.release = sample_close
};

int calc_init(void) {
	major = register_chrdev(MAJOR_DYNAMIC, DRIVER_NAME, &fops);
	printk("CALC_INIT.\n");
	if(major < 0) {
		printk(KERN_ERR "%s: Could not assign major number.\n", DRIVER_NAME);
		return major;
	}

	//Register the device class
	calcClass = class_create(THIS_MODULE, "calc");
	if(IS_ERR(calcClass)) {
		unregister_chrdev(major, DRIVER_NAME);
		printk(KERN_ERR "Failed to register device class\n");
		return PTR_ERR(calcClass);
	}

	//Register the device driver
	calcDevice = device_create(calcClass, NULL, MKDEV(major, 0), NULL, DRIVER_NAME);
	if (IS_ERR(calcDevice)){               // Clean up if there is an error
	      class_destroy(calcClass);           // Repeated code but the alternative is goto statements
	      unregister_chrdev(major, DRIVER_NAME);
	      printk(KERN_ALERT "Failed to create the device\n");
	      return PTR_ERR(calcDevice);
	}
	return 0;
}

void calc_exit(void) {
	device_destroy(calcClass, MKDEV(major, 0));
	class_unregister(calcClass);
	class_destroy(calcClass);
	unregister_chrdev(major, DRIVER_NAME);
	printk(KERN_ALERT "Exiting calc driver.\n");
}


static long sample_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
	switch(cmd) {
		case SAMPLE_IOCTL_ADD:
			{
				struct calc addition;
				memset(&addition, 0, sizeof(struct calc));

				if(copy_from_user(&addition, (struct calc*)arg, sizeof(struct calc)))
					return -EFAULT;
				
				addition.result = addition.num1 + addition.num2;
				if(copy_to_user((struct calc*)arg, &addition, sizeof(struct calc)))
					return -EFAULT;
				printk("Kernel Added.\n");
				return 0;
			}

		default:
			return -EFAULT;
	}
}

static int sample_open(struct inode *inode, struct file *filp) {
	printk("sample_open called.\n");
	return 0;
}

static int sample_close(struct inode *inode, struct file *filp) {
	printk("sample_close called.\n");
	return 0;
}


module_init(calc_init);
module_exit(calc_exit);
