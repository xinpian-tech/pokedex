#include "../model_util.h"

enum class BranchOp {
    EQ,
    NEQ,
    LT_S,
    GE_S,
    LT_U,
    GE_U,
};

static bool compare_u32(BranchOp op, uint32_t src1, uint32_t src2) {
    switch (op) {
        case BranchOp::EQ:   return src1 == src2;
        case BranchOp::NEQ:  return src1 != src2;
        case BranchOp::LT_S: return int32_t(src1) < int32_t(src2);
        case BranchOp::GE_S: return int32_t(src1) >= int32_t(src2);
        case BranchOp::LT_U: return src1 < src2;
        case BranchOp::GE_U: return src1 >= src2;
        default: abort();
    }
}

static ExecResult execute_branch(CoreModel& core, Inst inst, BranchOp op, NextPc& npc) {
    XRegIdx xs1 = inst.xs1();
    XRegIdx xs2 = inst.xs2();
    uint32_t src1 = core.xreg(xs1);
    uint32_t src2 = core.xreg(xs2);
    uint32_t pc = core.pc();

    if (compare_u32(op, src1, src2)) {
        uint32_t target = pc + uint32_t(inst.imm_b());

        // target address is always aligned with ext C
        // TODO: C is disabled
        static_assert(CoreModel::EXT_C);

        npc.jump(target);
    }

    return ExecResult::ok();
}

static ExecResult execute_JAL(CoreModel& core, Inst inst, NextPc& npc) {
    XRegIdx xd = inst.xd();
    uint32_t pc = core.pc();

    uint32_t target = pc + (uint32_t)inst.imm_j();

    // target address is always aligned with ext C
    // TODO: C is disabled
    static_assert(CoreModel::EXT_C);

    core.write_xreg(xd, npc.read());
    npc.jump(target);

    return ExecResult::ok();
}

static ExecResult execute_JALR(CoreModel& core, Inst inst, NextPc& npc) {
    XRegIdx xd = inst.xd();
    XRegIdx xs1 = inst.xs1();
    uint32_t src1 = core.xreg(xs1);

    uint32_t target = (src1 + (uint32_t)inst.imm_i()) & ~(uint32_t)1;

    // target address is always aligned with ext C
    // TODO: C is disabled
    static_assert(CoreModel::EXT_C);

    core.write_xreg(xd, npc.read());
    npc.jump(target);

    return ExecResult::ok();
}

#define EXECUTE_BEQ(core, inst, mem, npc) execute_branch(core, inst, BranchOp::EQ, npc)
#define EXECUTE_BNE(core, inst, mem, npc) execute_branch(core, inst, BranchOp::NEQ, npc)
#define EXECUTE_BLT(core, inst, mem, npc) execute_branch(core, inst, BranchOp::LT_S, npc)
#define EXECUTE_BGE(core, inst, mem, npc) execute_branch(core, inst, BranchOp::GE_S, npc)
#define EXECUTE_BLTU(core, inst, mem, npc) execute_branch(core, inst, BranchOp::LT_U, npc)
#define EXECUTE_BGEU(core, inst, mem, npc) execute_branch(core, inst, BranchOp::GE_U, npc)

#define EXECUTE_JAL(core, inst, mem, npc) execute_JAL(core, inst, npc)
#define EXECUTE_JALR(core, inst, mem, npc) execute_JALR(core, inst, npc)
