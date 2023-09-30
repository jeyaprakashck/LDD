#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

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

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xf230cadf, "module_layout" },
	{ 0x8c76ee53, "cdev_del" },
	{ 0x17ae16c5, "device_destroy" },
	{ 0x8c03d20c, "destroy_workqueue" },
	{ 0x37a0cba, "kfree" },
	{ 0xd64795cd, "class_destroy" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0xb2d48a2e, "queue_work_on" },
	{ 0x43a53735, "__alloc_workqueue_key" },
	{ 0x1e9a78cc, "kmem_cache_alloc_trace" },
	{ 0x51201dd7, "kmalloc_caches" },
	{ 0x89ab7bf8, "cdev_add" },
	{ 0x2b68ed92, "cdev_init" },
	{ 0xabe5f4f7, "device_create" },
	{ 0x4751a8ae, "__class_create" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0xf4fa543b, "arm_copy_to_user" },
	{ 0x5f754e5a, "memset" },
	{ 0x28cc25db, "arm_copy_from_user" },
	{ 0x2e5810c6, "__aeabi_unwind_cpp_pr1" },
	{ 0x7c32d0f0, "printk" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "56FE494216AE82D7B0C2D38");
