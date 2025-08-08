#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("lkm that demonstrates dynamic memory allocation.");
MODULE_AUTHOR("runes");
MODULE_VERSION("0.1");

struct driver_data {
	u8 version;
	char text[64];
};

u32* ptr1;
struct driver_data* ptr2;

static int __init malloc_init(void) {
	ptr1 = kmalloc(sizeof(u32), GFP_KERNEL);
	if (ptr1 == NULL) {
		printk("dyna_malloc out of memory.\n");
		return -1;
	}

	printk("dyna_malloc - *ptr1: 0x%x\n", *ptr1);
	*ptr1 = 0xc001c0de;
	printk("dyna_malloc - *ptr1: 0x%x\n", *ptr1);
	kfree(ptr1);

	ptr2 = kzalloc(sizeof(struct driver_data), GFP_KERNEL);
	if (ptr2 == NULL) {
		printk("dyna_malloc out of memory.\n");
		return -1;
	}

	ptr2->version = 230;
	strcpy(ptr2->text, "hi from the linux kernel module.");

	printk("dyna_malloc - ptr2->version: %d\n", ptr2->version);
	printk("dyna_malloc - ptr2->test: %s\n", ptr2->text);

	return 0;
}

static void __exit malloc_exit(void) {
	printk("dyna_malloc - ptr2->version: %d\n", ptr2->version);
	printk("dyna_malloc - ptr2->text: %s\n", ptr2->text);
	kfree(ptr2);
}

module_init(malloc_init);
module_exit(malloc_exit);
