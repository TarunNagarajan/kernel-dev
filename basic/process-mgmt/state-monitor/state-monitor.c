#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("runes");
MODULE_DESCRIPTION("process-mgmt: simple process state monitor");
MODULE_VERSION("1.0");

static struct proc_dir_entry* proc_entry;

static const char* get_task_state_name(struct task_struct* task) {
	if (task->exit_state == EXIT_ZOMBIE) return "Zombie";
	if (task->exit_state == EXIT_DEAD)   return "Dead";

	switch(task->state) {
		case TASK_RUNNING:
			return "Running";
		case TASK_INTERRUPTIBLE:
			return "Sleeping";
		case TASK_UNINTERRUPTIBLE:
			return "Disk Sleep";
		case __TASK_STOPPED:
			return "Stopped";
		case __TASK_TRACED:
			return "Traced";
		default:
			return "Unknown";
	}
}

static int proc_show(struct seq_file* m, void* v) {
	struct task_struct* task;

	seq_printf(m, "PID\tCOMM\t\tSTATE\t\tPPID\t\tPRIO\n");
	seq_printf(m, "---\t---------------\t\tSTATE\t\tPPID\t\tPRIO\n");

	rcu_read_lock();

	for_each_process(task) {
		seq_printf(m, "%d\t%-15s\t%-10s\t%d\t%d\n", task->pid, task->comm, get_task_state_name(task), rcu_dereference(task->real_parent)->pid, task->prio);
	}

	rcu_read_unlock();

	return 0;
}

static int proc_open(struct inode* inode, struct file* file) {
	return single_open(file, proc_show, NULL);
}

static const struct proc_ops proc_fops = {
	.proc_open = proc_open,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = single_release,
};

static int __init monitor_init(void) {
	printk(KERN_INFO "Loading Process State Monitor...\n");
	proc_entry = proc_create("process-monitor", 0444, NULL, &proc_fops);

	if (!proc_entry) {
		printk(KERN_ERR "Failed to create proc entry.\n");
		return -ENOMEM;
	}

	printk(KERN_INFO "Module loaded. Check /proc/process-monitor\n");
	return 0;
}

static void __exit monitor_exit(void) {
	proc_remove(proc_entry);
	printk(KERN_INFO "Unloaded Process State Monitor...\n");
}

module_init(monitor_init);
module_exit(monitor_exit);
