/*
	Created by Juan Diego Castro
*/

#include <linux/atomic.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/version.h>

#include <asm/errno.h>

static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char __user *, size_t, loff_t *);
static loff_t device_llseek(struct file *filp, loff_t offset, int whence);

#define SUCCESS 0
#define DEVICE_NAME "chardev"
#define BUF_LEN 80

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

static int major;

enum
{
	CDEV_NOT_USED,
	CDEV_EXCLUSIVE_OPEN
};

static atomic_t already_open = ATOMIC_INIT(CDEV_NOT_USED);

static char msg[BUF_LEN];

static struct class *cls;

static struct file_operations chardev_fops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release,
	.llseek = device_llseek
};

static int __init chardev_init(void)
{
	major = register_chrdev(0, DEVICE_NAME, &chardev_fops);

	if (major < 0)
	{
		pr_alert("Registering char device failed with %d\n", major);
		return major;
	}

	pr_info("New char device was assigned major number %d.\n", major);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 4, 0)
	cls = class_create(DEVICE_NAME);
#else
	cls = class_create(THIS_MODULE, DEVICE_NAME);
#endif
	device_create(cls, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);

	pr_info("Device created on /dev/%s\n", DEVICE_NAME);

	return SUCCESS;
}

static void __exit chardev_exit(void)
{
	device_destroy(cls, MKDEV(major, 0));
	class_destroy(cls);

	unregister_chrdev(major, DEVICE_NAME);
}

static int device_open(struct inode *inode, struct file *file)
{
	if (atomic_cmpxchg(&already_open, CDEV_NOT_USED, CDEV_EXCLUSIVE_OPEN))
		return -EBUSY;

	try_module_get(THIS_MODULE);

	return SUCCESS;
}

static int device_release(struct inode *inode, struct file *file)
{
	atomic_set(&already_open, CDEV_NOT_USED);

	module_put(THIS_MODULE);

	return SUCCESS;
}

static ssize_t device_read(struct file *filp, char __user *buffer, size_t length, loff_t *offset)
{
	size_t pos = 0;
	int bytes_read = 0;

	if (*offset > BUF_LEN || length > BUF_LEN)
	{
		offset = 0;
		pr_err("%s: [read] Read exceeds buffer capacity.\n", DEVICE_NAME);
		return -EINVAL;
	}

	while (length > 0)
	{
		pos = *offset % BUF_LEN; // circular buffer behavior

		if (*(msg + pos) == 0)
			break;

		if (put_user(*(msg + pos), buffer++))
		{
			pr_err("%s: [read] Failed to copy data to user buffer.\n", DEVICE_NAME);
			return -EFAULT;
		}

		(*offset)++;
		length--;
		bytes_read++;
	}

	pr_info("%s: [read] Successfully read %d bytes.\n", DEVICE_NAME, bytes_read);

	return bytes_read;
}

static ssize_t device_write(struct file *filp, const char __user *buffer, size_t length, loff_t *offset)
{
	int bytes_written = 0;

	if (*offset > BUF_LEN || length > BUF_LEN)
	{
		offset = 0;
		pr_err("%s: [write] Write exceeds buffer capacity.\n", DEVICE_NAME);
		return -EINVAL;
	}

	while (length > 0)
	{
		size_t pos = *offset % BUF_LEN; // circular buffer behavior

		if (get_user(*(msg + pos), buffer++))
		{
			pr_err("%s: [write] Failed to copy data from user buffer.\n", DEVICE_NAME);
			return -EFAULT;
		}

		(*offset)++;
		length--;
		bytes_written++;
	}

	pr_info("%s: [write] Successfully wrote %d bytes.\n", DEVICE_NAME, bytes_written);

	return bytes_written;
}

static loff_t device_llseek(struct file *filp, loff_t offset, int whence)
{
	switch (whence)
	{
		case SEEK_SET:
			if ((offset < 0) || (offset > BUF_LEN))
				return -EINVAL;
			filp->f_pos = offset;
			break;

		case SEEK_CUR:
			if ((filp->f_pos + offset) < 0 || (filp->f_pos + offset) > BUF_LEN)
				return -EINVAL;
			filp->f_pos += offset;
			break;

		case SEEK_END:
			if ((BUF_LEN + offset) < 0 || (BUF_LEN + offset) > BUF_LEN)
				return -EINVAL;
			filp->f_pos = BUF_LEN + offset;
			break;

		default:
			pr_err("%s: [lseek] Invalid 'whence' value\n", DEVICE_NAME);
			return -EINVAL;
	}

	pr_info("%s: [lseek] Buffer offset successfully set to %lld\n", DEVICE_NAME, filp->f_pos);
	return filp->f_pos;
}

module_init(chardev_init);
module_exit(chardev_exit);

MODULE_LICENSE("GPL"); 
