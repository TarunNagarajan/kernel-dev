#include <linux/module.h>
#include <linux/export-internal.h>
#include <linux/compiler.h>

MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xd3981938, "seq_printf" },
	{ 0x3e40aee5, "proc_create" },
	{ 0xe8213e80, "_printk" },
	{ 0xd9a1e815, "remove_proc_entry" },
	{ 0x01bbdc04, "seq_read" },
	{ 0x4f109577, "seq_lseek" },
	{ 0x4bd80bc9, "single_release" },
	{ 0xd272d446, "__fentry__" },
	{ 0xc13d7999, "single_open" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0x39696530, "init_task" },
	{ 0xab006604, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0xd3981938,
	0x3e40aee5,
	0xe8213e80,
	0xd9a1e815,
	0x01bbdc04,
	0x4f109577,
	0x4bd80bc9,
	0xd272d446,
	0xc13d7999,
	0xd272d446,
	0x39696530,
	0xab006604,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"seq_printf\0"
	"proc_create\0"
	"_printk\0"
	"remove_proc_entry\0"
	"seq_read\0"
	"seq_lseek\0"
	"single_release\0"
	"__fentry__\0"
	"single_open\0"
	"__x86_return_thunk\0"
	"init_task\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "2679097F3D5F421FEB8BABA");
