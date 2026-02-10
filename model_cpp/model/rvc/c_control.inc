#include "../model_util.h"

static ExecResult execute_C_J([[maybe_unused]] CoreModel& core, CInst inst, NextPc& npc) {
    uint32_t target = core.pc() + (uint32_t)inst.cj_imm();
    npc.jump(target);
    return ExecResult::ok();
}

static ExecResult execute_C_JAL(CoreModel& core, CInst inst, NextPc& npc) {
    uint32_t target = core.pc() + (uint32_t)inst.cj_imm();

    core.write_xreg(XREG_RA, npc.read());
    npc.jump(target);

    return ExecResult::ok();
}

static ExecResult execute_C_JR(CoreModel& core, CInst inst, NextPc& npc) {
    uint32_t target = core.xreg(inst.rd());
    if (inst.rd().is_zero())
        return ExecResult::illegal_inst();
    npc.jump(target);
    return ExecResult::ok();
}

static ExecResult execute_C_JALR(CoreModel& core, CInst inst, NextPc& npc) {
    uint32_t target = core.xreg(inst.rd());
    core.write_xreg(XREG_RA, npc.read());
    npc.jump(target);
    return ExecResult::ok();
}

static ExecResult execute_C_BEQZ(CoreModel& core, CInst inst, NextPc& npc) {
    uint32_t target = core.pc() + (uint32_t)inst.cb_imm();
    if (core.xreg(inst.rs1_c()) == 0)
        npc.jump(target);
    return ExecResult::ok();
}

static ExecResult execute_C_BNEZ(CoreModel& core, CInst inst, NextPc& npc) {
    uint32_t target = core.pc() + (uint32_t)inst.cb_imm();
    if (core.xreg(inst.rs1_c()) != 0)
        npc.jump(target);
    return ExecResult::ok();
}

static ExecResult execute_C_EBREAK([[maybe_unused]] CoreModel& core, [[maybe_unused]] CInst inst) {
    return ExecResult::exception(ExecResult::XCPT_CODE_BREAKPOINT, core.pc());
}

static ExecResult execute_C_NOP([[maybe_unused]] CoreModel& core, [[maybe_unused]] CInst inst) {
    return ExecResult::ok();
}

#define EXECUTE_C_J(core, inst, mem, npc) execute_C_J(core, inst, npc)
#define EXECUTE_C_JAL(core, inst, mem, npc) execute_C_JAL(core, inst, npc)
#define EXECUTE_C_JR(core, inst, mem, npc) execute_C_JR(core, inst, npc)
#define EXECUTE_C_JALR(core, inst, mem, npc) execute_C_JALR(core, inst, npc)
#define EXECUTE_C_BEQZ(core, inst, mem, npc) execute_C_BEQZ(core, inst, npc)
#define EXECUTE_C_BNEZ(core, inst, mem, npc) execute_C_BNEZ(core, inst, npc)
#define EXECUTE_C_EBREAK(core, inst, mem, npc) execute_C_EBREAK(core, inst)
#define EXECUTE_C_NOP(core, inst, mem, npc) execute_C_NOP(core, inst)
