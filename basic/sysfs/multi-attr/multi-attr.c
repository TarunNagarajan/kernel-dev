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
