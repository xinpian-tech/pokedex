#include "../model_util.h"

static ExecResult execute_CSRRW(CoreModel& core, Inst inst) {
    XRegIdx xd = inst.xd();
    XRegIdx xs1 = inst.xs1();
    CsrIdx csr = inst.csr();
    uint32_t src1 = core.xreg(xs1);

    // Suppress read side effects when xd is x0
    if (xd.is_zero()) {
        return do_csr_write(core, csr, src1);
    } else {
        uint32_t old;
        ExecResult res = do_csr_rmw(core, csr, CsrOp::READ_WRITE, src1, &old);
        if (!res.is_ok()) return res;
        core.write_xreg(xd, old);
        return ExecResult::ok();
    }
}

static ExecResult execute_CSRRWI(CoreModel& core, Inst inst) {
    XRegIdx xd = inst.xd();
    CsrIdx csr = inst.csr();
    uint32_t zimm = inst.zimm_rs1();

    // Suppress read side effects when xd is x0
    if (xd.is_zero()) {
        return do_csr_write(core, csr, zimm);
    } else {
        uint32_t old;
        ExecResult res = do_csr_rmw(core, csr, CsrOp::READ_WRITE, zimm, &old);
        if (!res.is_ok()) return res;
        core.write_xreg(xd, old);
        return ExecResult::ok();
    }
}

static ExecResult execute_CSRRSC(CoreModel& core, Inst inst, CsrOp op) {
    XRegIdx xd = inst.xd();
    XRegIdx xs1 = inst.xs1();
    CsrIdx csr = inst.csr();
    uint32_t old;

    ExecResult res;
    if (xs1.is_zero()) {
        res = do_csr_read(core, csr, &old);
    } else {
        res = do_csr_rmw(core, csr, op, core.xreg(xs1), &old);
    }
    if (!res.is_ok()) return res;
    core.write_xreg(xd, old);
    return ExecResult::ok();
}

static ExecResult execute_CSRRSCI(CoreModel& core, Inst inst, CsrOp op) {
    XRegIdx xd = inst.xd();
    CsrIdx csr = inst.csr();
    uint32_t zimm = inst.zimm_rs1();
    uint32_t old;

    ExecResult res;
    if (zimm == 0) {
        res = do_csr_read(core, csr, &old);
    } else {
        res = do_csr_rmw(core, csr, op, zimm, &old);
    }
    if (!res.is_ok()) return res;
    core.write_xreg(xd, old);
    return ExecResult::ok();
}

#define EXECUTE_CSRRW(core, inst, mem, npc) execute_CSRRW(core, inst)
#define EXECUTE_CSRRS(core, inst, mem, npc) execute_CSRRSC(core, inst, CsrOp::READ_SET)
#define EXECUTE_CSRRC(core, inst, mem, npc) execute_CSRRSC(core, inst, CsrOp::READ_CLEAR)
#define EXECUTE_CSRRWI(core, inst, mem, npc) execute_CSRRWI(core, inst)
#define EXECUTE_CSRRSI(core, inst, mem, npc) execute_CSRRSCI(core, inst, CsrOp::READ_SET)
#define EXECUTE_CSRRCI(core, inst, mem, npc) execute_CSRRSCI(core, inst, CsrOp::READ_CLEAR)
