#include "../model_util.h"

static ExecResult execute_LUI(CoreModel& core, Inst inst) {
    XRegIdx xd = inst.xd();

    uint32_t value = (uint32_t)inst.imm_u();
    core.write_xreg(xd, value);

    return ExecResult::ok();
}

static ExecResult execute_AUIPC(CoreModel& core, Inst inst) {
    XRegIdx xd = inst.xd();
    uint32_t pc = core.pc();

    uint32_t value = pc + (uint32_t)inst.imm_u();
    core.write_xreg(xd, value);

    return ExecResult::ok();
}

#define EXECUTE_LUI(core, inst, mem, npc) execute_LUI(core, inst)
#define EXECUTE_AUIPC(core, inst, mem, npc) execute_AUIPC(core, inst)
