
#include <linux/types.h>


bool vmcs_init(void);

static inline u8 _vmclear(phys_addr_t vmcs_phy){
    u64 physc = (u64)vmcs_phy;
    u8 ret;
    asm volatile(
        "vmclear %[phy_mem]; setna %[ret_val]"
        : [ret_val] "=rm"(ret)
        : [phy_mem] "m"(physc)
        :"cc","memory"
    );
    return ret; 
}

static inline void _vmptrld(phys_addr_t vmcs_phy){
    u64 physc = (u64)vmcs_phy;
    asm volatile(
        "vmptrld %[mem]"
        : :[mem] "m"(physc) 
        :"memory"
    );
}

static inline void _vmwrite(u64 field_encoding,u64 value){
    asm volatile(
        "vmwrite %[value],%[field]"
        : :[field]"r"(field_encoding),[value]"rm"(value)
        : "memory","cc"
    );
}

static inline u64 _vmread(u64 field_encoding){
    u64 value;
    asm volatile(
        "vmread %[field],%[value]"
        :[value]"=rm"(value)
        :[field]"r"(field_encoding) 
        :"memory","cc"
    );
    return value;
}