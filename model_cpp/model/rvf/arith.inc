#include "../model_util.h"

static ExecResult execute_f32_alu(CoreModel& core, Inst inst, FpAluOp op) {
    RoundingMode frm;
    
    REQUIRE_FP(core)
    REQUIRE_FRM_RESOLVE(frm, core, inst.rm())

    FRegIdx fd = inst.fd();
    FRegIdx fs1 = inst.fs1();
    FRegIdx fs2 = inst.fs2();
    F32 src1 = core.freg(fs1);
    F32 src2 = core.freg(fs2);

    WithFlag<F32> result = fp_compute_f32(op, src1, src2, frm);

    core.write_freg(fd, result.value);
    core.accure_fflags(result.fflags);
    core.mark_dirty_fs();
    return ExecResult::ok();
}

static ExecResult execute_f32_fmadd(CoreModel& core, Inst inst, bool inv_prod, bool inv_rs3) {
    RoundingMode frm;

    REQUIRE_FP(core)
    REQUIRE_FRM_RESOLVE(frm, core, inst.rm())

    FRegIdx fd = inst.fd();
    FRegIdx fs1 = inst.fs1();
    FRegIdx fs2 = inst.fs2();
    FRegIdx fs3 = inst.fs3();
    F32 src1 = core.freg(fs1);
    F32 src2 = core.freg(fs2);
    F32 src3 = core.freg(fs3);

    if (inv_prod) {
        src1.bits ^= 0x80000000u;
    }
    if (inv_rs3) {
        src3.bits ^= 0x80000000u;
    }

    WithFlag<F32> result = F32::fmadd(src1, src2, src3, frm);

    core.write_freg(fd, result.value);
    core.accure_fflags(result.fflags);
    core.mark_dirty_fs();
    return ExecResult::ok();
}

static ExecResult execute_f32_fsqrt(CoreModel& core, Inst inst) {
    RoundingMode frm;
    
    REQUIRE_FP(core)
    REQUIRE_FRM_RESOLVE(frm, core, inst.rm())

    FRegIdx fd = inst.fd();
    FRegIdx fs1 = inst.fs1();
    F32 src1 = core.freg(fs1);

    WithFlag<F32> result = F32::fsqrt(src1, frm);

    core.write_freg(fd, result.value);
    core.accure_fflags(result.fflags);
    core.mark_dirty_fs();
    return ExecResult::ok();
}

static ExecResult execute_f32_copysign(CoreModel& core, Inst inst, F32 (*op)(F32, F32)) {
    REQUIRE_FP(core)

    FRegIdx fd = inst.fd();
    FRegIdx fs1 = inst.fs1();
    FRegIdx fs2 = inst.fs2();
    F32 src1 = core.freg(fs1);
    F32 src2 = core.freg(fs2);

    F32 res = op(src1, src2);

    core.write_freg(fd, res);
    core.mark_dirty_fs();
    return ExecResult::ok();
}

static ExecResult execute_f32_minmax(CoreModel& core, Inst inst, WithFlag<F32> (*op)(F32, F32)) {
    REQUIRE_FP(core)

    FRegIdx fd = inst.fd();
    FRegIdx fs1 = inst.fs1();
    FRegIdx fs2 = inst.fs2();
    F32 src1 = core.freg(fs1);
    F32 src2 = core.freg(fs2);

    WithFlag<F32> result = op(src1, src2);

    core.write_freg(fd, result.value);
    core.accure_fflags(result.fflags);
    core.mark_dirty_fs();
    return ExecResult::ok();
}

static ExecResult execute_f32_compare(CoreModel& core, Inst inst, WithFlag<bool> (*op)(F32, F32)) {
    REQUIRE_FP(core)

    XRegIdx rd = inst.xd();
    FRegIdx fs1 = inst.fs1();
    FRegIdx fs2 = inst.fs2();
    F32 src1 = core.freg(fs1);
    F32 src2 = core.freg(fs2);

    WithFlag<bool> result = op(src1, src2);

    core.write_xreg(rd, uint32_t(result.value));
    core.accure_fflags(result.fflags);
    core.mark_dirty_fs();
    return ExecResult::ok();
}

#define EXECUTE_FADD_S(core, inst, mem, npc) execute_f32_alu(core, inst, FpAluOp::Add)
#define EXECUTE_FSUB_S(core, inst, mem, npc) execute_f32_alu(core, inst, FpAluOp::Sub)
#define EXECUTE_FMUL_S(core, inst, mem, npc) execute_f32_alu(core, inst, FpAluOp::Mul)
#define EXECUTE_FDIV_S(core, inst, mem, npc) execute_f32_alu(core, inst, FpAluOp::Div)
#define EXECUTE_FMADD_S(core, inst, mem, npc) execute_f32_fmadd(core, inst, false, false)
#define EXECUTE_FMSUB_S(core, inst, mem, npc) execute_f32_fmadd(core, inst, false, true)
#define EXECUTE_FNMSUB_S(core, inst, mem, npc) execute_f32_fmadd(core, inst, true, false)
#define EXECUTE_FNMADD_S(core, inst, mem, npc) execute_f32_fmadd(core, inst, true, true)
#define EXECUTE_FSQRT_S(core, inst, mem, npc) execute_f32_fsqrt(core, inst)
#define EXECUTE_FSGNJ_S(core, inst, mem, npc) execute_f32_copysign(core, inst, F32::copysign)
#define EXECUTE_FSGNJN_S(core, inst, mem, npc) execute_f32_copysign(core, inst, F32::copysign_neg)
#define EXECUTE_FSGNJX_S(core, inst, mem, npc) execute_f32_copysign(core, inst, F32::copysign_xor)
#define EXECUTE_FMIN_S(core, inst, mem, npc) execute_f32_minmax(core, inst, &F32::fmin)
#define EXECUTE_FMAX_S(core, inst, mem, npc) execute_f32_minmax(core, inst, &F32::fmax)
#define EXECUTE_FEQ_S(core, inst, mem, npc) execute_f32_compare(core, inst, &F32::feq)
#define EXECUTE_FLT_S(core, inst, mem, npc) execute_f32_compare(core, inst, &F32::flt)
#define EXECUTE_FLE_S(core, inst, mem, npc) execute_f32_compare(core, inst, &F32::fle)
