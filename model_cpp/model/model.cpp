#include "model.h"
#include "model_util.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>

[[noreturn]]
static void todo_exec_exception(uint32_t pc, uint32_t inst, ExecResult res) {
    fprintf(
        stderr,
        "todo: handle exec exception at pc=0x%08x inst=0x%08x code=%u payload=0x%08x\n",
        pc,
        inst,
        unsigned(res.m_code),
        res.m_payload
    );
    abort();
}

StepResult do_step(CoreModel& core, MemCallback mem) {
    // TODO: check interrupt

    auto pc = core.m_pc;

    uint16_t inst_lo = 0;
    if (mem.inst_fetch_u16(pc, &inst_lo) != 0) {
        todo("handle fetch exception");
    }

    if ((inst_lo & 0x03) == 0x03) {
        // execute non-compressed instruction

        uint16_t inst_hi = 0;
        if (mem.inst_fetch_u16(pc + 2, &inst_hi) != 0) {
            todo("handle fetch exception");
        }

        Inst inst = Inst::from_lo_hi(inst_lo, inst_hi);
        NextPc npc { pc + 4 };

        ExecResult res = do_execute(core, inst, mem, npc);
        if (res.is_ok()) {
            core.m_pc = npc.m_pc;
            return StepResult::commit(inst);
        } else {
            // if the inst is not commited, PC should not be modified
            assert(pc == core.m_pc);

            todo_exec_exception(pc, inst.data, res);
        }
    } else {
        CInst inst = CInst::from_u16(inst_lo);
        NextPc npc { pc + 2 };

        ExecResult res = do_execute_c(core, inst, mem, npc);
        if (res.is_ok()) {
            core.m_pc = npc.m_pc;
            return StepResult::commit_c(inst);
        } else {
            // if the inst is not commited, PC should not be modified
            assert(pc == core.m_pc);

            todo_exec_exception(pc, inst.data, res);
        }
    }
}
