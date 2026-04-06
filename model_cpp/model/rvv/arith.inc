#include "../model_util.h"

namespace {

static ExecResult rvv_require_vop(CoreModel& core, VRegIdx vd, VRegIdx vs2, bool vm) {
    REQUIRE_VECTOR(core)
    REQUIRE_VTYPE_VALID(core)
    REQUIRE_VSTART_ZERO(core)

    uint32_t align = core.m_vtype_vl.align();
    if (!vm && vd.idx == 0) {
        return ExecResult::illegal_inst();
    }
    if ((vd.idx % align) != 0) {
        return ExecResult::illegal_inst();
    }
    if ((vs2.idx % align) != 0) {
        return ExecResult::illegal_inst();
    }

    return ExecResult::ok();
}

static uint32_t rvv_vd_mask(VRegIdx vd, uint32_t align) {
    return ((1u << align) - 1) << vd.idx;
}

static ExecResult rvv_require_vmv(CoreModel& core, VRegIdx vd) {
    REQUIRE_VECTOR(core)
    REQUIRE_VTYPE_VALID(core)
    REQUIRE_VSTART_ZERO(core)

    uint32_t align = core.m_vtype_vl.align();
    if ((vd.idx % align) != 0) {
        return ExecResult::illegal_inst();
    }

    return ExecResult::ok();
}

template<typename T, typename Src1Fn>
static ExecResult execute_vadd_base(CoreModel& core, VRegIdx vd, VRegIdx vs2, bool vm, Src1Fn&& read_src1) {
    ExecResult res = rvv_require_vop(core, vd, vs2, vm);
    if (!res.is_ok()) return res;

    uint32_t vl = core.m_vtype_vl.vl();
    for (uint32_t idx = 0; idx < vl; ++idx) {
        if (vm || core.m_vrf.read_v0_mask(idx)) {
            T src2 = core.m_vrf.read_elem<T>(vs2, idx);
            T src1 = read_src1(idx);
            core.m_vrf.write_elem<T>(vd, idx, T(src2 + src1));
        }
    }

    core.trace->vreg_write_multiple(rvv_vd_mask(vd, core.m_vtype_vl.align()));
    core.mark_dirty_vs();
    core.clear_vstart();
    return ExecResult::ok();
}

static ExecResult execute_vadd_vv(CoreModel& core, Inst inst) {
    VRegIdx vd = inst.vd();
    VRegIdx vs2 = inst.vs2();
    VRegIdx vs1 = inst.vs1();
    bool vm = inst.vm();

    ExecResult res = rvv_require_vop(core, vd, vs2, vm);
    if (!res.is_ok()) return res;
    if ((vs1.idx % core.m_vtype_vl.align()) != 0) {
        return ExecResult::illegal_inst();
    }

    switch (core.m_vtype_vl.sew()) {
        case 8:
            return execute_vadd_base<uint8_t>(core, vd, vs2, vm, [&](uint32_t idx) {
                return core.m_vrf.read_elem<uint8_t>(vs1, idx);
            });
        case 16:
            return execute_vadd_base<uint16_t>(core, vd, vs2, vm, [&](uint32_t idx) {
                return core.m_vrf.read_elem<uint16_t>(vs1, idx);
            });
        case 32:
            return execute_vadd_base<uint32_t>(core, vd, vs2, vm, [&](uint32_t idx) {
                return core.m_vrf.read_elem<uint32_t>(vs1, idx);
            });
        default:
            return ExecResult::illegal_inst();
    }
}

static ExecResult execute_vadd_vx(CoreModel& core, Inst inst) {
    VRegIdx vd = inst.vd();
    VRegIdx vs2 = inst.vs2();
    XRegIdx rs1 = inst.xs1();
    bool vm = inst.vm();

    switch (core.m_vtype_vl.sew()) {
        case 8: {
            uint8_t src1 = uint8_t(core.xreg(rs1));
            return execute_vadd_base<uint8_t>(core, vd, vs2, vm, [&](uint32_t idx) {
                (void)(idx);
                return src1;
            });
        }
        case 16: {
            uint16_t src1 = uint16_t(core.xreg(rs1));
            return execute_vadd_base<uint16_t>(core, vd, vs2, vm, [&](uint32_t idx) {
                (void)(idx);
                return src1;
            });
        }
        case 32: {
            uint32_t src1 = core.xreg(rs1);
            return execute_vadd_base<uint32_t>(core, vd, vs2, vm, [&](uint32_t idx) {
                (void)(idx);
                return src1;
            });
        }
        default:
            return ExecResult::illegal_inst();
    }
}

static ExecResult execute_vadd_vi(CoreModel& core, Inst inst) {
    VRegIdx vd = inst.vd();
    VRegIdx vs2 = inst.vs2();
    bool vm = inst.vm();

    switch (core.m_vtype_vl.sew()) {
        case 8: {
            uint8_t imm = uint8_t(inst.simm5());
            return execute_vadd_base<uint8_t>(core, vd, vs2, vm, [&](uint32_t idx) {
                (void)(idx);
                return imm;
            });
        }
        case 16: {
            uint16_t imm = uint16_t(inst.simm5());
            return execute_vadd_base<uint16_t>(core, vd, vs2, vm, [&](uint32_t idx) {
                (void)(idx);
                return imm;
            });
        }
        case 32: {
            uint32_t imm = uint32_t(inst.simm5());
            return execute_vadd_base<uint32_t>(core, vd, vs2, vm, [&](uint32_t idx) {
                (void)(idx);
                return imm;
            });
        }
        default:
            return ExecResult::illegal_inst();
    }
}

template<typename T>
static ExecResult execute_vmv_v_i_base(CoreModel& core, VRegIdx vd, T imm) {
    ExecResult res = rvv_require_vmv(core, vd);
    if (!res.is_ok()) return res;

    for (uint32_t idx = 0; idx < core.m_vtype_vl.vl(); ++idx) {
        core.m_vrf.write_elem<T>(vd, idx, imm);
    }

    core.trace->vreg_write_multiple(rvv_vd_mask(vd, core.m_vtype_vl.align()));
    core.mark_dirty_vs();
    core.clear_vstart();
    return ExecResult::ok();
}

static ExecResult execute_vmv_v_i(CoreModel& core, Inst inst) {
    VRegIdx vd = inst.vd();
    int32_t imm = inst.simm5();
    ExecResult res = rvv_require_vmv(core, vd);
    if (!res.is_ok()) return res;

    switch (core.m_vtype_vl.sew()) {
        case 8:
            return execute_vmv_v_i_base<uint8_t>(core, vd, uint8_t(imm));
        case 16:
            return execute_vmv_v_i_base<uint16_t>(core, vd, uint16_t(imm));
        case 32:
            return execute_vmv_v_i_base<uint32_t>(core, vd, uint32_t(imm));
        default:
            return ExecResult::illegal_inst();
    }
}

}

#define EXECUTE_VADD_VV(core, inst, mem, npc) execute_vadd_vv(core, inst)
#define EXECUTE_VADD_VX(core, inst, mem, npc) execute_vadd_vx(core, inst)
#define EXECUTE_VADD_VI(core, inst, mem, npc) execute_vadd_vi(core, inst)
#define EXECUTE_VMV_V_I(core, inst, mem, npc) execute_vmv_v_i(core, inst)
