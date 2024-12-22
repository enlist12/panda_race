#define __STDC_FORMAT_MACROS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "panda/plugin.h"
#include "panda/plugin_plugin.h"
#include "panda/common.h"
//hello!
bool init_plugin(void *self);
void uninit_plugin(void *self);
void after_vmload(CPUState *env);

void after_vmload(CPUState *env) {
    puts("hello world");
}

bool init_plugin(void *self) {
    panda_cb pcb;
    pcb.after_loadvm = after_vmload; 
    panda_register_callback(self, PANDA_CB_AFTER_LOADVM, pcb);
    return true; 
}

void uninit_plugin(void *self) {
    // nothing
}
