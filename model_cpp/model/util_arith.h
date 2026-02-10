#pragma once

#include <cstdint>
#include <cstdlib>

enum class AluOp {
    ADD,
    SUB,
    SLT_S,
    SLT_U,
    AND,
    OR,
    XOR,
    MUL,
    MULH_SS,
    MULH_SU,
    MULH_UU,
    DIV_S,
    DIV_U,
    REM_S,
    REM_U,
};

enum class ShiftOp {
    LOGIC_LEFT,
    LOGIC_RIGHT,
    ARITH_RIGHT,
};

inline uint32_t compute_u32(AluOp op, uint32_t src1, uint32_t src2) {
    switch (op) {
        case AluOp::ADD:   return src1 + src2;
        case AluOp::SUB:   return src1 - src2;
        case AluOp::SLT_S: return (int32_t)src1 < (int32_t)src2 ? 1u : 0u;
        case AluOp::SLT_U: return src1 < src2 ? 1u : 0u;
        case AluOp::AND:   return src1 & src2;
        case AluOp::OR:    return src1 | src2;
        case AluOp::XOR:   return src1 ^ src2;
        case AluOp::MUL:   return src1 * src2;
        case AluOp::MULH_SS: {
            int64_t a = (int64_t)(int32_t)src1;
            int64_t b = (int64_t)(int32_t)src2;
            return (uint32_t)((a * b) >> 32);
        }
        case AluOp::MULH_SU: {
            int64_t a = (int64_t)(int32_t)src1;
            int64_t b = (int64_t)(uint32_t)src2;
            return (uint32_t)((a * b) >> 32);
        }
        case AluOp::MULH_UU: {
            uint64_t a = (uint64_t)src1;
            uint64_t b = (uint64_t)src2;
            return (uint32_t)((a * b) >> 32);
        }
        case AluOp::DIV_S: {
            if (src2 == 0)
                return UINT32_MAX; // -1
            if (src1 == 0x80000000u && src2 == 0xffffffffu)
                return src1; // overflow: INT32_MIN / -1 = INT32_MIN
            return (uint32_t)((int32_t)src1 / (int32_t)src2);
        }
        case AluOp::DIV_U: {
            if (src2 == 0)
                return UINT32_MAX;
            return src1 / src2;
        }
        case AluOp::REM_S: {
            if (src2 == 0)
                return src1;
            if (src1 == 0x80000000u && src2 == 0xffffffffu)
                return 0;
            return (uint32_t)((int32_t)src1 % (int32_t)src2);
        }
        case AluOp::REM_U: {
            if (src2 == 0)
                return src1;
            return src1 % src2;
        }
        default: abort();
    }
}

inline uint32_t shift_u32(ShiftOp op, uint32_t src1, uint8_t shamt) {
    switch (op) {
        case ShiftOp::LOGIC_LEFT:  return src1 << shamt;
        case ShiftOp::LOGIC_RIGHT: return src1 >> shamt;
        case ShiftOp::ARITH_RIGHT: return (uint32_t)((int32_t)src1 >> shamt);
        default: abort();
    }
}

enum class RoundingMode: uint8_t {
    RNE = 0,    // Round to Nearest, ties to Even
    RTZ = 1,    // Round towzrds Zero
    RDN = 2,    // Round Down
    RUP = 3,    // Round Up
    RMM = 4,    // Round to Nearest, ties to Max Magnitude
};

template<typename V>
struct WithFlag {
    V value;
    uint8_t fflags;
};

struct F32 {
    uint32_t bits;

    static constexpr uint32_t CANONICAL_NAN_BITS = 0x7fc00000u;

    static bool is_nan(F32 x) {
        return (x.bits & 0x7f800000u) == 0x7f800000u && (x.bits & 0x007fffffu) != 0;
    }
    static bool is_signaling_nan(F32 x) {
        return (x.bits & 0x7fc00000u) == 0x7f800000u && (x.bits & 0x003fffffu) != 0;
    }

    static F32 copysign(F32 x, F32 y) {
        return { .bits = (x.bits & 0x7fffffffu) | (y.bits & 0x80000000u) };
    }
    static F32 copysign_neg(F32 x, F32 y) {
        return { .bits = (x.bits & 0x7fffffffu) | ((~y.bits) & 0x80000000u) };
    }
    static F32 copysign_xor(F32 x, F32 y) {
        return { .bits = (x.bits & 0x7fffffffu) | ((x.bits ^ y.bits) & 0x80000000u) };
    }

    static WithFlag<F32> fadd(F32 x, F32 y, RoundingMode rm);
    static WithFlag<F32> fsub(F32 x, F32 y, RoundingMode rm);
    static WithFlag<F32> fmul(F32 x, F32 y, RoundingMode rm);
    static WithFlag<F32> fdiv(F32 x, F32 y, RoundingMode rm);
    static WithFlag<F32> fsqrt(F32 x, RoundingMode rm);
    static WithFlag<F32> fmadd(F32 x, F32 y, F32 z, RoundingMode rm);
    static WithFlag<F32> from_i32(uint32_t x, RoundingMode rm);
    static WithFlag<F32> from_u32(uint32_t x, RoundingMode rm);
    static WithFlag<uint32_t> to_i32(F32 x, RoundingMode rm);
    static WithFlag<uint32_t> to_u32(F32 x, RoundingMode rm);
    static WithFlag<bool> feq(F32 x, F32 y);
    static WithFlag<bool> flt(F32 x, F32 y);
    static WithFlag<bool> fle(F32 x, F32 y);
    static WithFlag<F32> fmin(F32 x, F32 y);
    static WithFlag<F32> fmax(F32 x, F32 y);
    static uint32_t fclass(F32 x);
};

constexpr F32 F32_CANONICAL_NAN = { .bits = F32::CANONICAL_NAN_BITS };

enum class FpAluOp {
    Add,
    Sub,
    Mul,
    Div,
};

inline WithFlag<F32> fp_compute_f32(FpAluOp op, F32 x, F32 y, RoundingMode rm) {
    switch (op) {
        case FpAluOp::Add: return F32::fadd(x, y, rm);
        case FpAluOp::Sub: return F32::fsub(x, y, rm);
        case FpAluOp::Mul: return F32::fmul(x, y, rm);
        case FpAluOp::Div: return F32::fdiv(x, y, rm);
        default: abort();
    }
}
