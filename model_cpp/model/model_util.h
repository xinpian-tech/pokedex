#pragma once

#include "model.h"

enum class CsrOp {
    READ_WRITE,
    READ_CLEAR,
    READ_SET,
};

ExecResult do_execute(CoreModel& core, Inst inst, MemCallback mem, NextPc& npc);
ExecResult do_execute_c(CoreModel& core, CInst inst, MemCallback mem, NextPc& npc);
ExecResult do_csr_read(CoreModel& core, CsrIdx csr, uint32_t* ret);
ExecResult do_csr_write(CoreModel& core, CsrIdx csr, uint32_t value);
ExecResult do_csr_rmw(CoreModel& core, CsrIdx csr, CsrOp op, uint32_t value, uint32_t* ret);
uint32_t do_csr_inspect(CoreModel const& core, CsrIdx csr_idx);

#define REQUIRE_PRIV_M(core) if (!(core).is_atleast_M()) { return ExecResult::illegal_inst(); }

[[noreturn]]
inline uint32_t unimpl_csr_read(const char* name) {
    fprintf(stderr, "unimplemented csr read: %s\n", name);
    abort();
}

[[noreturn]]
inline void unimpl_csr_write(const char* name) {
    fprintf(stderr, "unimplemented csr write: %s\n", name);
    abort();
}
