#include "../model_util.h"

struct CSR_MVENDORID {
  static constexpr uint16_t INDEX = 0xf11;
  static constexpr bool WRITABLE = false;

  static ExecResult do_read(CoreModel& core, uint32_t* ret) {
    REQUIRE_PRIV_M(core);

    *ret = do_inspect(core);
    return ExecResult::ok();
  }

  static constexpr bool INSPECTABLE = true;
  static uint32_t do_inspect([[maybe_unused]] CoreModel const& core) {
    return 0;
  }
};

struct CSR_MARCHID {
  static constexpr uint16_t INDEX = 0xf12;
  static constexpr bool WRITABLE = false;

  static ExecResult do_read(CoreModel& core, uint32_t* ret) {
    REQUIRE_PRIV_M(core);

    *ret = do_inspect(core);
    return ExecResult::ok();
  }

  static constexpr bool INSPECTABLE = true;
  static uint32_t do_inspect([[maybe_unused]] CoreModel const& core) {
    return 0;
  }
};

struct CSR_MIMPID {
  static constexpr uint16_t INDEX = 0xf13;
  static constexpr bool WRITABLE = false;

  static ExecResult do_read(CoreModel& core, uint32_t* ret) {
    REQUIRE_PRIV_M(core);

    *ret = do_inspect(core);
    return ExecResult::ok();
  }

  static constexpr bool INSPECTABLE = true;
  static uint32_t do_inspect([[maybe_unused]] CoreModel const& core) {
    return 0;
  }
};

struct CSR_MHARTID {
  static constexpr uint16_t INDEX = 0xf14;
  static constexpr bool WRITABLE = false;

  static ExecResult do_read(CoreModel& core, uint32_t* ret) {
    REQUIRE_PRIV_M(core);

    *ret = do_inspect(core);
    return ExecResult::ok();
  }

  static constexpr bool INSPECTABLE = true;
  static uint32_t do_inspect([[maybe_unused]] CoreModel const& core) {
    return 0;
  }
};

struct CSR_MCONFIGPTR {
  static constexpr uint16_t INDEX = 0xf15;
  static constexpr bool WRITABLE = false;

  static ExecResult do_read(CoreModel& core, uint32_t* ret) {
    REQUIRE_PRIV_M(core);

    *ret = do_inspect(core);
    return ExecResult::ok();
  }

  static constexpr bool INSPECTABLE = true;
  static uint32_t do_inspect([[maybe_unused]] CoreModel const& core) {
    return 0;
  }
};
