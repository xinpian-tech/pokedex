#include "../model_util.h"

static ExecResult execute_FENCE([[maybe_unused]] CoreModel& core, [[maybe_unused]] Inst inst) {
    return ExecResult::ok();
}

static ExecResult execute_FENCE_I([[maybe_unused]] CoreModel& core, [[maybe_unused]] Inst inst) {
    return ExecResult::ok();
}

static ExecResult execute_ECALL([[maybe_unused]] CoreModel& core, [[maybe_unused]] Inst inst) {
    return ExecResult::exception(ExecResult::XCPT_CODE_MACHINE_ECALL, core.pc());
}

static ExecResult execute_EBREAK([[maybe_unused]] CoreModel& core, [[maybe_unused]] Inst inst) {
    return ExecResult::exception(ExecResult::XCPT_CODE_BREAKPOINT, core.pc());
}

static ExecResult execute_WFI([[maybe_unused]] CoreModel& core, [[maybe_unused]] Inst inst) {
    REQUIRE_PRIV_M(core);

    // TODO: WFI

    return ExecResult::ok();
}

static ExecResult execute_MRET([[maybe_unused]] CoreModel& core, [[maybe_unused]] Inst inst, NextPc& npc) {
    REQUIRE_PRIV_M(core);

    assert(CoreModel::is_pc_aligned(core.m_mepc));
    assert(CoreModel::is_valid_priv_mode(core.m_mstatus_mpp));

    core.m_priv = core.m_mstatus_mpp;
    core.m_mstatus_mpp = CoreModel::LEAST_PRIV_MODE;

    core.m_mstatus_mie = core.m_mstatus_mpie;
    core.m_mstatus_mpie = true;

    npc.jump(core.m_mepc);

    core.trace.mstatus_write();

    return ExecResult::ok();
}

#define EXECUTE_FENCE(core, inst, mem, npc) execute_FENCE(core, inst)
#define EXECUTE_FENCE_I(core, inst, mem, npc) execute_FENCE_I(core, inst)
#define EXECUTE_ECALL(core, inst, mem, npc) execute_ECALL(core, inst)
#define EXECUTE_EBREAK(core, inst, mem, npc) execute_EBREAK(core, inst)
#define EXECUTE_WFI(core, inst, mem, npc) execute_WFI(core, inst)
#define EXECUTE_MRET(core, inst, mem, npc) execute_MRET(core, inst, npc)
