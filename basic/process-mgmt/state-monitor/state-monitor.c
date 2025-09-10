#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("runes");
MODULE_DESCRIPTION("process-mgmt: simple process state monitor"):
MODULE_VERSION("1.0");

static struct proc_dir_entry* proc_entry;

static const char* get_task_state_name(long state) {
	switch(state) {
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

