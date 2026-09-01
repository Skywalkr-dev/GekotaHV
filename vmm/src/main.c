#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>

#include "../include/vmx.h"
#include "../include/vmcs.h"

static int __init ghv_init(void){
    pr_info("GHV: started...\n");

    if (!vmx_support()) {
        pr_err("GHV: CPU does not support VMX\n");
        return -ENODEV;
    }

    pr_info("GHV: VMX supported\n");

    if (!get_vmx_operation()) {
        pr_err("GHV: failed to enable VMX\n");
        return -EIO;
    }
    if(!vmcs_init()){
        pr_err("GHV: failed to allocate space for VMCS\n");
        return -EIO;
    }
    pr_info("GHV: VMX setup successful\n");

    return 0;
}

static void __exit ghv_exit(void){
    pr_info("GHV: exited...\n");
}

module_init(ghv_init);
module_exit(ghv_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Skywalkr-dev");
MODULE_DESCRIPTION("Gekota Hypervisor");
