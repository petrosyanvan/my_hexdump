#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x9d35aeec, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x66724a48, __VMLINUX_SYMBOL_STR(class_unregister) },
	{ 0xb75368e6, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0x17d070c2, __VMLINUX_SYMBOL_STR(__mutex_init) },
	{ 0x78f5ccab, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0x9bced8a5, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0x6bc3fbc0, __VMLINUX_SYMBOL_STR(__unregister_chrdev) },
	{ 0x220464a8, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0x177758bc, __VMLINUX_SYMBOL_STR(__register_chrdev) },
	{ 0x4a055dfb, __VMLINUX_SYMBOL_STR(filp_open) },
	{ 0xc3f0cddf, __VMLINUX_SYMBOL_STR(mutex_trylock) },
	{ 0xdb7305a1, __VMLINUX_SYMBOL_STR(__stack_chk_fail) },
	{ 0xffaa0d10, __VMLINUX_SYMBOL_STR(vfs_write) },
	{ 0x91715312, __VMLINUX_SYMBOL_STR(sprintf) },
	{ 0x3356b90b, __VMLINUX_SYMBOL_STR(cpu_tss) },
	{ 0x347cd1b3, __VMLINUX_SYMBOL_STR(mutex_unlock) },
	{ 0xb7b734dc, __VMLINUX_SYMBOL_STR(filp_close) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "360A65A9947E5AE0B0D567F");
