#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>
#include <linux/jiffies.h>

MODULE_DESCRIPTION("/proc example kernel #02: linux driver that displays system uptime.");
MODULE_AUTHOR("runes");
MODULE_VERSION("0.1");
MODULE_LICENSE("GPL");

#define PROC_NAME "02_uptime_info"

static int uptime_show(struct seq_file* m, void* v) {
	unsigned long uptime = jiffies / HZ;
	seq_printf(m, "System Uptime: %lu seconds\n", uptime);
	return 0;
}

static int uptime_open(struct inode* inode, struct file* file) {
	return single_open(file, uptime_show, NULL);
}

static const struct proc_ops uptime_fops = {
	.proc_open      = uptime_open,
	.proc_read      = seq_read,
	.proc_lseek     = seq_lseek,
	.proc_release   = single_release,
};

static int __init uptime_module_init(void) {
	proc_create(PROC_NAME, 0, NULL, &uptime_fops);
	pr_info("Uptime Module Loaded: /proc/%s\n", PROC_NAME);
	return 0;
}

static void __exit uptime_module_exit(void) {
	remove_proc_entry(PROC_NAME, NULL);
	pr_info("Uptime Module Removed: /proc/%s removed. \n", PROC_NAME);
}

module_init(uptime_module_init);
module_exit(uptime_module_exit);
