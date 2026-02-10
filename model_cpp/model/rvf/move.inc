#include "../model_util.h"

static ExecResult execute_fmv_x_w(CoreModel& core, Inst inst) {
    REQUIRE_FP(core)

    XRegIdx xd = inst.xd();
    FRegIdx fs1 = inst.fs1();
    F32 src1 = core.freg(fs1);

    core.write_xreg(xd, src1.bits);

    return ExecResult::ok();
}

static ExecResult execute_fmv_w_x(CoreModel& core, Inst inst) {
    REQUIRE_FP(core)

    FRegIdx fd = inst.fd();
    XRegIdx xs1 = inst.xs1();
    uint32_t src1 = core.xreg(xs1);

    core.write_freg(fd, { .bits = src1 });

    core.mark_dirty_fs();
    return ExecResult::ok();
}

static ExecResult execute_fclass_s(CoreModel& core, Inst inst) {
    REQUIRE_FP(core)

    XRegIdx xd = inst.xd();
    FRegIdx fs1 = inst.fs1();
    F32 src1 = core.freg(fs1);

    uint32_t res = F32::fclass(src1);

    core.write_xreg(xd, res);
    return ExecResult::ok();
}

#define EXECUTE_FMV_X_W(core, inst, mem, npc) execute_fmv_x_w(core, inst)
#define EXECUTE_FMV_W_X(core, inst, mem, npc) execute_fmv_w_x(core, inst)
#define EXECUTE_FCLASS_S(core, inst, mem, npc) execute_fclass_s(core, inst)
