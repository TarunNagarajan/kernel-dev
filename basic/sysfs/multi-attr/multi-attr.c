#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("runes");
MODULE_DESCRIPTION("sysfs: example with foo, bar, baz");
MODULE_VERSION("1.0");

static struct kobject* kobj; // pointer that will hold a reference to the kernel object

static int foo = 1;
static int bar = 1000;
static char baz[32] = "hello world";

static ssize_t foo_show(struct kobject* kobj, struct kobj_attribute* attr, char* buf) {
	return sprintf(buf, "%d\n", foo);
}

// function called when userspace process writes to the sysfs file
static ssize_t foo_store(struct kobject* kobj, struct kobj_attribute* attr, const char* buf, size_t count) {
	int tmp;
	if (sscanf(buf, "%d", &tmp) == 1) {
		foo = tmp;
	}
	return count;
}

static ssize_t bar_show(struct kobject* kobj, struct kobj_attribute* attr, char* buf) {
	return sprintf(buf, "%d\n", bar);
}

static ssize_t bar_store(struct kobject* kobj, struct kobj_attribute* attr, const char* buf, size_t count) {
	int tmp;
	if (sscanf(buf, "%d", &tmp) == 1) {
		bar = tmp;
	}
	return count;
}

static ssize_t baz_show(struct kobject* kobj, struct kobj_attribute* attr, char* buf) {
	return sprintf(buf, "%s\n", baz);
}

static ssize_t baz_store(struct kobject* kobj, struct kobj_attribute* attr, const char* buf, size_t count) {
	size_t len = min(count, sizeof(baz) - 1);
	strncpy(baz, buf, len);
	baz[len] = '\0';

	// remove trailing newline if present
	if (baz[len - 1] == '\n') baz[len - 1] = '\0';

	return count;
}

// attribute definition
static struct kobj_attribute foo_attr = __ATTR(foo, 0644, foo_show, foo_store);
static struct kobj_attribute bar_attr = __ATTR(bar, 0644, bar_show, bar_store);
static struct kobj_attribute baz_attr = __ATTR(baz, 0644, baz_show, baz_store);

static struct attribute *attrs[] = {
	&foo_attr.attr,
	&bar_attr.attr,
	&baz_attr.attr,
	NULL,
};

static struct attribute_group attr_group = {
	.attrs = attrs,
};

static int __init multi_init(void) {
	int retval;
	kobj = kobject_create_and_add("multi", kernel_kobj);

	if (!kobj) return -ENOMEM;

	retval = sysfs_create_group(kobj, &attr_group);
	if (retval) kobject_put(kobj); // reduce reference count

	printk(KERN_INFO "multi-attr: sysfs entries created.\n");
	return 0;
}

static void __exit multi_exit(void) {
	kobject_put(kobj);
	printk(KERN_INFO "multi-attr: sysfs entries removed.\n");
}

module_init(multi_init);
module_exit(multi_exit);
