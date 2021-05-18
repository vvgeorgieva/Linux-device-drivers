#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");

#define MODNAME		TMP100
#define TMP100_I2C_ADDRESS	0x18

static int major;

int chardev_init(void);
void chardev_exit(void);
static int chardev_open(struct inode *inode, struct file *file);
static int chardev_release(struct inode *inode, struct file *file);
static ssize_t chardev_read(struct file *file, char __user *buf, size_t count, loff_t *offset);
static ssize_t chardev_write(struct file *file, const char __user *buf, size_t count, loff_t *offset);

static const struct file_operations chardev_fops = {
	.owner = THIS_MODULE,
	.open = chardev_open,
	.release = chardev_release,
	.read = chardev_read,
	.write = chardev_write
}

static int __init chardev_init(void) {
	major = register_chrdev(0, "chardev", &chardev_fops);
	
	return 0;
}

static void __exit chardev_exit(void) {
	//TODO
	
	return;
}

module_init(chardev_init);
module_exit(chardev_exit);
