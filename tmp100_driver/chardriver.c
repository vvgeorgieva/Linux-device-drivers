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

/* Documentation defined max resolution */
#define RESOLUTION		16 /* Max resolution */

/* BUFF_SIZE set to 2 => char temp[2] = 16 bits + 1 for sign + 1 for '\0' */
#define BUFF_SIZE	4

struct chardev_data {
	dev_t dev; /* used to hold device numbers—both the major and minor parts */
	struct cdev chardev; /*  kernel's internal structure that represents char devices */
	struct regmap *regmap; /* use regmap API to factorize and unify the way to access SPI/I2C devices*/
	struct class *chardev_class;
	struct i2c_client *client;
	u8 resolution;
};

struct chardev_data chardev_data;

static ssize_t chardev_read(struct file *file, char __user *buf, size_t count, loff_t *offset);

static const struct file_operations chardev_fops = {
	.owner = THIS_MODULE,
	.read = chardev_read
};

static int __init chardev_init(void) {

	/* Request dynamic allocation of a device major number */
	if (alloc_chrdev_region(&chardev_data.dev, 0, 1, MODNAME) < 0) {
		printk(KERN_DEBUG "Can't register device\n");
		return -1;
	}
	/* Populate sysfs entries */
	chardev_data.chardev_class = create_class(THIS_MODULE, MODNAME);
	if(chardev_data.chardev_class == NULL) {
		printk(KERN_DEBUG "Can't create struct class\n");
		unregister_chrdev_region(chardev_data.dev, 1);
		return -1;
	}
	/* Connect the file operations with the cdev */
	cdev_init(&chardev_data.chardev, &chardev_fops);
	/* Connect the major/minor number to the cdev */
	if (cdev_add(&chardev_data.chardev, chardev_data.dev, 1) < 0) {
		printk(KERN_DEBUG "Can't add device to the system\n");
		unregister_chrdev_region(chardev_data.dev, 1);
		return -1;
	}
	/* Send uevents to udev, so it'll create /dev nodes */
	if(device_create(chardev_data.chardev_class, NULL, chardev_data.dev, NULL, MODNAME)
			== NULL) {
		printk("Can't create device\n");
		class_destroy(chardev_data.chardev_class);
		unregister_chrdev_region(chardev_data.dev, 1);
		return -1;
	}

	printk(KERN_DEBUG "Temperature Driver initialized\n")

	return 0;
}

static void __exit chardev_exit(void) {
	/* Remove the cdev */
	cdev_del(&chardev_data.chardev);
	/* Release the major number */
	unregister_chrdev_region(chardev_data.dev, 1);
	/* Release I/O region */
	device_destroy(chardev_data.chardev_class, chardev_data.dev);
	 /* Destroy cmos_class */
	class_destroy(chardev_data.chardev_class);

	printk(KERN_DEBUG "Temperature Driver removed\n")
	return;
}

/* Documentation defined - 12 bit resolution, but two bytes should be initially read where 4 MSB are set to 0
 * Using this function in order to escape floating point and convert C to MC which is why is * by 1000, 12 bit resolution also equals 0.0625C=>1/16=> << 4
 */
static s16 convert_to_mc(unsigned int regval, int resolution) {
	return ((regval >> (16 - resolution)) * 1000) >> (resolution - 8);
}

/* Two bytes mustbe read to obtain data - first 12 bits contain data the other are null */
static ssize_t chardev_read(struct file *file, char __user *buf, size_t count, loff_t *offset) {
	unsigned int regval; /* regmap read requires unsigned int for regval and reg */
	s16 temp_mc;
	int err, count;
	char temp[BUFF_SIZE] = {0};

	err = regmap_read(tmp100_data.regmap, TMP100_READ_REG, &regval);
	if (err < 0)
		return err;

	temp_mc = convert_to_mc(regval, RESOLUTION);

	return 0;
}

/* reg_bits: Number of digits in register address, mandatory must be configured
 * val_bits: Number of registry values, mandatory must be configured
 * max_register: This is optional, which specifies the maximum valid register address.
 */

static const struct regmap_config tmp100_regmap_config = {
	.reg_bits = 8,
	.val_bits = 16,
	.max_register = TMP100_HIGH_REGISTER,
};

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
