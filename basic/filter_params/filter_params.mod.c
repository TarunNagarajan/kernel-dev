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
	{ 0xd272d446, "__fentry__" },
	{ 0x43a349ca, "strlen" },
	{ 0x888b8f57, "strcmp" },
	{ 0x90a48d82, "__ubsan_handle_out_of_bounds" },
	{ 0xc368a5e1, "param_array_ops" },
	{ 0xc368a5e1, "param_ops_int" },
	{ 0xc368a5e1, "param_ops_charp" },
	{ 0xe8213e80, "_printk" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0xab006604, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0xd272d446,
	0x43a349ca,
	0x888b8f57,
	0x90a48d82,
	0xc368a5e1,
	0xc368a5e1,
	0xc368a5e1,
	0xe8213e80,
	0xd272d446,
	0xab006604,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"__fentry__\0"
	"strlen\0"
	"strcmp\0"
	"__ubsan_handle_out_of_bounds\0"
	"param_array_ops\0"
	"param_ops_int\0"
	"param_ops_charp\0"
	"_printk\0"
	"__x86_return_thunk\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "BA4C02F9A74FC8278A043F8");
