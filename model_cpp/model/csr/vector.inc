#include "../model_util.h"

struct CSR_VSTART {
  static constexpr uint16_t INDEX = 0x008;
  static constexpr bool WRITABLE = true;

  static ExecResult do_read(CoreModel& core, uint32_t* ret) {
    REQUIRE_VECTOR(core);

    *ret = do_inspect(core);
    return ExecResult::ok();
  }

  static ExecResult do_write(CoreModel& core, uint32_t value) {
    REQUIRE_VECTOR(core);

    core.m_vstart = value & (core.VLEN - 1);
    core.trace->csr_write(INDEX);

    core.mark_dirty_vs();
    return ExecResult::ok();
  }

  static constexpr bool INSPECTABLE = true;
  static uint32_t do_inspect(CoreModel const& core) {
    return core.m_vstart;
  }
};

struct CSR_VCSR {
  static constexpr uint16_t INDEX = 0x00f;
  static constexpr bool WRITABLE = true;

  static ExecResult do_read(CoreModel& core, uint32_t* ret) {
    REQUIRE_VECTOR(core);

    *ret = do_inspect(core);
    return ExecResult::ok();
  }

  static ExecResult do_write(CoreModel& core, uint32_t value) {
    REQUIRE_VECTOR(core);

    core.m_vxrm = (value >> 1) & 0x03;
    core.m_vxsat = value & 1;
    core.trace->csr_write(INDEX);

    core.mark_dirty_vs();
    return ExecResult::ok();
  }

  static constexpr bool INSPECTABLE = true;
  static uint32_t do_inspect(CoreModel const& core) {
    // [3:1] vxrm
    //   [0] vxsat
    return ((core.m_vxrm & 0x03) << 1) | core.m_vxsat;
  }
};

struct CSR_VXSAT {
  static constexpr uint16_t INDEX = 0x009;
  static constexpr bool WRITABLE = true;

  static ExecResult do_read(CoreModel& core, uint32_t* ret) {
    REQUIRE_VECTOR(core);

    *ret = core.m_vxsat & 0x01;
    return ExecResult::ok();
  }

  static ExecResult do_write(CoreModel& core, uint32_t value) {
    REQUIRE_VECTOR(core);

    core.m_vxsat = value & 0x01;
    core.trace->csr_write(CSR_VCSR::INDEX);

    core.mark_dirty_vs();
    return ExecResult::ok();
  }

  static constexpr bool INSPECTABLE = false;
  static uint32_t do_inspect(CoreModel const& core);
};

struct CSR_VXRM {
  static constexpr uint16_t INDEX = 0x00a;
  static constexpr bool WRITABLE = true;

  static ExecResult do_read(CoreModel& core, uint32_t* ret) {
    REQUIRE_VECTOR(core);

    *ret = core.m_vxrm & 0x03;
    return ExecResult::ok();
  }

  static ExecResult do_write(CoreModel& core, uint32_t value) {
    REQUIRE_VECTOR(core);

    core.m_vxrm = value & 0x03;
    core.trace->csr_write(CSR_VCSR::INDEX);

    core.mark_dirty_vs();
    return ExecResult::ok();
  }

  static constexpr bool INSPECTABLE = false;
  static uint32_t do_inspect(CoreModel const& core);
};

struct CSR_VLENB {
  static constexpr uint16_t INDEX = 0xc22;
  static constexpr bool WRITABLE = false;

  static ExecResult do_read(CoreModel& core, uint32_t* ret) {
    *ret = core.VLEN / 8;
    return ExecResult::ok();
  }

  static constexpr bool INSPECTABLE = false;
  static uint32_t do_inspect(CoreModel const& core);
};

struct CSR_VL {
  static constexpr uint16_t INDEX = 0xc20;
  static constexpr bool WRITABLE = false;

  static ExecResult do_read(CoreModel& core, uint32_t* ret) {
    REQUIRE_VECTOR(core);

    *ret = do_inspect(core);
    return ExecResult::ok();
  }

  static constexpr bool INSPECTABLE = true;
  static uint32_t do_inspect(CoreModel const& core) {
    return core.m_vtype_vl.vl();
  }
};

struct CSR_VTYPE {
  static constexpr uint16_t INDEX = 0xc21;
  static constexpr bool WRITABLE = false;

  static ExecResult do_read(CoreModel& core, uint32_t* ret) {
    REQUIRE_VECTOR(core);

    *ret = do_inspect(core);
    return ExecResult::ok();
  }

  static constexpr bool INSPECTABLE = true;
  static uint32_t do_inspect(CoreModel const& core) {
    return core.m_vtype_vl.vtype_bits();
  }
};
