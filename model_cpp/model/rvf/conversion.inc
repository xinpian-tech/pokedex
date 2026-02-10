#include "../model_util.h"

static ExecResult execute_f32_to_int(CoreModel& core, Inst inst, bool is_signed) {
    RoundingMode frm;

    REQUIRE_FP(core)
    REQUIRE_FRM_RESOLVE(frm, core, inst.rm())

    XRegIdx rd = inst.xd();
    FRegIdx fs1 = inst.fs1();
    F32 src1 = core.freg(fs1);

    WithFlag<uint32_t> result = is_signed
        ? F32::to_i32(src1, frm)
        : F32::to_u32(src1, frm);

    core.write_xreg(rd, result.value);
    core.accure_fflags(result.fflags);
    core.mark_dirty_fs();
    return ExecResult::ok();
}

static ExecResult execute_f32_from_int(CoreModel& core, Inst inst, bool is_signed) {
    RoundingMode frm;

    REQUIRE_FP(core)
    REQUIRE_FRM_RESOLVE(frm, core, inst.rm())

    FRegIdx fd = inst.fd();
    XRegIdx xs1 = inst.xs1();
    uint32_t src1 = core.xreg(xs1);

    WithFlag<F32> result = is_signed
        ? F32::from_i32(src1, frm)
        : F32::from_u32(src1, frm);

    core.write_freg(fd, result.value);
    core.accure_fflags(result.fflags);
    core.mark_dirty_fs();
    return ExecResult::ok();
}

#define EXECUTE_FCVT_W_S(core, inst, mem, npc) execute_f32_to_int(core, inst, true)
#define EXECUTE_FCVT_WU_S(core, inst, mem, npc) execute_f32_to_int(core, inst, false)
#define EXECUTE_FCVT_S_W(core, inst, mem, npc) execute_f32_from_int(core, inst, true)
#define EXECUTE_FCVT_S_WU(core, inst, mem, npc) execute_f32_from_int(core, inst, false)
