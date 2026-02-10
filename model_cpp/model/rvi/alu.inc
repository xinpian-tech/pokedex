#include "../model_util.h"

static ExecResult execute_alu_reg(CoreModel& core, Inst inst, AluOp op) {
    XRegIdx xd = inst.xd();
    XRegIdx xs1 = inst.xs1();
    XRegIdx xs2 = inst.xs2();
    uint32_t src1 = core.xreg(xs1);
    uint32_t src2 = core.xreg(xs2);

    uint32_t value = compute_u32(op, src1, src2);
    core.write_xreg(xd, value);

    return ExecResult::ok();
}

static ExecResult execute_alu_imm(CoreModel& core, Inst inst, AluOp op) {
    XRegIdx xd = inst.xd();
    XRegIdx xs1 = inst.xs1();
    uint32_t src1 = core.xreg(xs1);
    uint32_t src2 = (uint32_t)inst.imm_i();

    uint32_t value = compute_u32(op, src1, src2);
    core.write_xreg(xd, value);

    return ExecResult::ok();
}

static ExecResult execute_shift_reg(CoreModel& core, Inst inst, ShiftOp op) {
    XRegIdx xd = inst.xd();
    XRegIdx xs1 = inst.xs1();
    XRegIdx xs2 = inst.xs2();
    uint32_t src1 = core.xreg(xs1);
    uint8_t src2 = core.xreg(xs2);

    uint8_t shamt5 = src2 & 0x1f;
    uint32_t value = shift_u32(op, src1, shamt5);
    core.write_xreg(xd, value);

    return ExecResult::ok();
}

static ExecResult execute_shift_imm(CoreModel& core, Inst inst, ShiftOp op) {
    XRegIdx xd = inst.xd();
    XRegIdx xs1 = inst.xs1();
    uint32_t src1 = core.xreg(xs1);
    uint8_t shamt5 = inst.shamt5();

    uint32_t value = shift_u32(op, src1, shamt5);
    core.write_xreg(xd, value);

    return ExecResult::ok();
}

#define EXECUTE_ADD(core, inst, mem, npc) execute_alu_reg(core, inst, AluOp::ADD)
#define EXECUTE_SUB(core, inst, mem, npc) execute_alu_reg(core, inst, AluOp::SUB)
#define EXECUTE_SLT(core, inst, mem, npc) execute_alu_reg(core, inst, AluOp::SLT_S)
#define EXECUTE_SLTU(core, inst, mem, npc) execute_alu_reg(core, inst, AluOp::SLT_U)
#define EXECUTE_XOR(core, inst, mem, npc) execute_alu_reg(core, inst, AluOp::XOR)
#define EXECUTE_OR(core, inst, mem, npc) execute_alu_reg(core, inst, AluOp::OR)
#define EXECUTE_AND(core, inst, mem, npc) execute_alu_reg(core, inst, AluOp::AND)
#define EXECUTE_MUL(core, inst, mem, npc) execute_alu_reg(core, inst, AluOp::MUL)
#define EXECUTE_MULH(core, inst, mem, npc) execute_alu_reg(core, inst, AluOp::MULH_SS)
#define EXECUTE_MULHSU(core, inst, mem, npc) execute_alu_reg(core, inst, AluOp::MULH_SU)
#define EXECUTE_MULHU(core, inst, mem, npc) execute_alu_reg(core, inst, AluOp::MULH_UU)
#define EXECUTE_DIV(core, inst, mem, npc) execute_alu_reg(core, inst, AluOp::DIV_S)
#define EXECUTE_DIVU(core, inst, mem, npc) execute_alu_reg(core, inst, AluOp::DIV_U)
#define EXECUTE_REM(core, inst, mem, npc) execute_alu_reg(core, inst, AluOp::REM_S)
#define EXECUTE_REMU(core, inst, mem, npc) execute_alu_reg(core, inst, AluOp::REM_U)

#define EXECUTE_ADDI(core, inst, mem, npc) execute_alu_imm(core, inst, AluOp::ADD)
#define EXECUTE_SLTI(core, inst, mem, npc) execute_alu_imm(core, inst, AluOp::SLT_S)
#define EXECUTE_SLTIU(core, inst, mem, npc) execute_alu_imm(core, inst, AluOp::SLT_U)
#define EXECUTE_XORI(core, inst, mem, npc) execute_alu_imm(core, inst, AluOp::XOR)
#define EXECUTE_ORI(core, inst, mem, npc) execute_alu_imm(core, inst, AluOp::OR)
#define EXECUTE_ANDI(core, inst, mem, npc) execute_alu_imm(core, inst, AluOp::AND)

#define EXECUTE_SLL(core, inst, mem, npc) execute_shift_reg(core, inst, ShiftOp::LOGIC_LEFT)
#define EXECUTE_SRL(core, inst, mem, npc) execute_shift_reg(core, inst, ShiftOp::LOGIC_RIGHT)
#define EXECUTE_SRA(core, inst, mem, npc) execute_shift_reg(core, inst, ShiftOp::ARITH_RIGHT)

#define EXECUTE_SLLI(core, inst, mem, npc) execute_shift_imm(core, inst, ShiftOp::LOGIC_LEFT)
#define EXECUTE_SRLI(core, inst, mem, npc) execute_shift_imm(core, inst, ShiftOp::LOGIC_RIGHT)
#define EXECUTE_SRAI(core, inst, mem, npc) execute_shift_imm(core, inst, ShiftOp::ARITH_RIGHT)
