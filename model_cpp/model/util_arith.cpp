#include "util_arith.h"

extern "C" {
#include <softfloat.h>
}

// Ensure RoundingMode is compatible with softfloat
static_assert((int)softfloat_round_near_even == (int)RoundingMode::RNE);
static_assert((int)softfloat_round_minMag == (int)RoundingMode::RTZ);
static_assert((int)softfloat_round_min == (int)RoundingMode::RDN);
static_assert((int)softfloat_round_max == (int)RoundingMode::RUP);
static_assert((int)softfloat_round_near_maxMag == (int)RoundingMode::RMM);

// Ensure softfloat's representation of exception flags
// is the same with RISCV's
static_assert((int)softfloat_flag_inexact == 1);
static_assert((int)softfloat_flag_underflow == 2);
static_assert((int)softfloat_flag_overflow == 4);
static_assert((int)softfloat_flag_infinite == 8);
static_assert((int)softfloat_flag_invalid == 16);

static void set_rounding_mode_clear_fflags(RoundingMode rm) {
  softfloat_roundingMode = (int)rm;
  softfloat_exceptionFlags = 0;
}

static void clear_fflags() {
  // Defensive programming,
  // we clear roundingMode even if the operation does not depend on it.
  softfloat_roundingMode = softfloat_round_near_even;

  softfloat_exceptionFlags = 0;
}

static bool f32_lt_total_order(uint32_t x, uint32_t y) {
    bool x_sign = (x >> 31) != 0;
    bool y_sign = (y >> 31) != 0;

    if (!x_sign && !y_sign) return x < y;
    if (!x_sign && y_sign) return false;
    if (x_sign && !y_sign) return true;
    return x > y;
}

WithFlag<F32> F32::fadd(F32 x, F32 y, RoundingMode rm) {
    set_rounding_mode_clear_fflags(rm);

    float32_t xx = { .v = x.bits };
    float32_t yy = { .v = y.bits };

    float32_t res = f32_add(xx, yy);
    return {
        .value = { .bits = res.v },
        .fflags = softfloat_exceptionFlags,
    };
}

WithFlag<F32> F32::fsub(F32 x, F32 y, RoundingMode rm) {
    set_rounding_mode_clear_fflags(rm);

    float32_t xx = { .v = x.bits };
    float32_t yy = { .v = y.bits };

    float32_t res = f32_sub(xx, yy);
    return {
        .value = { .bits = res.v },
        .fflags = softfloat_exceptionFlags,
    };
}

WithFlag<F32> F32::fmul(F32 x, F32 y, RoundingMode rm) {
    set_rounding_mode_clear_fflags(rm);

    float32_t xx = { .v = x.bits };
    float32_t yy = { .v = y.bits };

    float32_t res = f32_mul(xx, yy);
    return {
        .value = { .bits = res. v},
        .fflags = softfloat_exceptionFlags,
    };
}

WithFlag<F32> F32::fdiv(F32 x, F32 y, RoundingMode rm) {
    set_rounding_mode_clear_fflags(rm);

    float32_t xx = { .v = x.bits };
    float32_t yy = { .v = y.bits };

    float32_t res = f32_div(xx, yy);
    return {
        .value = { .bits = res. v},
        .fflags = softfloat_exceptionFlags,
    };
}

WithFlag<F32> F32::fsqrt(F32 x, RoundingMode rm) {
    set_rounding_mode_clear_fflags(rm);

    float32_t xx = { .v = x.bits };

    float32_t res = f32_sqrt(xx);
    return {
        .value = { .bits = res. v},
        .fflags = softfloat_exceptionFlags,
    };
}

WithFlag<F32> F32::fmadd(F32 x, F32 y, F32 z, RoundingMode rm) {
    set_rounding_mode_clear_fflags(rm);

    float32_t xx = { .v = x.bits };
    float32_t yy = { .v = y.bits };
    float32_t zz = { .v = z.bits };

    float32_t res = f32_mulAdd(xx, yy, zz);
    return {
        .value = { .bits = res.v },
        .fflags = softfloat_exceptionFlags,
    };
}

WithFlag<F32> F32::from_i32(uint32_t x, RoundingMode rm) {
    set_rounding_mode_clear_fflags(rm);

    float32_t res = i32_to_f32((int32_t)x);
    return {
        .value = { .bits = res.v },
        .fflags = softfloat_exceptionFlags,
    };
}

WithFlag<F32> F32::from_u32(uint32_t x, RoundingMode rm) {
    set_rounding_mode_clear_fflags(rm);

    float32_t res = ui32_to_f32(x);
    return {
        .value = { .bits = res.v },
        .fflags = softfloat_exceptionFlags,
    };
}

WithFlag<uint32_t> F32::to_i32(F32 x, RoundingMode rm) {
    clear_fflags();

    float32_t xx = { .v = x.bits };
    return {
        .value = (uint32_t)f32_to_i32(xx, (uint8_t)rm, true),
        .fflags = softfloat_exceptionFlags,
    };
}

WithFlag<uint32_t> F32::to_u32(F32 x, RoundingMode rm) {
    clear_fflags();

    float32_t xx = { .v = x.bits };
    return {
        .value = (uint32_t)f32_to_ui32(xx, (uint8_t)rm, true),
        .fflags = softfloat_exceptionFlags,
    };
}

WithFlag<bool> F32::feq(F32 x, F32 y) {
    clear_fflags();

    float32_t xx = { .v = x.bits };
    float32_t yy = { .v = y.bits };
    return {
        .value = f32_eq(xx, yy),
        .fflags = softfloat_exceptionFlags,
    };
}

WithFlag<bool> F32::flt(F32 x, F32 y) {
    clear_fflags();

    float32_t xx = { .v = x.bits };
    float32_t yy = { .v = y.bits };
    return {
        .value = f32_lt(xx, yy),
        .fflags = softfloat_exceptionFlags,
    };
}

WithFlag<bool> F32::fle(F32 x, F32 y) {
    clear_fflags();

    float32_t xx = { .v = x.bits };
    float32_t yy = { .v = y.bits };
    return {
        .value = f32_le(xx, yy),
        .fflags = softfloat_exceptionFlags,
    };
}

WithFlag<F32> F32::fmin(F32 x, F32 y) {
    bool x_is_nan = F32::is_nan(x);
    bool y_is_nan = F32::is_nan(y);
    if (x_is_nan || y_is_nan) {
        uint8_t flags = 0;
        if (F32::is_signaling_nan(x) || F32::is_signaling_nan(y)) {
            flags |= softfloat_flag_invalid;
        }

        F32 value = F32_CANONICAL_NAN;
        if (x_is_nan && !y_is_nan) {
            value = y;
        } else if (!x_is_nan && y_is_nan) {
            value = x;
        }

        return {
            .value = value,
            .fflags = flags
        };
    }

    uint32_t res = f32_lt_total_order(x.bits, y.bits) ? x.bits : y.bits;

    return {
        .value = { .bits = res },
        .fflags = 0,
    };
}

WithFlag<F32> F32::fmax(F32 x, F32 y) {
    bool x_is_nan = F32::is_nan(x);
    bool y_is_nan = F32::is_nan(y);
    if (x_is_nan || y_is_nan) {
        uint8_t flags = 0;
        if (F32::is_signaling_nan(x) || F32::is_signaling_nan(y)) {
            flags |= softfloat_flag_invalid;
        }

        F32 value = F32_CANONICAL_NAN;
        if (x_is_nan && !y_is_nan) {
            value = y;
        } else if (!x_is_nan && y_is_nan) {
            value = x;
        }

        return {
            .value = value,
            .fflags = flags
        };
    }

    uint32_t res = f32_lt_total_order(x.bits, y.bits) ? y.bits : x.bits;

    return {
        .value = { .bits = res },
        .fflags = 0,
    };
}

uint32_t F32::fclass(F32 x) {
    uint32_t ui = x.bits;
    uint32_t exp = (ui >> 23) & 0xff;
    uint32_t frac = ui & 0x007fffff;
    bool sign = (ui >> 31) != 0;

    return
        ( sign && exp == 0xff && frac == 0) << 0 |
        ( sign && exp != 0xff && exp != 0)  << 1 |
        ( sign && exp == 0 && frac != 0)    << 2 |
        ( sign && exp == 0 && frac == 0)    << 3 |
        (!sign && exp == 0 && frac == 0)    << 4 |
        (!sign && exp == 0 && frac != 0)    << 5 |
        (!sign && exp != 0xff && exp != 0)  << 6 |
        (!sign && exp == 0xff && frac == 0) << 7 |
        (F32::is_nan(x) && F32::is_signaling_nan(x)) << 8 |
        (F32::is_nan(x) && !F32::is_signaling_nan(x)) << 9;
}
