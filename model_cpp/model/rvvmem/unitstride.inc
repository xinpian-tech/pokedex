#include "../model_util.h"

template<typename T>
static ExecResult execute_vle(CoreModel& core, Inst inst, MemCallback mem) {
    REQUIRE_VECTOR(core)

    if (core.m_vtype_vl.is_ill()) {
        return ExecResult::illegal_inst();
    }
    if (core.m_vstart > core.m_vtype_vl.vl()) {
        return ExecResult::illegal_inst();
    }

    VRegIdx vd = inst.vd();
    XRegIdx rs1 = inst.xs1();
    bool vm = inst.vm();
    uint32_t align;

    if (!vm && vd.idx == 0) {
        return ExecResult::illegal_inst();
    }
    if (!core.m_vtype_vl.eew_align(sizeof(T) * 8, &align)) {
        return ExecResult::illegal_inst();
    }
    if ((vd.idx % align) != 0) {
        return ExecResult::illegal_inst();
    }

    uint32_t base_addr = core.xreg(rs1);
    for (uint32_t idx = core.m_vstart; idx < core.m_vtype_vl.vl(); ++idx) {
        if (vm || core.m_vrf.read_v0_mask(idx)) {
            uint32_t addr = base_addr + idx * sizeof(T);
            T value;
            if (mem.read<T>(addr, &value) != 0) {
                todo("handle vector load exception");
            }
            core.m_vrf.write_elem<T>(vd, idx, value);
        }
    }

    core.trace->vreg_write_multiple(((1u << align) - 1) << vd.idx);
    core.mark_dirty_vs();
    core.clear_vstart();
    return ExecResult::ok();
}

template<typename T>
static ExecResult execute_vse(CoreModel& core, Inst inst, MemCallback mem) {
    REQUIRE_VECTOR(core)

    if (core.m_vtype_vl.is_ill()) {
        return ExecResult::illegal_inst();
    }
    if (core.m_vstart > core.m_vtype_vl.vl()) {
        return ExecResult::illegal_inst();
    }

    VRegIdx vs3 = inst.vd();
    XRegIdx rs1 = inst.xs1();
    bool vm = inst.vm();
    uint32_t align;

    if (!core.m_vtype_vl.eew_align(sizeof(T) * 8, &align)) {
        return ExecResult::illegal_inst();
    }
    if ((vs3.idx % align) != 0) {
        return ExecResult::illegal_inst();
    }

    uint32_t base_addr = core.xreg(rs1);
    for (uint32_t idx = core.m_vstart; idx < core.m_vtype_vl.vl(); ++idx) {
        if (vm || core.m_vrf.read_v0_mask(idx)) {
            uint32_t addr = base_addr + idx * sizeof(T);
            T value = core.m_vrf.read_elem<T>(vs3, idx);
            if (mem.write<T>(addr, value) != 0) {
                todo("handle vector store exception");
            }
        }
    }

    core.clear_vstart();
    return ExecResult::ok();
}

#define EXECUTE_VLE8_V(core, inst, mem, npc) execute_vle<uint8_t>(core, inst, mem)
#define EXECUTE_VLE16_V(core, inst, mem, npc) execute_vle<uint16_t>(core, inst, mem)
#define EXECUTE_VLE32_V(core, inst, mem, npc) execute_vle<uint32_t>(core, inst, mem)
#define EXECUTE_VSE8_V(core, inst, mem, npc) execute_vse<uint8_t>(core, inst, mem)
#define EXECUTE_VSE16_V(core, inst, mem, npc) execute_vse<uint16_t>(core, inst, mem)
#define EXECUTE_VSE32_V(core, inst, mem, npc) execute_vse<uint32_t>(core, inst, mem)
