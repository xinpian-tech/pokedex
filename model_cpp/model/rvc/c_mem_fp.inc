#include "../model_util.h"

static ExecResult execute_C_FLW(CoreModel& core, CInst inst, MemCallback mem) {
    uint32_t addr = core.xreg(inst.rs1_c()) + inst.cl_uimm();
    uint32_t val;
    if (mem.read<uint32_t>(addr, &val) != 0)
        return ExecResult::exception(ExecResult::XCPT_CODE_LOAD_ACCESS, addr);
    core.write_freg(inst.fd_c(), { .bits = val });
    return ExecResult::ok();
}

static ExecResult execute_C_FSW(CoreModel& core, CInst inst, MemCallback mem) {
    uint32_t addr = core.xreg(inst.rs1_c()) + inst.cl_uimm();
    uint32_t value = core.freg(inst.fs2_c()).bits;
    if (mem.write<uint32_t>(addr, value) != 0)
        return ExecResult::exception(ExecResult::XCPT_CODE_STORE_ACCESS, addr);
    return ExecResult::ok();
}

static ExecResult execute_C_FLWSP(CoreModel& core, CInst inst, MemCallback mem) {
    uint32_t addr = core.xreg(XREG_SP) + inst.ci_lwsp_uimm();
    uint32_t val;
    if (mem.read<uint32_t>(addr, &val) != 0)
        return ExecResult::exception(ExecResult::XCPT_CODE_LOAD_ACCESS, addr);
    core.write_freg(inst.fd(), { .bits = val });
    return ExecResult::ok();
}

static ExecResult execute_C_FSWSP(CoreModel& core, CInst inst, MemCallback mem) {
    uint32_t addr = core.xreg(XREG_SP) + inst.css_swsp_uimm();
    uint32_t value = core.freg(inst.cr_fs2()).bits;
    if (mem.write<uint32_t>(addr, value) != 0)
        return ExecResult::exception(ExecResult::XCPT_CODE_STORE_ACCESS, addr);
    return ExecResult::ok();
}

#define EXECUTE_C_FLW(core, inst, mem, npc) execute_C_FLW(core, inst, mem)
#define EXECUTE_C_FSW(core, inst, mem, npc) execute_C_FSW(core, inst, mem)
#define EXECUTE_C_FLWSP(core, inst, mem, npc) execute_C_FLWSP(core, inst, mem)
#define EXECUTE_C_FSWSP(core, inst, mem, npc) execute_C_FSWSP(core, inst, mem)
