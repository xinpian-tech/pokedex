#include <assert.h>
#include <softfloat.h>

#include <pokedex-sim_types.h>

// ASL interpreter will suffix all the function with "_N" suffix. For
// non-polymorphic function, it is always "_0". Even for external function, ASLi
// will generate function signature with "_0" suffix. This macro help adjust all
// the function name in one place.
#define ASL_FN(fn) fn##_0

// NOTE: we may replace it with a rm_to_softfloat function once it breaks.
//       It lokks like quite hacky.
static_assert((int)softfloat_round_near_even == (int)RM_RNE);
static_assert((int)softfloat_round_minMag == (int)RM_RTZ);
static_assert((int)softfloat_round_min == (int)RM_RDN);
static_assert((int)softfloat_round_max == (int)RM_RUP);
static_assert((int)softfloat_round_near_maxMag == (int)RM_RMM);

// Ensure softfloat's representation of exception flags
// is the same with RISCV's
static_assert((int)softfloat_flag_inexact == 1);
static_assert((int)softfloat_flag_underflow == 2);
static_assert((int)softfloat_flag_overflow == 4);
static_assert((int)softfloat_flag_infinite == 8);
static_assert((int)softfloat_flag_invalid == 16);

static void set_rounding_mode_clear_fflags(RM rm) {
  softfloat_roundingMode = rm;
  softfloat_exceptionFlags = 0;
}

static void clear_fflags() {
  // Defensive programming,
  // we clear roundingMode even if the operation does not depend on it.
  softfloat_roundingMode = softfloat_round_near_even;

  softfloat_exceptionFlags = 0;
}

F32_Flags ASL_FN(riscv_f32_add)(RM rm, uint32_t x, uint32_t y) {
  set_rounding_mode_clear_fflags(rm);

  float32_t xx = { .v = x };
  float32_t yy = { .v = y };

  F32_Flags res;
  res.value = f32_add(xx, yy).v;
  res.fflags = softfloat_exceptionFlags;
  return res;
}

F32_Flags ASL_FN(riscv_f32_sub)(RM rm, uint32_t x, uint32_t y) {
  set_rounding_mode_clear_fflags(rm);

  float32_t xx = { .v = x };
  float32_t yy = { .v = y };

  F32_Flags res;
  res.value = f32_sub(xx, yy).v;
  res.fflags = softfloat_exceptionFlags;
  return res;
}

F32_Flags ASL_FN(riscv_f32_mul)(RM rm, uint32_t x, uint32_t y) {
  set_rounding_mode_clear_fflags(rm);

  float32_t xx = { .v = x };
  float32_t yy = { .v = y };

  F32_Flags res;
  res.value = f32_mul(xx, yy).v;
  res.fflags = softfloat_exceptionFlags;
  return res;
}

F32_Flags ASL_FN(riscv_f32_div)(RM rm, uint32_t x, uint32_t y) {
  set_rounding_mode_clear_fflags(rm);

  float32_t xx = { .v = x };
  float32_t yy = { .v = y };

  F32_Flags res;
  res.value = f32_div(xx, yy).v;
  res.fflags = softfloat_exceptionFlags;
  return res;
}

F32_Flags ASL_FN(riscv_f32_sqrt)(RM rm, uint32_t x) {
  set_rounding_mode_clear_fflags(rm);

  float32_t xx = { .v = x };

  F32_Flags res;
  res.value = f32_sqrt(xx).v;
  res.fflags = softfloat_exceptionFlags;
  return res;
}

F32_Flags ASL_FN(riscv_f32_mulAdd)(RM rm, uint32_t x, uint32_t y, uint32_t z) {
  set_rounding_mode_clear_fflags(rm);

  float32_t xx = { .v = x };
  float32_t yy = { .v = y };
  float32_t zz = { .v = z };

  F32_Flags res;
  res.value = f32_mulAdd(xx, yy, zz).v;
  res.fflags = softfloat_exceptionFlags;
  return res;
}

Bool_Flags ASL_FN(riscv_f32_eqQuiet)(uint32_t x, uint32_t y) {
  clear_fflags();

  float32_t xx = { .v = x };
  float32_t yy = { .v = y };

  Bool_Flags res;
  res.value = f32_eq(xx, yy);
  res.fflags = softfloat_exceptionFlags;
  return res;
}

Bool_Flags ASL_FN(riscv_f32_ltSignaling)(uint32_t x, uint32_t y) {
  clear_fflags();

  float32_t xx = { .v = x };
  float32_t yy = { .v = y };

  Bool_Flags res;
  res.value = f32_lt(xx, yy);
  res.fflags = softfloat_exceptionFlags;
  return res;
}

Bool_Flags ASL_FN(riscv_f32_leSignaling)(uint32_t x, uint32_t y) {
  clear_fflags();

  float32_t xx = { .v = x };
  float32_t yy = { .v = y };

  Bool_Flags res;
  res.value = f32_le(xx, yy);
  res.fflags = softfloat_exceptionFlags;
  return res;
}

F32_Flags ASL_FN(riscv_f32_fromSInt32)(RM rm, uint32_t x) {
  set_rounding_mode_clear_fflags(rm);

  F32_Flags res;
  res.value = i32_to_f32((int32_t)x).v;
  res.fflags = softfloat_exceptionFlags;
  return res;
}

F32_Flags ASL_FN(riscv_f32_fromUInt32)(RM rm, uint32_t x) {
  set_rounding_mode_clear_fflags(rm);

  F32_Flags res;
  res.value = ui32_to_f32(x).v;
  res.fflags = softfloat_exceptionFlags;
  return res;
}

Bits32_Flags ASL_FN(riscv_f32_toSInt32)(RM rm, uint32_t x) {
  clear_fflags();

  float32_t xx = { .v = x };

  Bits32_Flags res;
  res.value = (uint32_t)f32_to_i32(xx, rm, true);
  res.fflags = softfloat_exceptionFlags;
  return res;
}

Bits32_Flags ASL_FN(riscv_f32_toUInt32)(RM rm, uint32_t x) {
  clear_fflags();

  float32_t xx = { .v = x };

  Bits32_Flags res;
  res.value = f32_to_ui32(xx, rm, true);
  res.fflags = softfloat_exceptionFlags;
  return res;
}

//////////////////////////////////////////////
// Following are approx FP functions.       //
// The implmentation is adapted from Spike. //
//////////////////////////////////////////////

// See for original code: <riscv_isa_sim>/softfloat/fall_reciprocal.c

#define defaultNaNF32UI 0x7FC00000

#define signF32UI( a ) ((bool) ((uint32_t) (a)>>31))
#define expF32UI( a ) ((int_fast16_t) ((a)>>23) & 0xFF)
#define fracF32UI( a ) ((a) & 0x007FFFFF)

#define isNaNF32UI( a ) (((~(a) & 0x7F800000) == 0) && ((a) & 0x007FFFFF))

#define softfloat_isSigNaNF32UI( uiA ) ((((uiA) & 0x7FC00000) == 0x7F800000) && ((uiA) & 0x003FFFFF))

static inline uint_fast16_t f32_classify(uint_fast32_t uiA)
{
    uint_fast16_t infOrNaN = expF32UI( uiA ) == 0xFF;
    uint_fast16_t subnormalOrZero = expF32UI( uiA ) == 0;
    bool sign = signF32UI( uiA );
    bool fracZero = fracF32UI( uiA ) == 0;
    bool isNaN = isNaNF32UI( uiA );
    bool isSNaN = softfloat_isSigNaNF32UI( uiA );

    return
        (  sign && infOrNaN && fracZero )          << 0 |
        (  sign && !infOrNaN && !subnormalOrZero ) << 1 |
        (  sign && subnormalOrZero && !fracZero )  << 2 |
        (  sign && subnormalOrZero && fracZero )   << 3 |
        ( !sign && infOrNaN && fracZero )          << 7 |
        ( !sign && !infOrNaN && !subnormalOrZero ) << 6 |
        ( !sign && subnormalOrZero && !fracZero )  << 5 |
        ( !sign && subnormalOrZero && fracZero )   << 4 |
        ( isNaN &&  isSNaN )                       << 8 |
        ( isNaN && !isSNaN )                       << 9;
}

static inline uint64_t extract64(uint64_t val, int pos, int len)
{
  assert(pos >= 0 && len > 0 && len <= 64 - pos);
  return (val >> pos) & (~UINT64_C(0) >> (64 - len));
}

static inline uint64_t make_mask64(int pos, int len)
{
    assert(pos >= 0 && len > 0 && pos < 64 && len <= 64);
    return (UINT64_MAX >> (64 - len)) << pos;
}

//user needs to truncate output to required length
static inline uint64_t rsqrte7(uint64_t val, int e, int s, bool sub) {
  uint64_t exp = extract64(val, s, e);
  uint64_t sig = extract64(val, 0, s);
  uint64_t sign = extract64(val, s + e, 1);
  const int p = 7;

  static const uint8_t table[] = {
      52, 51, 50, 48, 47, 46, 44, 43,
      42, 41, 40, 39, 38, 36, 35, 34,
      33, 32, 31, 30, 30, 29, 28, 27,
      26, 25, 24, 23, 23, 22, 21, 20,
      19, 19, 18, 17, 16, 16, 15, 14,
      14, 13, 12, 12, 11, 10, 10, 9,
      9, 8, 7, 7, 6, 6, 5, 4,
      4, 3, 3, 2, 2, 1, 1, 0,
      127, 125, 123, 121, 119, 118, 116, 114,
      113, 111, 109, 108, 106, 105, 103, 102,
      100, 99, 97, 96, 95, 93, 92, 91,
      90, 88, 87, 86, 85, 84, 83, 82,
      80, 79, 78, 77, 76, 75, 74, 73,
      72, 71, 70, 70, 69, 68, 67, 66,
      65, 64, 63, 63, 62, 61, 60, 59,
      59, 58, 57, 56, 56, 55, 54, 53};

  if (sub) {
      while (extract64(sig, s - 1, 1) == 0)
          exp--, sig <<= 1;

      sig = (sig << 1) & make_mask64(0 ,s);
  }

  int idx = ((exp & 1) << (p-1)) | (sig >> (s-p+1));
  uint64_t out_sig = (uint64_t)(table[idx]) << (s-p);
  uint64_t out_exp = (3 * make_mask64(0, e - 1) + ~exp) / 2;

  return (sign << (s+e)) | (out_exp << s) | out_sig;
}

//user needs to truncate output to required length
static inline uint64_t recip7(uint64_t val, int e, int s, int rm, bool sub,
                              bool *round_abnormal)
{
    uint64_t exp = extract64(val, s, e);
    uint64_t sig = extract64(val, 0, s);
    uint64_t sign = extract64(val, s + e, 1);
    const int p = 7;

    static const uint8_t table[] = {
        127, 125, 123, 121, 119, 117, 116, 114,
        112, 110, 109, 107, 105, 104, 102, 100,
        99, 97, 96, 94, 93, 91, 90, 88,
        87, 85, 84, 83, 81, 80, 79, 77,
        76, 75, 74, 72, 71, 70, 69, 68,
        66, 65, 64, 63, 62, 61, 60, 59,
        58, 57, 56, 55, 54, 53, 52, 51,
        50, 49, 48, 47, 46, 45, 44, 43,
        42, 41, 40, 40, 39, 38, 37, 36,
        35, 35, 34, 33, 32, 31, 31, 30,
        29, 28, 28, 27, 26, 25, 25, 24,
        23, 23, 22, 21, 21, 20, 19, 19,
        18, 17, 17, 16, 15, 15, 14, 14,
        13, 12, 12, 11, 11, 10, 9, 9,
        8, 8, 7, 7, 6, 5, 5, 4,
        4, 3, 3, 2, 2, 1, 1, 0};

    if (sub) {
        while (extract64(sig, s - 1, 1) == 0)
            exp--, sig <<= 1;

        sig = (sig << 1) & make_mask64(0 ,s);

        if (exp != 0 && exp != UINT64_MAX) {
            *round_abnormal = true;
            if (rm == 1 ||
                (rm == 2 && !sign) ||
                (rm == 3 && sign))
                return ((sign << (s+e)) | make_mask64(s, e)) - 1;
            else
                return (sign << (s+e)) | make_mask64(s, e);
        }
    }

    int idx = sig >> (s-p);
    uint64_t out_sig = (uint64_t)(table[idx]) << (s-p);
    uint64_t out_exp = 2 * make_mask64(0, e - 1) + ~exp;
    if (out_exp == 0 || out_exp == UINT64_MAX) {
        out_sig = (out_sig >> 1) | make_mask64(s - 1, 1);
        if (out_exp == UINT64_MAX) {
            out_sig >>= 1;
            out_exp = 0;
        }
    }

    return (sign << (s+e)) | (out_exp << s) | out_sig;
}

F32_Flags ASL_FN(riscv_f32_rsqrt7)(RM rm, uint32_t x) {
  F32_Flags res;
  res.value = 0;
  res.fflags = 0;

  (void)(rm); // rsqrt7 do not depend on FRM

  unsigned int ret = f32_classify(x);
  bool sub = false;
  switch(ret) {
    case 0x001: // -inf
    case 0x002: // -normal
    case 0x004: // -subnormal
    case 0x100: // sNaN
        res.fflags = softfloat_flag_invalid;
        __attribute__ ((fallthrough));
    case 0x200: //qNaN
        res.value = defaultNaNF32UI;
        break;
    case 0x008: // -0
        res.value = 0xff800000;
        res.fflags = softfloat_flag_infinite;
        break;
    case 0x010: // +0
        res.value = 0x7f800000;
        res.fflags= softfloat_flag_infinite;
        break;
    case 0x080: //+inf
        res.value = 0x0;
        break;
    case 0x020: //+ sub
        sub = true;
        __attribute__ ((fallthrough));
    default: // +num
        res.value = rsqrte7(x, 8, 23, sub);
        break;
    }

  return res;
}

F32_Flags ASL_FN(riscv_f32_rec7)(RM rm, uint32_t x)
{
  F32_Flags res;
  res.value = 0;
  res.fflags = 0;

  unsigned int ret = f32_classify(x);
  bool sub = false;
  bool round_abnormal = false;
  switch(ret) {
  case 0x001: // -inf
      res.value = 0x80000000;
      break;
  case 0x080: //+inf
      res.value = 0x0;
      break;
  case 0x008: // -0
      res.value = 0xff800000;
      res.fflags |= softfloat_flag_infinite;
      break;
  case 0x010: // +0
      res.value = 0x7f800000;
      res.fflags |= softfloat_flag_infinite;
      break;
  case 0x100: // sNaN
      res.fflags |= softfloat_flag_invalid;
      __attribute__ ((fallthrough));
  case 0x200: //qNaN
      res.value = defaultNaNF32UI;
      break;
  case 0x004: // -subnormal
  case 0x020: //+ sub
      sub = true;
      __attribute__ ((fallthrough));
  default: // +- normal
      res.value = recip7(x, 8, 23, rm, sub, &round_abnormal);
      if (round_abnormal) {
        res.fflags |= softfloat_flag_inexact |
                      softfloat_flag_overflow;
      }
      break;
  }

  return res;
}
