#include "../model_util.h"

struct CSR_FCSR {
  static constexpr uint16_t INDEX = 0x003;
  static constexpr bool WRITABLE = true;

  static ExecResult do_read(CoreModel& core, uint32_t* ret) {
    REQUIRE_FP(core);

    *ret = do_inspect(core);
    return ExecResult::ok();
  }

  static ExecResult do_write(CoreModel& core, uint32_t value) {
    REQUIRE_FP(core);
    
    core.m_frm = (value >> 5) & 0x07;
    core.m_fflags = value & 0x1f;
    core.trace->csr_write(INDEX);

    core.mark_dirty_fs();
    return ExecResult::ok();
  }

  static constexpr bool INSPECTABLE = true;
  static uint32_t do_inspect(CoreModel const& core) {
    // [7:5] frm
    // [4:0] fflags
    return ((core.m_frm & 0x07) << 5) | (core.m_fflags & 0x1f);
  }
};

struct CSR_FFLAGS {
  static constexpr uint16_t INDEX = 0x001;
  static constexpr bool WRITABLE = true;

  static ExecResult do_read(CoreModel& core, uint32_t* ret) {
    REQUIRE_FP(core);

    *ret = core.m_fflags & 0x1f;
    return ExecResult::ok();
  }

  static ExecResult do_write(CoreModel& core, uint32_t value) {
    REQUIRE_FP(core);

    core.m_fflags = value & 0x1f;
    core.trace->csr_write(CSR_FCSR::INDEX);

    core.mark_dirty_fs();
    return ExecResult::ok();
  }

  static constexpr bool INSPECTABLE = false;
  static uint32_t do_inspect(CoreModel const& core);
};

struct CSR_FRM {
  static constexpr uint16_t INDEX = 0x002;
  static constexpr bool WRITABLE = true;

  static ExecResult do_read(CoreModel& core, uint32_t* ret) {
    REQUIRE_FP(core);

    *ret = core.m_frm & 0x07;
    return ExecResult::ok();
  }

  static ExecResult do_write(CoreModel& core, uint32_t value) {
    REQUIRE_FP(core);

    core.m_frm = value & 0x07;
    core.trace->csr_write(CSR_FCSR::INDEX);

    core.mark_dirty_fs();
    return ExecResult::ok();
  }

  static constexpr bool INSPECTABLE = false;
  static uint32_t do_inspect(CoreModel const& core);
};
