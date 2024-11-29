#define __STDC_FORMAT_MACROS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "panda/plugin.h"
#include "panda/plugin_plugin.h"
#include "panda/common.h"

// 函数声明
bool init_plugin(void *self);
void uninit_plugin(void *self);
int insn_exec(CPUState *env, target_ulong pc);
bool insn_translate(CPUState *env, target_ulong pc);

uint64_t instr_count=0;
size_t addr=0;
size_t len=0;
int reg=0;
size_t* buf=NULL;


bool insn_translate(CPUState *env, target_ulong pc){
    uint64_t cur_count=rr_get_guest_instr_count();
    if(cur_count<=instr_count){
        return true;
    }
    return false;
}

int insn_exec(CPUState *env, target_ulong pc){
    uint64_t cur_count=rr_get_guest_instr_count();
    if(cur_count!=instr_count){
        return 0;
    }
    if(reg){
        if(reg>8){
            puts("reg should be less than 8");
            return 0;
        }
        CPUArchState *UNUSED(cpu) = (CPUArchState *)env->env_ptr;
        uint64_t ans=cpu->regs[reg-1];
        printf("instr_count:%d   reg:%lx   \n",cur_count,ans);
        return 1;
    }
    if(addr&&len){
        buf=malloc(len+8);
        panda_virtual_memory_read(env,addr,buf,len);
        printf("addr:%lx len:%d value:\n",addr,len);
        for(int i=0;i<(len)/8;i++){
            printf("%d : %lx \n",i,buf[i]);
        }
        free(buf);
        return 1;
    }
    return 1;
}

bool init_plugin(void *self) {
    panda_arg_list *args = panda_get_args("get_val");
    instr_count=panda_parse_uint64(args,"instr_count",0);
    addr=panda_parse_uint64(args,"addr",0);
    reg=panda_parse_uint32(args,"reg",0);
    len=panda_parse_uint64(args,"len",0);
    panda_enable_memcb();
    panda_enable_precise_pc();
    panda_cb pcb;
    pcb.insn_translate=insn_translate;
    panda_register_callback(self,PANDA_CB_INSN_TRANSLATE,pcb);
    pcb.insn_exec=insn_exec;
    panda_register_callback(self,PANDA_CB_INSN_EXEC,pcb);
    return true;
}

void uninit_plugin(void *self) {
    //nothing
}
