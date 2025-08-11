#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/seq_file.h>
#include <linux/sched.h>
#include <linux/proc_fs.h>

#define PROC_NAME "04_process_info"

static int process_info_show(struct seq_file* m, void* v) {
	struct task_struct* task;
	for_each_process(task) {
		seq_printf(m, "PID: %d, Name: %s\n", task->pid, task->comm);
	}
	return 0;
}

static int process_info_open(struct inode* indo, struct file* file) {
	return single_open(file, process_info_show, NULL);
}

const struct proc_ops process_info_ops = {
	.proc_open     = process_info_open,
	.proc_read     = seq_read,
	.proc_lseek    = seq_lseek,
	.proc_release  = single_release,
};

static int __init process_info_module_init(void) {
	if (!proc_create(PROC_NAME, 0, NULL, &process_info_ops)) {
		pr_err("Failed to load module /proc/%s\n", PROC_NAME);
		return -ENOMEM;
	}
	pr_info("Kernel Module Loaded: /proc/%s\n", PROC_NAME);
	return 0;
}

static void __exit process_info_module_exit(void) {
	remove_proc_entry(PROC_NAME, NULL);
	pr_info("Kernel Module Removed: /proc/%s\n", PROC_NAME);
}

module_init(process_info_module_init);
module_exit(process_info_module_exit);

MODULE_DESCRIPTION("/proc example kernel #04: lkm to display process info via struct task_struct* task, task->pid, task->comm.");
MODULE_AUTHOR("runes");
MODULE_VERSION("0.1");
MODULE_LICENSE("GPL");
