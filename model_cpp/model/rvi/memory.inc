#include "../model_util.h"

template<typename T, bool is_signed>
uint32_t extend_u32(std::type_identity_t<T> x) {
    if constexpr (std::is_same_v<T, uint8_t>) {
        if (is_signed) {
            return (uint32_t)(int32_t)(int8_t)x;
        } else {
            return x;
        }
    }
    else if constexpr (std::is_same_v<T, uint16_t>) {
        if (is_signed) {
            return (uint32_t)(int16_t)(int16_t)x;
        } else {
            return x;
        }
    }
    else {
        static_assert(std::is_same_v<T, uint32_t>);
        return x;
    }
}

template<typename T, bool is_signed>
static ExecResult execute_load(CoreModel& core, Inst inst, MemCallback mem) {
    XRegIdx xd = inst.xd();
    XRegIdx xs1 = inst.xs1();
    uint32_t src1 = core.xreg(xs1);

    uint32_t addr = src1 + (uint32_t)inst.imm_i();

    T val;
    if (mem.read<T>(addr, &val) != 0)
        return ExecResult::exception(ExecResult::XCPT_CODE_LOAD_ACCESS, addr);

    uint32_t value_ext = extend_u32<T, is_signed>(val);
    core.write_xreg(xd, value_ext);

    return ExecResult::ok();
}

template<typename T>
static ExecResult execute_store(CoreModel& core, Inst inst, MemCallback mem) {
    XRegIdx xs1 = inst.xs1();
    XRegIdx xs2 = inst.xs2();
    uint32_t src1 = core.xreg(xs1);
    uint32_t src2 = core.xreg(xs2);

    uint32_t addr = src1 + (uint32_t)inst.imm_s();
    T value = T(src2); // truncation

    if (mem.write<T>(addr, value) != 0)
        return ExecResult::exception(ExecResult::XCPT_CODE_STORE_ACCESS, addr);

    return ExecResult::ok();
}

#define EXECUTE_LB(core, inst, mem, npc) execute_load<uint8_t, true>(core, inst, mem)
#define EXECUTE_LH(core, inst, mem, npc) execute_load<uint16_t, true>(core, inst, mem)
#define EXECUTE_LW(core, inst, mem, npc) execute_load<uint32_t, true>(core, inst, mem)
#define EXECUTE_LBU(core, inst, mem, npc) execute_load<uint8_t, false>(core, inst, mem)
#define EXECUTE_LHU(core, inst, mem, npc) execute_load<uint16_t, false>(core, inst, mem)

#define EXECUTE_SB(core, inst, mem, npc) execute_store<uint8_t>(core, inst, mem)
#define EXECUTE_SH(core, inst, mem, npc) execute_store<uint16_t>(core, inst, mem)
#define EXECUTE_SW(core, inst, mem, npc) execute_store<uint32_t>(core, inst, mem)
