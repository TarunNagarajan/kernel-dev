#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("runes");
MODULE_DESCRIPTION("sysfs: example");
MODULE_VERSION("1.0");

static struct kobject* kobj; // pointer that will hold a reference to the kernel object
static int val = 0; // in kernel module rn, objective is to expose this to userspace

static ssize_t val_show(struct kobject* kobj, struct kobj_attribute* attr, char* buf) {
	return sprintf(buf, "%d\n", val); // sprintf shows the number of chars written
}

// this function is called when userspace process writes to the sysfs file
static ssize_t val_store(struct kobject* kobj, struct kobj_attribute* attr, const char* buf, size_t count) {
	int tmp;
	if (sscanf(buf, "%d", &tmp) == 1) {
		val = tmp;
	}
	return count;
}

static struct kobj_attribute val_attr = __ATTR(val, 0644, val_show, val_store);

static int __init example_init(void) {
	// create a kobject named "example" and put it under the parent, 'kernel_kobj'
	// kernel_kobj represents /sys/kernel/ directory
	kobj = kobject_create_and_add("example", kernel_kobj);
	if (!kobj) {
		return -ENOMEM;
	}

	// the 'sysfs_create_file' function expects a pointer to the inner 'struct attribute'
	if (sysfs_create_file(kobj, &val_attr.attr)) {
		kobject_put(kobj);
		return -EINVAL;

	}

	// /sys/kernel/example
	printk(KERN_INFO "ex-sysfs: sysfs entry created.\n");
	return 0;
}

static void __exit example_exit(void) {
	kobject_put(kobj);
	printk(KERN_INFO "ex-sysfs: sysfs entry removed.\n");
}

module_init(example_init);
module_exit(example_exit);
