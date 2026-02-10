#include "../model_util.h"

static ExecResult execute_C_ADDI(CoreModel& core, CInst inst) {
    core.write_xreg(inst.rd(), core.xreg(inst.rd()) + (uint32_t)inst.ci_imm());
    return ExecResult::ok();
}

static ExecResult execute_C_ADDI4SPN(CoreModel& core, CInst inst) {
    uint32_t uimm = inst.ciw_uimm();
    if (uimm == 0)
        return ExecResult::illegal_inst();
    core.write_xreg(inst.rd_c(), core.xreg(xreg_from_idx(2)) + uimm);
    return ExecResult::ok();
}

static ExecResult execute_C_ADDI16SP(CoreModel& core, CInst inst) {
    int32_t imm = inst.ci_addi16sp_imm();
    if (imm == 0)
        return ExecResult::illegal_inst();
    core.write_xreg(xreg_from_idx(2), core.xreg(xreg_from_idx(2)) + (uint32_t)imm);
    return ExecResult::ok();
}

static ExecResult execute_C_LI(CoreModel& core, CInst inst) {
    core.write_xreg(inst.rd(), (uint32_t)inst.ci_imm());
    return ExecResult::ok();
}

static ExecResult execute_C_LUI(CoreModel& core, CInst inst) {
    int32_t imm = inst.ci_lui_imm();
    if (imm == 0)
        return ExecResult::illegal_inst();
    core.write_xreg(inst.rd(), (uint32_t)imm);
    return ExecResult::ok();
}

static ExecResult execute_C_ANDI(CoreModel& core, CInst inst) {
    core.write_xreg(inst.rs1_c(), core.xreg(inst.rs1_c()) & (uint32_t)inst.ci_imm());
    return ExecResult::ok();
}

static ExecResult execute_C_SUB(CoreModel& core, CInst inst) {
    core.write_xreg(inst.rs1_c(), core.xreg(inst.rs1_c()) - core.xreg(inst.rs2_c()));
    return ExecResult::ok();
}

static ExecResult execute_C_XOR(CoreModel& core, CInst inst) {
    core.write_xreg(inst.rs1_c(), core.xreg(inst.rs1_c()) ^ core.xreg(inst.rs2_c()));
    return ExecResult::ok();
}

static ExecResult execute_C_OR(CoreModel& core, CInst inst) {
    core.write_xreg(inst.rs1_c(), core.xreg(inst.rs1_c()) | core.xreg(inst.rs2_c()));
    return ExecResult::ok();
}

static ExecResult execute_C_AND(CoreModel& core, CInst inst) {
    core.write_xreg(inst.rs1_c(), core.xreg(inst.rs1_c()) & core.xreg(inst.rs2_c()));
    return ExecResult::ok();
}

static ExecResult execute_C_ADD(CoreModel& core, CInst inst) {
    core.write_xreg(inst.rd(), core.xreg(inst.rd()) + core.xreg(inst.cr_rs2()));
    return ExecResult::ok();
}

static ExecResult execute_C_MV(CoreModel& core, CInst inst) {
    core.write_xreg(inst.rd(), core.xreg(inst.cr_rs2()));
    return ExecResult::ok();
}

static ExecResult execute_C_SLLI(CoreModel& core, CInst inst) {
    uint32_t shamt = inst.ci_shamt();
    core.write_xreg(inst.rd(), core.xreg(inst.rd()) << shamt);
    return ExecResult::ok();
}

static ExecResult execute_C_SRLI(CoreModel& core, CInst inst) {
    uint32_t shamt = inst.ci_shamt();
    core.write_xreg(inst.rs1_c(), core.xreg(inst.rs1_c()) >> shamt);
    return ExecResult::ok();
}

static ExecResult execute_C_SRAI(CoreModel& core, CInst inst) {
    uint32_t shamt = inst.ci_shamt();
    core.write_xreg(inst.rs1_c(), (uint32_t)((int32_t)core.xreg(inst.rs1_c()) >> shamt));
    return ExecResult::ok();
}

#define EXECUTE_C_ADDI(core, inst, mem, npc) execute_C_ADDI(core, inst)
#define EXECUTE_C_ADDI4SPN(core, inst, mem, npc) execute_C_ADDI4SPN(core, inst)
#define EXECUTE_C_ADDI16SP(core, inst, mem, npc) execute_C_ADDI16SP(core, inst)
#define EXECUTE_C_LI(core, inst, mem, npc) execute_C_LI(core, inst)
#define EXECUTE_C_LUI(core, inst, mem, npc) execute_C_LUI(core, inst)
#define EXECUTE_C_ANDI(core, inst, mem, npc) execute_C_ANDI(core, inst)
#define EXECUTE_C_SUB(core, inst, mem, npc) execute_C_SUB(core, inst)
#define EXECUTE_C_XOR(core, inst, mem, npc) execute_C_XOR(core, inst)
#define EXECUTE_C_OR(core, inst, mem, npc) execute_C_OR(core, inst)
#define EXECUTE_C_AND(core, inst, mem, npc) execute_C_AND(core, inst)
#define EXECUTE_C_ADD(core, inst, mem, npc) execute_C_ADD(core, inst)
#define EXECUTE_C_MV(core, inst, mem, npc) execute_C_MV(core, inst)
#define EXECUTE_C_SLLI(core, inst, mem, npc) execute_C_SLLI(core, inst)
#define EXECUTE_C_SRLI(core, inst, mem, npc) execute_C_SRLI(core, inst)
#define EXECUTE_C_SRAI(core, inst, mem, npc) execute_C_SRAI(core, inst)
