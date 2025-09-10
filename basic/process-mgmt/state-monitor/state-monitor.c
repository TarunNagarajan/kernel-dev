#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/version.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("runes");
MODULE_DESCRIPTION("process-mgmt: simple process state monitor");
MODULE_VERSION("1.0");

// make clean
// make
// sudo insmod state-monitor.ko
// cat /proc/process-monitor | head -n 20
// sudo rmmod state-monitor

static struct proc_dir_entry *proc_entry;

static const char *get_task_state_name(struct task_struct *task) {
	char state = task_state_to_char(task);

	switch (state) {
	case 'R':
		return "Running";
	case 'S':
		return "Sleeping";
	case 'D':
		return "Disk Sleep";
	case 'T':
		return "Stopped";
	case 't':
		return "Tracing Stop";
	case 'Z':
		return "Zombie";
	case 'X':
		return "Dead";
	default:
		return "Unknown";
	}
}

static int proc_show(struct seq_file *m, void *v) {
	struct task_struct *task;

	seq_printf(m, "PID\tCOMM\t\t\tSTATE\t\tPPID\tPRIO\n");
	seq_printf(m, "---\t---------------\t----------\t----\t----\n");

	rcu_read_lock();

	for_each_process(task) {
		seq_printf(m, "%d\t%-15s\t%-10s\t%d\t%d\n",
			   task->pid,
			   task->comm,
			   get_task_state_name(task),
			   rcu_dereference(task->real_parent)->pid,
			   task->prio);
	}

	rcu_read_unlock();

	return 0;
}

static int proc_open(struct inode *inode, struct file *file) {
	return single_open(file, proc_show, NULL);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops proc_fops = {
	.proc_open    = proc_open,
	.proc_read    = seq_read,
	.proc_lseek   = seq_lseek,
	.proc_release = single_release,
};
#else
static const struct file_operations proc_fops = {
	.open    = proc_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = single_release,
};
#endif

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

