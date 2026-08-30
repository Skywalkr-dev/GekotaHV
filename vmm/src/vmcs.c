#include <cstdint>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <asm/processor.h>
#include "vmx.h"
#include "vmcs.h"

#define VMCS_PAGE_SIZE                            4096

static void* vmcs_region;
static phys_addr_t vmcs_phy_region;



bool vmcs_init(void){
    vmcs_region = kzalloc(VMCS_PAGE_SIZE,GFP_KERNEL);
    if (vmcs_region==NULL){
        pr_info("GHV: failed to allocate space for VMCS");
        return false;
    }
    *(uint32_t*) vmcs_region = vmx_get_revision_id();
    vmcs_phy_region = __pa(vmcs_region);
    u8 ret_code = _vmclear(vmcs_phy_region);
    if (ret_code!=0){
        pr_info("GHV: failed to clear allocated space");
        return false;
    }
}