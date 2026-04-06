#include "../model_util.h"

namespace {

static void rvv_write_vtype_vl(CoreModel& core, VtypeVl const& vtype_vl) {
    core.m_vtype_vl = vtype_vl;
    core.trace->csr_write(0xc21);
    core.trace->csr_write(0xc20);
}

static ExecResult execute_vsetvl_common(CoreModel& core, XRegIdx rd, XRegIdx rs1, uint32_t new_vtype) {
    REQUIRE_VECTOR(core)

    VtypeVl vtype_vl = core.m_vtype_vl;
    if (!vtype_vl.set_vtype_bits(new_vtype)) {
        rvv_write_vtype_vl(core, vtype_vl);
    } else if (rd.is_zero() && rs1.is_zero()) {
        if (!core.m_vtype_vl.is_ill() &&
            core.m_vtype_vl.vlmax(core.VLEN) == vtype_vl.vlmax(core.VLEN)) {
            vtype_vl.set_vl(core.m_vtype_vl.vl());
            rvv_write_vtype_vl(core, vtype_vl);
        } else {
            vtype_vl.set_ill();
            rvv_write_vtype_vl(core, vtype_vl);
        }
    } else {
        uint32_t avl = rs1.is_zero() ? UINT32_MAX : core.xreg(rs1);
        uint32_t vlmax = vtype_vl.vlmax(core.VLEN);
        vtype_vl.set_vl(avl < vlmax ? avl : vlmax);
        rvv_write_vtype_vl(core, vtype_vl);
    }

    core.write_xreg(rd, core.m_vtype_vl.vl());
    core.clear_vstart();
    core.mark_dirty_vs();
    return ExecResult::ok();
}

}

static ExecResult execute_VSETVLI(CoreModel& core, Inst inst) {
    return execute_vsetvl_common(core, inst.xd(), inst.xs1(), (inst.data >> 20) & 0x7ff);
}

static ExecResult execute_VSETIVLI(CoreModel& core, Inst inst) {
    XRegIdx rd = inst.xd();
    uint32_t avl = inst.zimm_rs1();
    uint32_t new_vtype = (inst.data >> 20) & 0x03ff;

    REQUIRE_VECTOR(core)

    VtypeVl vtype_vl;
    if (!vtype_vl.set_vtype_bits(new_vtype)) {
        rvv_write_vtype_vl(core, vtype_vl);
    } else {
        uint32_t vlmax = vtype_vl.vlmax(core.VLEN);
        vtype_vl.set_vl(avl < vlmax ? avl : vlmax);
        rvv_write_vtype_vl(core, vtype_vl);
    }

    core.write_xreg(rd, core.m_vtype_vl.vl());
    core.clear_vstart();
    core.mark_dirty_vs();
    return ExecResult::ok();
}

static ExecResult execute_VSETVL(CoreModel& core, Inst inst) {
    return execute_vsetvl_common(core, inst.xd(), inst.xs1(), core.xreg(inst.xs2()));
}

#define EXECUTE_VSETVLI(core, inst, mem, npc) execute_VSETVLI(core, inst)
#define EXECUTE_VSETIVLI(core, inst, mem, npc) execute_VSETIVLI(core, inst)
#define EXECUTE_VSETVL(core, inst, mem, npc) execute_VSETVL(core, inst)
