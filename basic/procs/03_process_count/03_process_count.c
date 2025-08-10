#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>

MODULE_DESCRIPTION("/proc example kernel #03: linux driver that displays the number of processes.");
MODULE_AUTHOR("runes");
MODULE_VERSION("0.1");
MODULE_LICENSE("GPL");

#define PROC_NAME "03_process_count"

static int process_count_show(struct seq_file *m, void *v) {
	unsigned int num = 0;
	struct task_struct* task;

	for_each_process(task) {
		num++;
	}

	seq_printf(m, "Number of Processes: %u\n", num);
	return 0;
}

static const struct proc_ops process_count_fops {
	.proc_show     = single_open,
	.proc_read     = seq_read,
	.proc_lseek    = seq_lseek,
	.proc_release  = single_release,
};

static int __init process_count_module_init(void) {
	if (!PROC_CREATE(PROC_NAME, 0, NULL, &process_count_fops)) {
		pr_err("Failed to create /proc/%s.\n", PROC_NAME);
		return -ENOMEM;
	}
	pr_info("Kernel Module Loaded: /proc/%s.\n", PROC_NAME);
	return 0;
}

static void __exit process_count_module_exit(void) {
	remove_proc_entry(PROC_NAME, NULL);
	pr_info("Kernel Module Unloaded: /proc/%s removed.\n", PROC_NAME);
}

module_init(process_count_module_init);
module_exit(process_count_module_exit);

