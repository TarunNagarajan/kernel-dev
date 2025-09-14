#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("sysfs: level hierarchy");
MODULE_AUTHOR("runes");
MODULE_VERSION("1.0");

static struct kobject* parent_kobj;
static struct kobject* child_kobj;

static int __init level_init(void) {

	// create parent kobject under /sys/kernel/
	pr_info("Level 01: Initializing Hierarchy.\n");
	parent_kobj = kobject_create_and_add("parent_kobj", kernel_kobj);
	if (!parent_kobj) return -ENOMEM;

	child_kobj = kobject_create_and_add("child_kobj", parent_kobj);
	if (!child_kobj) {
		kobject_put(parent_kobj);
		return -ENOMEM;
	}

	pr_info("Created /sys/kernel/parent_kobj/child_kobj\n");
	return 0;
}

static void __exit level_exit(void) {
	pr_info("Level 01 Module: Exiting Hierarchy.\n");
	kobject_put(child_kobj);
	kobject_put(parent_kobj);
}

module_init(level_init);
module_exit(level_exit);


