#include "../model_util.h"

static ExecResult execute_flw(CoreModel& core, Inst inst, MemCallback mem) {
    REQUIRE_FP(core)

    FRegIdx fd = inst.fd();
    XRegIdx xs1 = inst.xs1();
    uint32_t src1 = core.xreg(xs1);

    uint32_t addr = src1 + (uint32_t)inst.imm_i();

    uint32_t val;
    if (mem.read<uint32_t>(addr, &val) != 0)
        return ExecResult::exception(ExecResult::XCPT_CODE_LOAD_ACCESS, addr);
    
    core.write_freg(fd, { .bits = val });
    core.mark_dirty_fs();

    return ExecResult::ok();
}

static ExecResult execute_fsw(CoreModel& core, Inst inst, MemCallback mem) {
    REQUIRE_FP(core)

    XRegIdx xs1 = inst.xs1();
    FRegIdx fs2 = inst.fs2();
    uint32_t src1 = core.xreg(xs1);
    uint32_t src2 = core.freg(fs2).bits;

    uint32_t addr = src1 + (uint32_t)inst.imm_s();

    if (mem.write<uint32_t>(addr, src2) != 0)
        return ExecResult::exception(ExecResult::XCPT_CODE_STORE_ACCESS, addr);

    return ExecResult::ok();
}

#define EXECUTE_FLW(core, inst, mem, npc) execute_flw(core, inst, mem)
#define EXECUTE_FSW(core, inst, mem, npc) execute_fsw(core, inst, mem)
