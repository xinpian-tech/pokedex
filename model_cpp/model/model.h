#pragma once

#include "util_pokedex.h"
#include "util_model.h"

struct TraceBuffer;

// SEW encoding: 0 -> sew=8, 1 -> sew=16, 2 -> sew=32, 3 -> sew=64
// LMUL encoding: 0 -> lmul=1, 3 -> lmul=8, -3 -> lmul=1/8
struct VtypeVl {
    static constexpr uint32_t VILL = 0x80000000u;
    static constexpr uint32_t ELEN = 32u;

    bool m_valid;
    uint8_t m_sew;
    int8_t m_lmul;
    bool m_ta;
    bool m_ma;
    uint32_t m_vl;

    void reset() {
        m_valid = false;
        m_sew = 0;
        m_lmul = 0;
        m_ta = false;
        m_ma = false;
        m_vl = 0;
    }

    void set_ill() {
        reset();
    }

    bool is_ill() const {
        return !m_valid;
    }

    uint32_t vl() const {
        return m_vl;
    }

    void set_vl(uint32_t vl) {
        m_vl = vl;
    }

    uint32_t sew() const {
        assert(m_valid);
        return 8u << m_sew;
    }

    uint32_t vtype_bits() const {
        if (!m_valid) {
            return VILL;
        }

        uint32_t lmul_bits = m_lmul >= 0 ? uint32_t(m_lmul) : uint32_t(m_lmul + 8);
        return (uint32_t(m_ma) << 7) | (uint32_t(m_ta) << 6) | (uint32_t(m_sew) << 3) | lmul_bits;
    }

    uint32_t vlmax(int vlen) const {
        assert(m_valid);

        uint32_t vlmax = uint32_t(vlen) / sew();
        switch (m_lmul) {
            case 0: return vlmax;
            case 1: return vlmax * 2;
            case 2: return vlmax * 4;
            case 3: return vlmax * 8;
            case -1: return vlmax / 2;
            case -2: return vlmax / 4;
            case -3: return vlmax / 8;
            default: assert(false); return 0;
        }
    }

    uint32_t align() const {
        assert(m_valid);

        switch (m_lmul) {
            case 0:
            case -1:
            case -2:
            case -3:
                return 1;
            case 1: return 2;
            case 2: return 4;
            case 3: return 8;
            default: assert(false); return 0;
        }
    }

    static uint32_t log2_sew(uint32_t sew) {
        switch (sew) {
            case 8: return 3;
            case 16: return 4;
            case 32: return 5;
            case 64: return 6;
            default: assert(false); return 0;
        }
    }

    bool eew_align(uint32_t eew, uint32_t* align_out) const {
        assert(m_valid);

        int32_t log2_emul = m_lmul + int32_t(log2_sew(eew)) - int32_t(log2_sew(sew()));
        switch (log2_emul) {
            case 0:
            case -1:
            case -2:
            case -3:
                *align_out = 1;
                return true;
            case 1:
                *align_out = 2;
                return true;
            case 2:
                *align_out = 4;
                return true;
            case 3:
                *align_out = 8;
                return true;
            default:
                *align_out = 1;
                return false;
        }
    }

    static bool is_ill_bits(uint32_t bits) {
        if ((bits >> 8) != 0) {
            return true;
        }

        uint32_t sew = (bits >> 3) & 0x07;
        if (sew > 0x03) {
            return true;
        }

        uint32_t lmul_bits = bits & 0x07;
        if (lmul_bits == 0x04) {
            return true;
        }

        int8_t lmul = lmul_bits < 0x04 ? int8_t(lmul_bits) : int8_t(lmul_bits) - 8;
        if (lmul < 0 && ((8u << sew) << (-lmul)) > ELEN) {
            return true;
        }

        return false;
    }

    bool set_vtype_bits(uint32_t bits) {
        if (is_ill_bits(bits)) {
            set_ill();
            return false;
        }

        m_valid = true;
        m_sew = (bits >> 3) & 0x07;
        m_lmul = (bits & 0x07) < 0x04 ? int8_t(bits & 0x07) : int8_t(bits & 0x07) - 8;
        m_ta = (bits >> 6) & 0x01;
        m_ma = (bits >> 7) & 0x01;
        return true;
    }
};

struct Vrf {
    std::byte* _data;
    const int VLEN;

    explicit Vrf(int vlen): VLEN(vlen) {
        _data = (std::byte*)aligned_alloc(16, vlen * 4);
        assert(_data != nullptr);
    }

    ~Vrf() {
        free(_data);
    }

    void reset() {
        memset(_data, 0, VLEN * 4);
    }

    size_t bytes_per_reg() const {
        return size_t(VLEN) / 8;
    }

    template<typename T>
    T read_elem(VRegIdx reg, uint32_t elem_idx) const {
        T value;
        size_t offset = size_t(reg.idx) * bytes_per_reg() + size_t(elem_idx) * sizeof(T);
        assert(offset + sizeof(T) <= size_t(VLEN) * 4);
        memcpy(&value, _data + offset, sizeof(T));
        return value;
    }

    template<typename T>
    void write_elem(VRegIdx reg, uint32_t elem_idx, T value) {
        size_t offset = size_t(reg.idx) * bytes_per_reg() + size_t(elem_idx) * sizeof(T);
        assert(offset + sizeof(T) <= size_t(VLEN) * 4);
        memcpy(_data + offset, &value, sizeof(T));
    }

    bool read_v0_mask(uint32_t elem_idx) const {
        size_t offset = elem_idx >> 3;
        assert(offset < bytes_per_reg());
        uint8_t value;
        memcpy(&value, _data + offset, sizeof(value));
        return ((value >> (elem_idx & 0x07)) & 0x01) != 0;
    }
};

struct CoreModel: public ModelHelper<CoreModel> {
    static constexpr int XLEN = 32;
    static constexpr int FLEN = 32;
    static constexpr bool EXT_C = true;
    static constexpr bool EXT_V = true;
    static constexpr bool M_MODE = true;
    static constexpr bool S_MODE = false;
    static constexpr bool U_MODE = false;
    const int VLEN;

    TraceBuffer* trace = nullptr;

    explicit CoreModel(int vlen):
        VLEN(vlen),
        m_vrf(vlen)
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

    VtypeVl m_vtype_vl;
    uint32_t m_vstart;
    uint8_t m_vxrm;
    bool m_vxsat;
    Vrf m_vrf;

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

    void clear_vstart() {
        if (m_vstart != 0) {
            m_vstart = 0;
            trace->csr_write(0x008);// VSTART
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

        m_vtype_vl.reset();
        m_vstart = 0;
        m_vxrm = 0;
        m_vxsat = false;
        m_vrf.reset();

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
