#define __STDC_FORMAT_MACROS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "panda/plugin.h"
#include "panda/plugin_plugin.h"
#include "panda/common.h"

#include "osi/osi_types.h"
#include "osi/osi_ext.h"

bool init_plugin(void *self);
void uninit_plugin(void *self);

void virt_mem_before_write(CPUState *env, target_ulong pc, target_ulong addr, target_ulong size, void *buf);


uint64_t begin_cnt=0;
uint64_t end_cnt=0;
uint64_t begin_addr=0;
uint64_t end_addr=0;




void virt_mem_before_write(CPUState *env, target_ulong pc, target_ulong addr, target_ulong size, void *buf){
    if(addr<begin_addr)return;
    if(addr>end_addr)return;
    uint64_t cur=rr_get_guest_instr_count();
    if(cur>end_cnt)return;
    if(cur<begin_cnt)return;
    OsiThread*t=get_current_thread(env);
    printf("pc:%lx    addr:%lx   size:%lx   pid:%d    tid:%d   \n",pc,addr,size,t->pid,t->tid);
    int num=size/8;
    for(int i=0;i<num;i++)printf("%d : %lx \n",i,((size_t*)buf)[i]);
    return;
}




bool init_plugin(void *self) {
    panda_arg_list *args = panda_get_args("mon_addr");
    begin_cnt=panda_parse_uint64(args,"begin_cnt",0);
    end_cnt=panda_parse_uint64(args,"end_cnt",0);
    begin_addr=panda_parse_uint64(args,"begin_addr",0);
    end_addr=panda_parse_uint64(args,"end_addr",0);

    panda_require("osi");
    assert(init_osi_api());

    panda_enable_memcb();

    panda_cb pcb;
   
    pcb.virt_mem_before_write=virt_mem_before_write;
    panda_register_callback(self,PANDA_CB_VIRT_MEM_BEFORE_WRITE,pcb);

    return true;
}

void uninit_plugin(void *self) {
    //nothing
}
