#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xba997dd0, "module_layout" },
	{ 0xfe990052, "gpio_free" },
	{ 0xc1514a3b, "free_irq" },
	{ 0xbd416da1, "gpiod_unexport" },
	{ 0xd6b8e852, "request_threaded_irq" },
	{ 0x4a1baa7a, "gpiod_to_irq" },
	{ 0xa1f64e82, "gpiod_set_debounce" },
	{ 0xa72f8878, "gpiod_direction_input" },
	{ 0xaf62c0cc, "gpiod_export" },
	{ 0x7cfad8cd, "gpiod_direction_output_raw" },
	{ 0x47229b5c, "gpio_request" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
	{ 0x27e1a049, "printk" },
	{ 0xf38aece0, "gpiod_get_raw_value" },
	{ 0x48dea93, "gpiod_set_raw_value" },
	{ 0xc88bd805, "gpio_to_desc" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "5CDFA325D702E9456C97C29");
