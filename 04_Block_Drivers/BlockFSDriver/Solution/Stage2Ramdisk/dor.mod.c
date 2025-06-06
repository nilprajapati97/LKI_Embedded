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
	{ 0x90fd01f4, "module_layout" },
	{ 0x7f9a3e99, "blk_init_queue" },
	{ 0x39cfce09, "blk_cleanup_queue" },
	{ 0xd6ee688f, "vmalloc" },
	{ 0x2e5810c6, "__aeabi_unwind_cpp_pr1" },
	{ 0x573d921e, "page_address" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
	{ 0x999e8297, "vfree" },
	{ 0xd4a330ac, "__alloc_disk_node" },
	{ 0x5f754e5a, "memset" },
	{ 0x7c32d0f0, "printk" },
	{ 0x4da7ed8e, "del_gendisk" },
	{ 0x71a50dbc, "register_blkdev" },
	{ 0xb5a459dc, "unregister_blkdev" },
	{ 0x676bed3f, "put_disk" },
	{ 0x7ad4c8b0, "blk_fetch_request" },
	{ 0x772a0728, "__blk_end_request_all" },
	{ 0x1e047854, "warn_slowpath_fmt" },
	{ 0x9d669763, "memcpy" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
	{ 0x61653f6a, "device_add_disk" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

