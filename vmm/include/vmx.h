#ifndef GHV_VMX_H
#define GHV_VMX_H

#include <linux/types.h>

#define MSR_IA32_VMX_BASIC              0x00000480

bool vmx_support(void);
bool get_vmx_operation(void);

static inline uint64_t read_cr4(void){
    uint64_t ret;
    asm volatile(
        "mov %%cr4,%0"
        : "=r"(ret) : :"memory"
    );
    return ret;
}

static inline void write_cr4(uint64_t cr4){
    asm volatile(
        "mov %0,%%cr4"
        : :"r"(cr4):"memory"
    );
}

static inline u64 vmx_read_msr(u32 msr){
    u32 low,high;
    asm volatile(
        "rdmsr"
        : "=a"(low),"=d"(high)
        : "c"(msr)
    );

    return ((u64)high<<32)|low;
}

static inline void vmx_write_msr(u32 msr,u64 value){
    u32 high = (u32)(value>>32);
    u32 low = (u32)value;
    asm volatile(
        "wrmsr"
        :
        : "c"(msr), "d"(high),"a"(low)
    );
}

static inline uint32_t vmx_get_revision_id(void){
    return vmx_read_msr(MSR_IA32_VMX_BASIC);
}

static inline int _vmxon(uint64_t phys){
    uint8_t ret;
    asm volatile(
        "vmxon %[pa]"
        :[ret] "=rm"(ret)
        :[pa] "m"(phys)
        :"cc", "memory"
    ); 
    return ret;
}

#endif