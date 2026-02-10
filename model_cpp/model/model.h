#pragma once

#include "util_pokedex.h"
#include "util_model.h"

struct TraceBuffer;
struct CoreModel: public ModelHelper<CoreModel> {
    static constexpr int XLEN = 32;
    static constexpr int FLEN = 32;
    static constexpr bool EXT_C = true;
    static constexpr bool EXT_V = true;
    static constexpr bool M_MODE = true;
    static constexpr bool S_MODE = false;
    static constexpr bool U_MODE = false;
    const int VLEN;

    const pokedex_model_description MODEL_DESCRIPTION;

    TraceBuffer* trace = nullptr;

    CoreModel(int vlen):
        VLEN(vlen),
        MODEL_DESCRIPTION {
            .model_isa = "rv32imac_zve32_zvl256",
            .model_priv = "M",
            .xlen = 32,
            .flen = 32,
            .vlen = (uint32_t)vlen,
        }
    {
        assert(vlen == 256);
    }

    ////////////////
    // Arch State //
    ////////////////

    uint32_t m_pc;
    uint32_t m__xrf[32];
    uint32_t m__frf[32];

    uint8_t m_frm;
    uint8_t m_fflags;

    uint8_t m_vxrm;
    bool m_vxsat;

    PrivMode m_priv;

    PrivMode m_mstatus_mpp;
    bool m_mstatus_mpie;
    bool m_mstatus_mie;
    uint8_t m_mstatus_vs;
    uint8_t m_mstatus_fs;

    uint32_t m_mscratch;
    uint32_t m_mtvec;
    uint32_t m_mepc;
    uint32_t m_mcause;
    uint32_t m_mtval;

    ///////////////
    // Accessors //
    ///////////////

    uint32_t pc() const { return m_pc; }
    uint32_t xreg(XRegIdx xs) const { return m__xrf[xs.idx]; }
    F32 freg(FRegIdx fs) const { return { .bits = m__frf[fs.idx] }; }
    uint32_t csr(CsrIdx csr_idx) const { return do_csr_inspect(*this, csr_idx); }

    /////////////
    // Helpers //
    /////////////

    bool is_fp_enabled() const { return m_mstatus_fs != 0; }
    bool is_vector_enabled() const { return m_mstatus_vs != 0; }
    bool is_atleast_M() const { return m_priv == PrivMode::M; }
    uint8_t _get_frm() const { return m_frm; }

    //////////////
    // Updators //
    //////////////

    void write_xreg(XRegIdx xd, uint32_t value) {
        if (!xd.is_zero()) {
            m__xrf[xd.idx] = value;
            trace->xreg_write(xd);
        }
    }

    void write_freg(FRegIdx fd, F32 value) {
        m__frf[fd.idx] = value.bits;
        trace->freg_write(fd);
    }

    void accure_fflags(uint8_t fflags) {
        assert(fflags <= 0x1f);
        m_fflags |= fflags;
        if (fflags != 0) {
            trace->csr_write(0x003); // FCSR
        }
    }

    void mark_dirty_fs() {
        if (m_mstatus_fs != 0x03) {
            m_mstatus_fs = 0x03;
            trace->csr_write(0x300); // MSTATUS
        }
    }

    void mark_dirty_vs() {
        if (m_mstatus_vs != 0x03) {
            m_mstatus_vs = 0x03;
            trace->csr_write(0x300); // MSTATUS
        }
    }

    //////////////////////
    // Member functions //
    //////////////////////

    void reset(uint32_t reset_vector) {
        m_pc = reset_vector;
        for (auto& xd: m__xrf) xd = 0;
        for (auto& fd: m__frf) fd = 0;

        m_frm = 0;
        m_fflags = 0;

        m_priv = PrivMode::M;
        m_mstatus_mpp = LEAST_PRIV_MODE;
        m_mstatus_mie = false;
        m_mstatus_mpie = false;
        m_mstatus_fs = false;
        m_mstatus_vs = false;
        m_mscratch = 0;
        m_mtvec = 0;
        m_mepc = 0;
        m_mcause = 0;
        m_mtval = 0;
    }

    StepResult step_trace(MemCallback mem, TraceBuffer* tb) {
        trace = tb;

        trace->begin_trace(m_pc);
        StepResult res = do_step(*this, mem);
        trace->end_trace(res);

        trace = nullptr;

        return res;
    }

    friend uint32_t do_csr_inspect(CoreModel const& core, CsrIdx csr_idx);
    friend StepResult do_step(CoreModel& core, MemCallback mem);
};

[[noreturn]]
inline void todo(const char* message) {
    fprintf(stderr, "todo: %s\n", message);
    abort();
}
