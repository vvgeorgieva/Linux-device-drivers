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
#include <linux/regmap.h>

#define MODNAME		TMP100

/* TMP100 registers */
#define TMP100_TEMP_REGISTER 	0x00 /* Read only */
#define TMP100_CONF_REGISTER	0x01 /* Read/ Write */
#define TMP100_LOW_REGISTER	0x02 /* Read/ Write */
#define TMP100_HIGH_REGISTER	0x03 /* Read/ Write */

struct chardev_data {
	dev_t devt; /* used to hold device numbers—both the major and minor parts */
	struct cdev chardev; /*  kernel's internal structure that represents char devices */
	struct regmap *regmap; /* use regmap API to factorize and unify the way to access SPI/I2C devices*/
	struct class *chardev_class;
	struct i2c_client *client;
	u8 resolution;
};

static int major = 0;
struct chardev_data chardev_data;

static int chardev_open(struct inode *inode, struct file *file);
static int chardev_release(struct inode *inode, struct file *file);
static ssize_t chardev_read(struct file *file, char __user *buf, size_t count, loff_t *offset);

static const struct file_operations chardev_fops = {
	.owner = THIS_MODULE,
	.open = chardev_open,
	.release = chardev_release,
	.read = chardev_read
};

/* reg_bits: Number of digits in register address, mandatory must be configured
 * val_bits: Number of registry values, mandatory must be configured
 * max_register: This is optional, which specifies the maximum valid register address.
 */

static const struct regmap_config tmp100_regmap_config = {
	.reg_bits = 8,
	.val_bits = 16,
	.max_register = TMP100_HIGH_REGISTER,
};

static int __init chardev_init(void) {
	
	return 0;
}

static void __exit chardev_exit(void) {

	return;
}

static const struct i2c_device_id tmp100_i2c_device_id[] = {
	{	"tmp100", 0},
	{},
};

MODULE_DEVICE_TABLE(i2c, tmp100_i2c_device_id);

/* loading/inserting the driver for a device if not already loaded */
static const struct of_device_id tmp100_of_match[] = {
	{	.compatible = "ti,tmp100", },
	{},
};

/* At compilation time the build process extracts this infomation from all
 * the drivers and prepares a device table. By inserting a device,
 * the device table is referred by the kernel and if an entry is
 * found matching the device/vendor id of the added device, then its module is loaded and initialized.
 */

MODULE_DEVICE_TABLE(of, tmp100_of_match);

module_init(chardev_init);
module_exit(chardev_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Temperature driver using tmp100 sensor");
MODULE_AUTHOR("Vasilena Georgieva");
