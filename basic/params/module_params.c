#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/param.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("runes");
MODULE_DESCRIPTION("LKM with parameters and validation");
MODULE_VERSION("1.0");

// MODULE PARAMS
static int myint = 0;
module_param(myint, int, 0644);
MODULE_PARM_DESC(myint, "Integer between 1 and 100");

static char *mystr = "default";
module_param(mystr, charp, 0644);
MODULE_PARM_DESC(mystr, "non-empty string parameter");

#define ARR_LEN 5
static int myarray[ARR_LEN] = {0};
static int arr_argc = 0;
module_param_array(myarray, int, &arr_argc, 0644);
MODULE_PARM_DESC(myarray, "array of integers");

// VALIDATION FUNCS
static int validate_parameters(void) {
	if (myint < 1 || myint > 100) {
		pr_err("Invalid value for myint: %d \n", myint);
		return -EINVAL;
	}

	if (mystr == NULL || strlen(mystr) == 0) {
		pr_err("Invalid String parameter: must not be empty");
		return -EINVAL;
	}

	if (arr_argc == 0) {
		pr_warn("No values provided for myarray. \n");
	} else {
		int i;
		for (i = 0; i < arr_argc; i++) {
			pr_info("myarray[%d] = %d\n", i, myarray[i]);
		}
	}
	return 0;
}

static int __init param_mod_init(void) {
	pr_info("Loading modules with parameters...");
	if (validate_parameters() != 0) {
		pr_err("Parameter Validation Failed. Aborting Module Load.");
		return -EINVAL;
	}

	pr_info("myint = %d\n", myint);
	pr_info("mystr = %s\n", mystr);
	pr_info("Module Loaded Successfully.\n");

	return 0;
}

static void __exit param_mod_exit(void) {
	pr_info("Module Exists.\n");
}

module_init(param_mod_init);
module_exit(param_mod_exit);
