#include "../model_util.h"

static ExecResult execute_C_LW(CoreModel& core, CInst inst, MemCallback mem) {
    uint32_t addr = core.xreg(inst.rs1_c()) + inst.cl_uimm();
    uint32_t val;
    if (mem.read<uint32_t>(addr, &val) != 0)
        return ExecResult::exception(ExecResult::XCPT_CODE_LOAD_ACCESS, addr);
    core.write_xreg(inst.rd_c(), val);
    return ExecResult::ok();
}

static ExecResult execute_C_SW(CoreModel& core, CInst inst, MemCallback mem) {
    uint32_t addr = core.xreg(inst.rs1_c()) + inst.cl_uimm();
    if (mem.write<uint32_t>(addr, core.xreg(inst.rs2_c())) != 0)
        return ExecResult::exception(ExecResult::XCPT_CODE_STORE_ACCESS, addr);
    return ExecResult::ok();
}

static ExecResult execute_C_LWSP(CoreModel& core, CInst inst, MemCallback mem) {
    if (inst.rd().is_zero())
        return ExecResult::illegal_inst();
    uint32_t addr = core.xreg(XREG_SP) + inst.ci_lwsp_uimm();
    uint32_t val;
    if (mem.read<uint32_t>(addr, &val) != 0)
        return ExecResult::exception(ExecResult::XCPT_CODE_LOAD_ACCESS, addr);
    core.write_xreg(inst.rd(), val);
    return ExecResult::ok();
}

static ExecResult execute_C_SWSP(CoreModel& core, CInst inst, MemCallback mem) {
    uint32_t addr = core.xreg(XREG_SP) + inst.css_swsp_uimm();
    if (mem.write<uint32_t>(addr, core.xreg(inst.cr_rs2())) != 0)
        return ExecResult::exception(ExecResult::XCPT_CODE_STORE_ACCESS, addr);
    return ExecResult::ok();
}

#define EXECUTE_C_LW(core, inst, mem, npc) execute_C_LW(core, inst, mem)
#define EXECUTE_C_SW(core, inst, mem, npc) execute_C_SW(core, inst, mem)
#define EXECUTE_C_LWSP(core, inst, mem, npc) execute_C_LWSP(core, inst, mem)
#define EXECUTE_C_SWSP(core, inst, mem, npc) execute_C_SWSP(core, inst, mem)
