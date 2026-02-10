#include "../model_util.h"

enum class AluOp {
    ADD,
    SUB,
    SLT_S,
    SLT_U,
    AND,
    OR,
    XOR,
    MUL,
    MULH_SS,
    MULH_SU,
    MULH_UU,
    DIV_S,
    DIV_U,
    REM_S,
    REM_U,
};

enum class ShiftOp {
    LOGIC_LEFT,
    LOGIC_RIGHT,
    ARITH_RIGHT,
};

static uint32_t compute_u32(AluOp op, uint32_t src1, uint32_t src2) {
    switch (op) {
        case AluOp::ADD:   return src1 + src2;
        case AluOp::SUB:   return src1 - src2;
        case AluOp::SLT_S: return (int32_t)src1 < (int32_t)src2 ? 1u : 0u;
        case AluOp::SLT_U: return src1 < src2 ? 1u : 0u;
        case AluOp::AND:   return src1 & src2;
        case AluOp::OR:    return src1 | src2;
        case AluOp::XOR:   return src1 ^ src2;
        case AluOp::MUL:   return src1 * src2;
        case AluOp::MULH_SS: {
            int64_t a = (int64_t)(int32_t)src1;
            int64_t b = (int64_t)(int32_t)src2;
            return (uint32_t)((a * b) >> 32);
        }
        case AluOp::MULH_SU: {
            int64_t a = (int64_t)(int32_t)src1;
            int64_t b = (int64_t)(uint32_t)src2;
            return (uint32_t)((a * b) >> 32);
        }
        case AluOp::MULH_UU: {
            uint64_t a = (uint64_t)src1;
            uint64_t b = (uint64_t)src2;
            return (uint32_t)((a * b) >> 32);
        }
        case AluOp::DIV_S: {
            if (src2 == 0)
                return UINT32_MAX; // -1
            if (src1 == 0x80000000u && src2 == 0xffffffffu)
                return src1; // overflow: INT32_MIN / -1 = INT32_MIN
            return (uint32_t)((int32_t)src1 / (int32_t)src2);
        }
        case AluOp::DIV_U: {
            if (src2 == 0)
                return UINT32_MAX;
            return src1 / src2;
        }
        case AluOp::REM_S: {
            if (src2 == 0)
                return src1;
            if (src1 == 0x80000000u && src2 == 0xffffffffu)
                return 0;
            return (uint32_t)((int32_t)src1 % (int32_t)src2);
        }
        case AluOp::REM_U: {
            if (src2 == 0)
                return src1;
            return src1 % src2;
        }
        default: abort();
    }
}

static uint32_t shift_u32(ShiftOp op, uint32_t src1, uint8_t shamt) {
    switch (op) {
        case ShiftOp::LOGIC_LEFT:  return src1 << shamt;
        case ShiftOp::LOGIC_RIGHT: return src1 >> shamt;
        case ShiftOp::ARITH_RIGHT: return (uint32_t)((int32_t)src1 >> shamt);
        default: abort();
    }
}

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
