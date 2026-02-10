#pragma once

#include <cstdint>
#include <cassert>

struct XRegIdx {
    uint8_t idx;

    bool is_zero() const {
        return idx == 0;
    }
};

struct FRegIdx {
    uint8_t idx;
};

struct VRegIdx {
    uint8_t idx;
};

struct CsrIdx {
    uint16_t idx;
};

constexpr XRegIdx XREG_RA = { .idx = 1 };
constexpr XRegIdx XREG_SP = { .idx = 2 };

inline XRegIdx xreg_from_idx(uint8_t idx) {
    return { .idx = idx };
}

inline FRegIdx freg_from_idx(uint8_t idx) {
    return { .idx = idx };
}

inline VRegIdx vreg_from_idx(uint8_t idx) {
    return { .idx = idx };
}

inline CsrIdx csr_from_idx(uint16_t idx) {
    return { .idx = idx };
}

struct Inst {
    uint32_t data;

    static Inst from_u32(uint32_t data) {
        return { .data = data };
    }
    static Inst from_lo_hi(uint16_t lo, uint16_t hi) {
        return { .data = (uint32_t(hi) << 16) | uint32_t(lo) };
    }

    bool match(uint32_t mask, uint32_t base) const {
        return (data & mask) == base;
    }

    // Register indices
    XRegIdx xd()  const { return xreg_from_idx((data >> 7) & 0x1f); }
    XRegIdx xs1() const { return xreg_from_idx((data >> 15) & 0x1f); }
    XRegIdx xs2() const { return xreg_from_idx((data >> 20) & 0x1f); }
    FRegIdx fd()  const { return freg_from_idx((data >> 7) & 0x1f); }
    FRegIdx fs1() const { return freg_from_idx((data >> 15) & 0x1f); }
    FRegIdx fs2() const { return freg_from_idx((data >> 20) & 0x1f); }
    FRegIdx fs3() const { return freg_from_idx((data >> 27) & 0x1f); }
    CsrIdx  csr() const { return csr_from_idx(data >> 20); }

    // I-type immediate: sext(inst[31:20])
    int32_t imm_i() const {
        return (int32_t)data >> 20;
    }

    // TODO : review
    // S-type immediate: sext({inst[31:25], inst[11:7]})
    int32_t imm_s() const {
        uint32_t lo = (data >> 7) & 0x1f;
        uint32_t hi = (data >> 25);
        uint32_t raw = (hi << 5) | lo;
        return (int32_t)(raw << 20) >> 20;
    }

    // TODO : review
    // B-type immediate: sext({inst[31], inst[7], inst[30:25], inst[11:8], 0})
    int32_t imm_b() const {
        uint32_t b11  = (data >> 7) & 1;
        uint32_t b4_1 = (data >> 8) & 0xf;
        uint32_t b10_5 = (data >> 25) & 0x3f;
        uint32_t b12  = (data >> 31) & 1;
        uint32_t raw = (b12 << 12) | (b11 << 11) | (b10_5 << 5) | (b4_1 << 1);
        return (int32_t)(raw << 19) >> 19;
    }

    // U-type immediate: {inst[31:12], 12'b0}
    int32_t imm_u() const {
        return (int32_t)(data & 0xfffff000);
    }

    // TODO : review
    // J-type immediate: sext({inst[31], inst[19:12], inst[20], inst[30:21], 0})
    int32_t imm_j() const {
        uint32_t b19_12 = (data >> 12) & 0xff;
        uint32_t b11    = (data >> 20) & 1;
        uint32_t b10_1  = (data >> 21) & 0x3ff;
        uint32_t b20    = (data >> 31) & 1;
        uint32_t raw = (b20 << 20) | (b19_12 << 12) | (b11 << 11) | (b10_1 << 1);
        return (int32_t)(raw << 11) >> 11;
    }

    // Shift amount: bits[24:20]
    uint8_t shamt5() const {
        return (data >> 20) & 0x1f;
    }

    // CSR zimm (zero-extended): bits[19:15]
    uint32_t zimm_rs1() const {
        return (data >> 15) & 0x1f;
    }
};

struct CInst {
    uint16_t data;

    static CInst from_u16(uint16_t data) {
        return { .data = data };
    }

    bool match(uint16_t mask, uint16_t base) const {
        return (data & mask) == base;
    }

    // TODO : review
    // Full 5-bit register indices (CR/CI/CSS formats)
    XRegIdx rd()     const { return xreg_from_idx((data >> 7) & 0x1f); }
    XRegIdx cr_rs2() const { return xreg_from_idx((data >> 2) & 0x1f); }

    // TODO : review
    // Compressed 3-bit register indices (CIW/CL/CS/CA/CB, mapped to x8-x15)
    XRegIdx rd_c()  const { return xreg_from_idx(((data >> 2) & 0x7) + 8); }
    XRegIdx rs1_c() const { return xreg_from_idx(((data >> 7) & 0x7) + 8); }
    XRegIdx rs2_c() const { return xreg_from_idx(((data >> 2) & 0x7) + 8); }

    // TODO : review
    // c.addi4spn: nzuimm[5:4|9:6|2|3]
    uint32_t ciw_uimm() const {
        uint32_t b3  = (data >> 5) & 1;
        uint32_t b2  = (data >> 6) & 1;
        uint32_t b9_6 = (data >> 7) & 0xf;
        uint32_t b5_4 = (data >> 11) & 0x3;
        return (b9_6 << 6) | (b5_4 << 4) | (b3 << 3) | (b2 << 2);
    }

    // TODO : review
    // c.lw/c.sw: uimm[5:3|2|6]
    uint32_t cl_uimm() const {
        uint32_t b6  = (data >> 5) & 1;
        uint32_t b2  = (data >> 6) & 1;
        uint32_t b5_3 = (data >> 10) & 0x7;
        return (b6 << 6) | (b5_3 << 3) | (b2 << 2);
    }

    // TODO : review
    // c.addi/c.li/c.andi: sext({bit12, bits[6:2]})
    int32_t ci_imm() const {
        uint32_t b4_0 = (data >> 2) & 0x1f;
        uint32_t b5   = (data >> 12) & 1;
        uint32_t raw = (b5 << 5) | b4_0;
        return (int32_t)(raw << 26) >> 26;
    }

    // TODO : review
    // c.lui: sext({bit12, bits[6:2]} << 12)
    int32_t ci_lui_imm() const {
        uint32_t b4_0 = (data >> 2) & 0x1f;
        uint32_t b5   = (data >> 12) & 1;
        uint32_t raw = (b5 << 5) | b4_0;
        return (int32_t)(raw << 26) >> 14;
    }

    // TODO : review
    // c.addi16sp: sext(nzimm[9|4|6|8:7|5])
    int32_t ci_addi16sp_imm() const {
        uint32_t b5   = (data >> 2) & 1;
        uint32_t b8_7 = (data >> 3) & 0x3;
        uint32_t b6   = (data >> 5) & 1;
        uint32_t b4   = (data >> 6) & 1;
        uint32_t b9   = (data >> 12) & 1;
        uint32_t raw = (b9 << 9) | (b8_7 << 7) | (b6 << 6) | (b5 << 5) | (b4 << 4);
        return (int32_t)(raw << 22) >> 22;
    }

    // TODO : review
    // c.lwsp: uimm[5|4:2|7:6]
    uint32_t ci_lwsp_uimm() const {
        uint32_t b7_6 = (data >> 2) & 0x3;
        uint32_t b4_2 = (data >> 4) & 0x7;
        uint32_t b5   = (data >> 12) & 1;
        return (b7_6 << 6) | (b5 << 5) | (b4_2 << 2);
    }

    // TODO : review
    // c.swsp: uimm[5:2|7:6]
    uint32_t css_swsp_uimm() const {
        uint32_t b7_6 = (data >> 7) & 0x3;
        uint32_t b5_2 = (data >> 9) & 0xf;
        return (b7_6 << 6) | (b5_2 << 2);
    }

    // TODO : review
    // c.j/c.jal: sext(imm[11|4|9:8|10|6|7|3:1|5])
    int32_t cj_imm() const {
        uint32_t b5   = (data >> 2) & 1;
        uint32_t b3_1 = (data >> 3) & 0x7;
        uint32_t b7   = (data >> 6) & 1;
        uint32_t b6   = (data >> 7) & 1;
        uint32_t b10  = (data >> 8) & 1;
        uint32_t b9_8 = (data >> 9) & 0x3;
        uint32_t b4   = (data >> 11) & 1;
        uint32_t b11  = (data >> 12) & 1;
        uint32_t raw = (b11 << 11) | (b10 << 10) | (b9_8 << 8) | (b7 << 7) |
                       (b6 << 6) | (b5 << 5) | (b4 << 4) | (b3_1 << 1);
        return (int32_t)(raw << 20) >> 20;
    }

    // TODO : review
    // c.beqz/c.bnez: sext(offset[8|4:3|7:6|2:1|5])
    int32_t cb_imm() const {
        uint32_t b5   = (data >> 2) & 1;
        uint32_t b2_1 = (data >> 3) & 0x3;
        uint32_t b7_6 = (data >> 5) & 0x3;
        uint32_t b4_3 = (data >> 10) & 0x3;
        uint32_t b8   = (data >> 12) & 1;
        uint32_t raw = (b8 << 8) | (b7_6 << 6) | (b5 << 5) | (b4_3 << 3) | (b2_1 << 1);
        return (int32_t)(raw << 23) >> 23;
    }

    // TODO : review
    // c.slli/c.srli/c.srai: {bit12, bits[6:2]}
    uint32_t ci_shamt() const {
        uint32_t b4_0 = (data >> 2) & 0x1f;
        uint32_t b5   = (data >> 12) & 1;
        return (b5 << 5) | b4_0;
    }
};

struct ExecResult {
    bool m_ok;
    uint8_t m_code;
    uint32_t m_payload;

    bool is_ok() const {
        return m_ok;
    }

    static constexpr ExecResult ok() {
        return {
            .m_ok = true,
            .m_code = 0,
            .m_payload = 0,
        };
    }

    static ExecResult illegal_inst() {
        return {
            .m_ok = false,
            .m_code = XCPT_CODE_ILLEGAL_INSTRUCTION,
            .m_payload = 0,
        };
    }

    // TODO : review
    static ExecResult exception(uint8_t code, uint32_t payload) {
        return {
            .m_ok = false,
            .m_code = code,
            .m_payload = payload,
        };
    }

    static constexpr uint8_t INTR_CODE_SSI = 1;
    static constexpr uint8_t INTR_CODE_MSI = 3;
    static constexpr uint8_t INTR_CODE_STI = 5;
    static constexpr uint8_t INTR_CODE_MTI = 7;
    static constexpr uint8_t INTR_CODE_SEI = 9;
    static constexpr uint8_t INTR_CODE_MEI = 11;
    static constexpr uint8_t INTR_CODE_LCOFI = 13;

    static constexpr uint8_t XCPT_CODE_MISALIGNED_FETCH = 0;
    static constexpr uint8_t XCPT_CODE_FETCH_ACCESS = 1;
    static constexpr uint8_t XCPT_CODE_ILLEGAL_INSTRUCTION = 2;
    static constexpr uint8_t XCPT_CODE_BREAKPOINT = 3;
    static constexpr uint8_t XCPT_CODE_MISALIGNED_LOAD = 4;
    static constexpr uint8_t XCPT_CODE_LOAD_ACCESS = 5;
    static constexpr uint8_t XCPT_CODE_MISALIGNED_STORE = 6;
    static constexpr uint8_t XCPT_CODE_STORE_ACCESS = 7;
    static constexpr uint8_t XCPT_CODE_USER_ECALL = 8;
    static constexpr uint8_t XCPT_CODE_SUPERVISOR_ECALL = 9;
    static constexpr uint8_t XCPT_CODE_VIRTUAL_SUPERVISOR_ECALL = 10;
    static constexpr uint8_t XCPT_CODE_MACHINE_ECALL = 11;
    static constexpr uint8_t XCPT_CODE_FETCH_PAGE_FAULT = 12;
    static constexpr uint8_t XCPT_CODE_LOAD_PAGE_FAULT = 13;
    static constexpr uint8_t XCPT_CODE_STORE_PAGE_FAULT = 15;
    static constexpr uint8_t XCPT_CODE_DOUBLE_TRAP = 16;
    static constexpr uint8_t XCPT_CODE_SOFTWARE_CHECK_FAULT = 18;
    static constexpr uint8_t XCPT_CODE_HARDWARE_ERROR_FAULT = 19;
    static constexpr uint8_t XCPT_CODE_FETCH_GUEST_PAGE_FAULT = 20;
    static constexpr uint8_t XCPT_CODE_LOAD_GUEST_PAGE_FAULT = 21;
    static constexpr uint8_t XCPT_CODE_VIRTUAL_INSTRUCTION = 22;
    static constexpr uint8_t XCPT_CODE_STORE_GUEST_PAGE_FAULT = 23;
};

struct NextPc {
    bool m_jump;
    uint32_t m_pc;

    explicit NextPc(uint32_t pc) {
        m_jump = false;
        m_pc = pc;
    }

    NextPc(NextPc&&) = delete;
    NextPc(const NextPc&) = delete;

    uint32_t read() const {
        assert(!m_jump);
        return m_pc;
    }

    void jump(uint32_t npc) {
        m_jump = true;
        m_pc = npc;
    }
};

enum class PrivMode {
    U = 0,
    S = 1,
    // H = 2,
    M = 3,
};

template<typename Model>
struct ModelHelper {
    static constexpr PrivMode LEAST_PRIV_MODE = Model::U_MODE ? PrivMode::U : PrivMode::M;

    static bool is_valid_priv_mode(PrivMode mode) {
        switch (mode) {
            case PrivMode::U: return Model::U_MODE;
            case PrivMode::S: return Model::S_MODE;
            case PrivMode::M: return Model::M_MODE;
            default: return false;
        }
    }

    static bool is_pc_aligned(uint32_t x) {
        if (Model::EXT_C) {
            return (x & 1) == 0;
        } else {
            return (x & 3) == 0;
        }
    }
};
