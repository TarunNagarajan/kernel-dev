#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("runes");
MODULE_DESCRIPTION("process-mgmt: list processes using task_struct");
MODULE_VERSION("1.0");
static int __init process_list_init(void) {
	struct task_struct* task;
	printk(KERN_INFO "Process List Module: Loading...\n");

	for_each_process(task) {
		printk(KERN_INFO "PID: %d | Command: %s\n", task->pid, task->comm);
	}

	return 0;
}

static void __exit process_list_exit(void) {
	printk(KERN_INFO "Process List Module: Unloading...\n");
}

module_init(process_list_init);
module_exit(process_list_exit);
