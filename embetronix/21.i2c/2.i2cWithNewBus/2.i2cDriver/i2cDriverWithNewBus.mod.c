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
	{ 0x7318c220, "i2c_del_driver" },
	{ 0x14a85cf2, "i2c_unregister_device" },
	{ 0xb6c94b2, "i2c_put_adapter" },
	{ 0xcc97b913, "i2c_register_driver" },
	{ 0xe82cc1c0, "i2c_new_device" },
	{ 0xc01d79f2, "i2c_get_adapter" },
	{ 0x78f2655b, "i2c_transfer_buffer_flags" },
	{ 0x2e5810c6, "__aeabi_unwind_cpp_pr1" },
	{ 0x7c32d0f0, "printk" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("i2c:i2cRTC");

MODULE_INFO(srcversion, "1F7CA5F175329033F064B22");
