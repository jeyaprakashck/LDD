#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(.gnu.linkonce.this_module) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section(__versions) = {
	{ 0x46a6fb9b, "module_layout" },
	{ 0x349fd30a, "class_destroy" },
	{ 0x98a46a0a, "platform_driver_unregister" },
	{ 0xc7152159, "__platform_driver_register" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x6c13ee30, "__class_create" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x5b48788f, "device_destroy" },
	{ 0xa970bd05, "cdev_del" },
	{ 0xd4fb8b47, "device_create" },
	{ 0x476d1764, "cdev_add" },
	{ 0x12865478, "cdev_init" },
	{ 0x9cd0a981, "devm_kmalloc" },
	{ 0xc5850110, "printk" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "C03FDEBB463EBC0BDEE5054");
