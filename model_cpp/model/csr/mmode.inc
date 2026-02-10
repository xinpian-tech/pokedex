#include "../model_util.h"

struct CSR_MSTATUS {
  static constexpr uint16_t INDEX = 0x300;
  static constexpr bool WRITABLE = true;

  static ExecResult do_read(CoreModel& core, uint32_t* ret) {
    REQUIRE_PRIV_M(core);

    *ret = do_inspect(core);
    return ExecResult::ok();
  }

  static constexpr uint32_t FS_MASK   = 0x6000; // fs:   [14:13]
  static constexpr uint32_t VS_MASK   = 0x0600; // vs:   [10:9]
  static constexpr uint32_t MPIE_MASK = 0x0080; // mpie: [7]
  static constexpr uint32_t MIE_MASK  = 0x0008; // mie:  [3]

  static ExecResult do_write([[maybe_unused]] CoreModel& core, uint32_t value) {
    REQUIRE_PRIV_M(core);

    // IMPL NOTE:
    //   - For unsupported mpp value, the mpp field is unchanged
    //   - For FS/VS, initial/clean is converted to dirty at write

    PrivMode new_mpp = PrivMode((value >> 11) & 0x03);
    if (CoreModel::is_valid_priv_mode(new_mpp)) {
        core.m_mstatus_mpp = new_mpp;
    }

    core.m_mstatus_fs = (value & FS_MASK) != 0;
    core.m_mstatus_vs = (value & VS_MASK) != 0;

    core.m_mstatus_mpie = (value & MPIE_MASK) != 0;
    core.m_mstatus_mie = (value & MIE_MASK) != 0;

    core.trace.csr_write(INDEX);

    return ExecResult::ok();
  }

  static constexpr bool INSPECTABLE = true;
  static uint32_t do_inspect(CoreModel const& core) {
    uint32_t value = 0;

    bool dirty_fs = (CoreModel::FLEN > 0) && core.m_mstatus_fs;
    bool dirty_vs = (CoreModel::VLEN > 0) && core.m_mstatus_vs;

    if (dirty_fs || dirty_vs) {
        value |= 1 << 31; // sd: [31]
    }

    if (dirty_fs) {
        value |= FS_MASK; // fs = dirty
    }

    // mpp: [12:11]
    value |= uint32_t(core.m_mstatus_mpp) << 11;

    if (dirty_vs) {
        value |= VS_MASK; // vs = dirty
    }

    if (core.m_mstatus_mpie) {
        value |= MPIE_MASK;
    }
    if (core.m_mstatus_mie) {
        value |= MIE_MASK;
    }

    return value;
  }
};

struct CSR_MSCRATCH {
  static constexpr uint16_t INDEX = 0x340;
  static constexpr bool WRITABLE = true;

  static ExecResult do_read(CoreModel& core, uint32_t* ret) {
    REQUIRE_PRIV_M(core);

    *ret = do_inspect(core);
    return ExecResult::ok();
  }

  static ExecResult do_write(CoreModel& core, uint32_t value) {
    REQUIRE_PRIV_M(core);

    core.m_mscratch = value;
    core.trace.csr_write(INDEX);

    return ExecResult::ok();
  }

  static constexpr bool INSPECTABLE = true;
  static uint32_t do_inspect(CoreModel const& core) {
    return core.m_mscratch;
  }
};

struct CSR_MTVEC {
    static constexpr uint16_t INDEX = 0x305;
    static constexpr bool WRITABLE = true;

    static ExecResult do_read(CoreModel& core, uint32_t* ret) {
        REQUIRE_PRIV_M(core);

        *ret = do_inspect(core);
        return ExecResult::ok();
    }

    static ExecResult do_write([[maybe_unused]] CoreModel& core, [[maybe_unused]] uint32_t value) {
        REQUIRE_PRIV_M(core);

        // IMPL NOTE:
        //   - direct mode: mtvec must be 4 byte aligned
        //   - vector mode: mtvec must be 4 byte aligned
        //   - if the value uses an unsupported mode, the write is ignored

        // NOTE: the spec permits the implementation defining its own alignment requirements for mtvec

        switch (value & 0x03) {
            case 0:
            case 1: {
                core.m_mtvec = value;
                core.trace.csr_write(INDEX);
                break;
            }
            default: break;
        }

        return ExecResult::ok();
    }

    static constexpr bool INSPECTABLE = true;
    static uint32_t do_inspect(CoreModel const& core) {
        return core.m_mtvec;
    }
};

struct CSR_MEPC {
    static constexpr uint16_t INDEX = 0x341;
    static constexpr bool WRITABLE = true;

    static ExecResult do_read(CoreModel& core, uint32_t* ret) {
        REQUIRE_PRIV_M(core);

        *ret = do_inspect(core);
        return ExecResult::ok();
    }

    static ExecResult do_write(CoreModel& core, uint32_t value) {
        REQUIRE_PRIV_M(core);

        // IMPL NOTE: except that the last 1 or 2 bits must be zero (mandated by the spec),
        //   mepc is a XLEN-bit register.

        // NOTE: the spec only requires mepc could hold all valid virtual addresses
        //
        // NOTE: We will never support writable misa.C

        if constexpr (CoreModel::EXT_C) {
            // clear the least bit
            core.m_mepc = value & ~uint32_t(1);
        } else {
            // clear least 2 bits
            core.m_mepc = value & ~uint32_t(3);
        }

        core.trace.csr_write(INDEX);
        return ExecResult::ok();
    }

    static constexpr bool INSPECTABLE = true;
    static uint32_t do_inspect(CoreModel const& core) {
        return core.m_mepc;
    }
};

struct CSR_MCAUSE {
  static constexpr uint16_t INDEX = 0x342;
  static constexpr bool WRITABLE = true;

  static ExecResult do_read(CoreModel& core, uint32_t* ret) {
    REQUIRE_PRIV_M(core);

    *ret = do_inspect(core);
    return ExecResult::ok();
  }

  static ExecResult do_write(CoreModel& core, uint32_t value) {
    REQUIRE_PRIV_M(core);

    // IMPL NOTE: mcause is a XLEN-bit register

    // NOTE: the spec only requires mcause could hold all supported trap code (WLRL)

    core.m_mcause = value;
    core.trace.csr_write(INDEX);

    return ExecResult::ok();
  }

  static constexpr bool INSPECTABLE = true;
  static uint32_t do_inspect(CoreModel const& core) {
    return core.m_mcause;
  }
};
