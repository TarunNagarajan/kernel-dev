#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>

MODULE_DESCRIPTION("/proc example kernel #1");
MODULE_AUTHOR("runes");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

#define PROC_NAME "01_proc_kernel"

static int hello_show(struct seq_file* m, void* v) {
	// seq_file* m is an opaque handle to the output buffer the kernel is building
	seq_printf(m, "Hello from the kernel via /proc.\n");
	return 0;
}

static int hello_open(struct inode* inode, struct file* file) {
	// inode is a data structure used by the kernel to represent a file in the filesystem

	// file is a pointer to the file structure that tracks the state of this specific file descriptor
	// if the same file was opened multiple times, each instance of that file would have a different -file- structure
	return single_open(file, hello_show, NULL);
	// the third argument, where NULL is, is supposed to be used for custom data to hello_show()
}

// here, fields map to actions which can be performed on the file
static const struct proc_ops hello_fops = {
	.proc_open    = hello_open,
	.proc_read    = seq_read,
	.proc_lseek   = seq_lseek,
	.proc_release = single_release,
};

static int __init proc_init(void) {
	proc_create(PROC_NAME, 0, NULL, &hello_fops);
	// 0 -> file permissions (default)
	// NULL -> parent directory (I want it to be in the proc root dir)
	pr_info("hello_proc: /proc/%s\n", PROC_NAME);
	return 0;
}

static void __exit proc_exit(void) {
	remove_proc_entry(PROC_NAME, NULL);
	pr_info("hello_proc: removed /proc/%s\n", PROC_NAME);
}

module_init(proc_init);
module_exit(proc_exit);






