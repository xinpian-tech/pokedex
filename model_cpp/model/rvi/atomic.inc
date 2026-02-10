#include "../model_util.h"

static ExecResult execute_LR_W(CoreModel& core, Inst inst, MemCallback mem) {
    XRegIdx xd = inst.xd();
    XRegIdx xs1 = inst.xs1();
    uint32_t addr = core.xreg(xs1);

    uint32_t val;
    if (mem.lr_u32(addr, &val) != 0)
        return ExecResult::exception(ExecResult::XCPT_CODE_LOAD_ACCESS, addr);
    core.write_xreg(xd, val);

    return ExecResult::ok();
}

static ExecResult execute_SC_W(CoreModel& core, Inst inst, MemCallback mem) {
    XRegIdx xd = inst.xd();
    XRegIdx xs1 = inst.xs1();
    XRegIdx xs2 = inst.xs2();
    uint32_t addr = core.xreg(xs1);
    uint32_t src2 = core.xreg(xs2);

    uint32_t result;
    if (mem.sc_u32(addr, src2, &result) != 0)
        return ExecResult::exception(ExecResult::XCPT_CODE_STORE_ACCESS, addr);
    core.write_xreg(xd, result);

    return ExecResult::ok();
}

static ExecResult execute_AMO_W(CoreModel& core, Inst inst, MemCallback mem, uint8_t op) {
    XRegIdx xd = inst.xd();
    XRegIdx xs1 = inst.xs1();
    XRegIdx xs2 = inst.xs2();
    uint32_t addr = core.xreg(xs1);
    uint32_t src2 = core.xreg(xs2);

    uint32_t old;
    if (mem.amo_u32(addr, op, src2, &old) != 0)
        return ExecResult::exception(ExecResult::XCPT_CODE_STORE_ACCESS, addr);
    core.write_xreg(xd, old);

    return ExecResult::ok();
}

#define EXECUTE_LR_W(core, inst, mem, npc) execute_LR_W(core, inst, mem)
#define EXECUTE_SC_W(core, inst, mem, npc) execute_SC_W(core, inst, mem)
#define EXECUTE_AMOSWAP_W(core, inst, mem, npc) execute_AMO_W(core, inst, mem, POKEDEX_AMO_SWAP)
#define EXECUTE_AMOADD_W(core, inst, mem, npc) execute_AMO_W(core, inst, mem, POKEDEX_AMO_ADD)
#define EXECUTE_AMOXOR_W(core, inst, mem, npc) execute_AMO_W(core, inst, mem, POKEDEX_AMO_XOR)
#define EXECUTE_AMOAND_W(core, inst, mem, npc) execute_AMO_W(core, inst, mem, POKEDEX_AMO_AND)
#define EXECUTE_AMOOR_W(core, inst, mem, npc) execute_AMO_W(core, inst, mem, POKEDEX_AMO_OR)
#define EXECUTE_AMOMIN_W(core, inst, mem, npc) execute_AMO_W(core, inst, mem, POKEDEX_AMO_MIN)
#define EXECUTE_AMOMAX_W(core, inst, mem, npc) execute_AMO_W(core, inst, mem, POKEDEX_AMO_MAX)
#define EXECUTE_AMOMINU_W(core, inst, mem, npc) execute_AMO_W(core, inst, mem, POKEDEX_AMO_MINU)
#define EXECUTE_AMOMAXU_W(core, inst, mem, npc) execute_AMO_W(core, inst, mem, POKEDEX_AMO_MAXU)
