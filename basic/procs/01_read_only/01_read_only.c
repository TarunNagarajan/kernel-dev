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
}



