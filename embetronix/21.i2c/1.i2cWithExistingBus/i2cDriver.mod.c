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
	{ 0x5c7ab3bb, "i2c_smbus_read_byte_data" },
	{ 0xb6c94b2, "i2c_put_adapter" },
	{ 0xcc97b913, "i2c_register_driver" },
	{ 0xe82cc1c0, "i2c_new_device" },
	{ 0xc01d79f2, "i2c_get_adapter" },
	{ 0x2e5810c6, "__aeabi_unwind_cpp_pr1" },
	{ 0x7c32d0f0, "printk" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("i2c:I2C Slave");

MODULE_INFO(srcversion, "3C8DBC8C35312A7FB6BEC52");
