#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/param.h>
#include <linux/string.h>

MODULE_AUTHOR("runes");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("an lkm that filters even/odd numbers from a parameter array");
MODULE_VERSION("0.1");

/*
linux kernel that accepts an array of integers via a module parameter,
accepts a mode parameter (string) which can be either even or odd,
on module load:
	- validates that the array is not empty
	- validates that the mode is either even or odd
	- and then filters according to the mode.

on module unload:
	- print how many numbers passed the filter
*/

static int count = 0;
module_param(count, int, 0644);
MODULE_PARM_DESC(count, "maintains the count of the number of ints that passed the filter\n");

static char* mode = "default";
module_param(mode, charp, 0644);
MODULE_PARM_DESC(mode, "mode of the filter");

#define ARR_LEN 10
static int nums[ARR_LEN] = {0};
static int arg_num = 0;
module_param_array(nums, int, &arg_num, 0644);
MODULE_PARM_DESC(nums, "array of ints");

// VALIDATION FUNC
static int validate_params(void) {
	if (mode == NULL || strlen(mode) == 0 || (strcmp("odd", mode) != 0 && strcmp("even", mode) != 0)) {
		pr_err("Invalid Value for Mode string.\n");
		return -EINVAL;
	}

	if (arg_num == 0) {
		pr_warn("No values provided for the nums array.\n");
	} else {
		int i;
		for (i = 0; i < arg_num; i++) {
			if (strcmp("odd", mode) == 0) {
				if ((nums[i] & 1) == 1) {
					pr_info("nums[%d] = %d\n", i, nums[i]);
					count++;
				}
			}

			 else {
			// if nums[i] is even...
				if (nums[i] % 2 == 0) {
					pr_info("nums[%d] = %d\n", i, nums[i]);
					count++;
				}
			}
		}
	}

	return 0;
}


// INIT MODULE
static int __init filter_init(void) {
	pr_info("Loading modules with parameters...\n");

	if (validate_params() != 0) {
		pr_err("Parameter Validation has failed. Aborting module load.\n");
		return -EINVAL;
	}

	pr_info("count = %d\n", count);
	pr_info("mode = %s\n", mode);
	// the array get's printed, so no worries about that...

	pr_info("Module Loaded Successfully.\n");
	return 0;
}


// EXIT MODULE
static void __exit filter_exit(void) {
	pr_info("Count of numbers that passed the filter: %d\n", count);
}

module_init(filter_init);
module_exit(filter_exit);
