#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <asm/processor.h>
#include "../include/vmx.h"

#define X86_64_CR4_VMXE_BIT                       13 
#define X86_64_CR4_VMXE                           _BITUL(X86_64_CR4_VMXE_BIT)
#define FEATURE_CONTROL_VMXON_ENABLED_OUTSIDE_SMX (1<<2)
#define FEATURE_CONTROL_LOCK_BIT_SET              (1<<0)
#define FEATURE_CONTROL_IA32_MSR                  0x0000003a
#define MSR_IA32_VMX_CR0_FIXED0                   0x00000486
#define MSR_IA32_VMX_CR0_FIXED1                   0x00000487
#define MSR_IA32_VMX_CR4_FIXED0                   0x00000488
#define MSR_IA32_VMX_CR4_FIXED1                   0x00000489
#define VMCS_PAGE_SIZE                            4096


static void* vmxon_region;
static phys_addr_t vmxon_phy_region;

bool vmx_support(void){
    unsigned int eax,ebx,ecx,edx;
    cpuid(1,&eax,&ebx,&ecx,&edx);
    return (ecx>>5u)&1;
}

bool get_vmx_operation(void){
    uint64_t cr4;
    uint64_t cr0;
    uint64_t feature_control; 
    uint64_t vmxon_phy_region;
    //CR4.VMXE - virt enable
    cr4 = read_cr4()|X86_64_CR4_VMXE;
    write_cr4(cr4);
    //MSR setup for VMXON
    unsigned long msr_set_bits = FEATURE_CONTROL_VMXON_ENABLED_OUTSIDE_SMX|FEATURE_CONTROL_LOCK_BIT_SET;
    feature_control = vmx_read_msr(FEATURE_CONTROL_IA32_MSR);
    pr_info("GHV: MSR value read %ld\n",(long)feature_control);
    if ((feature_control&FEATURE_CONTROL_LOCK_BIT_SET)==0){
        vmx_write_msr(
            FEATURE_CONTROL_IA32_MSR,feature_control|msr_set_bits
        );
    }else if((feature_control&FEATURE_CONTROL_VMXON_ENABLED_OUTSIDE_SMX)==0){
        pr_info("GHV: MSR is locked and vmx is not allowed outside SMX\n");
        return false;
    }
    //CR0 CR4 validation
    asm volatile(
        "mov %%cr0, %0"
        : "=r"(cr0) : : "memory"
    );
    cr0&=vmx_read_msr(MSR_IA32_VMX_CR0_FIXED1);
    cr0|=vmx_read_msr(MSR_IA32_VMX_CR0_FIXED0);
    asm volatile(
        "mov %0,%%cr0"
        : :"r"(cr0) : "memory"
    );
    asm volatile(
        "mov %%cr4, %0"
        : "=r"(cr4) : : "memory"
    );
    cr4&=vmx_read_msr(MSR_IA32_VMX_CR4_FIXED1);
    cr4|=vmx_read_msr(MSR_IA32_VMX_CR4_FIXED0);
    asm volatile(
        "mov %0, %%cr4"
        : :"r"(cr4) : "memory"
    );
void* vmxon_region = kzalloc(VMCS_PAGE_SIZE,GFP_KERNEL);
    if (vmxon_region==NULL){
        pr_info("GHV: Unable to allocate space for VMXON region\n");
        return false;
    }
    vmxon_phy_region = __pa(vmxon_region);
    *(uint32_t*) vmxon_region = vmx_get_revision_id();

    if (_vmxon(vmxon_phy_region)!=0){
        pr_err("GHV: failed to send VMXON\n");
        return false;
    }
    pr_info("GHV: VMXON successful\n");
    return true;
}

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
