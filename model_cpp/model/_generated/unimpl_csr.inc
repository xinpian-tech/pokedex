
struct CSR_MISA {
  static constexpr uint16_t INDEX = 0x301;
  static constexpr bool WRITABLE = true;

  static ExecResult do_read(CoreModel& core, uint32_t* ret) {
    (void)(core);
    (void)(ret);
    unimpl_csr_read("misa");
    return ExecResult::illegal_inst();
  }

  static ExecResult do_write(CoreModel& core, uint32_t value) {
    (void)(core);
    (void)(value);
    unimpl_csr_write("misa");
    return ExecResult::illegal_inst();
  }

  static constexpr bool INSPECTABLE = false;
  static uint32_t do_inspect(CoreModel const& core);
};

struct CSR_MEDELEG {
  static constexpr uint16_t INDEX = 0x302;
  static constexpr bool WRITABLE = true;

  static ExecResult do_read(CoreModel& core, uint32_t* ret) {
    (void)(core);
    (void)(ret);
    unimpl_csr_read("medeleg");
    return ExecResult::illegal_inst();
  }

  static ExecResult do_write(CoreModel& core, uint32_t value) {
    (void)(core);
    (void)(value);
    unimpl_csr_write("medeleg");
    return ExecResult::illegal_inst();
  }

  static constexpr bool INSPECTABLE = false;
  static uint32_t do_inspect(CoreModel const& core);
};

struct CSR_MIDELEG {
  static constexpr uint16_t INDEX = 0x303;
  static constexpr bool WRITABLE = true;

  static ExecResult do_read(CoreModel& core, uint32_t* ret) {
    (void)(core);
    (void)(ret);
    unimpl_csr_read("mideleg");
    return ExecResult::illegal_inst();
  }

  static ExecResult do_write(CoreModel& core, uint32_t value) {
    (void)(core);
    (void)(value);
    unimpl_csr_write("mideleg");
    return ExecResult::illegal_inst();
  }

  static constexpr bool INSPECTABLE = false;
  static uint32_t do_inspect(CoreModel const& core);
};

struct CSR_MIE {
  static constexpr uint16_t INDEX = 0x304;
  static constexpr bool WRITABLE = true;

  static ExecResult do_read(CoreModel& core, uint32_t* ret) {
    (void)(core);
    (void)(ret);
    unimpl_csr_read("mie");
    return ExecResult::illegal_inst();
  }

  static ExecResult do_write(CoreModel& core, uint32_t value) {
    (void)(core);
    (void)(value);
    unimpl_csr_write("mie");
    return ExecResult::illegal_inst();
  }

  static constexpr bool INSPECTABLE = false;
  static uint32_t do_inspect(CoreModel const& core);
};

struct CSR_MCOUNTEREN {
  static constexpr uint16_t INDEX = 0x306;
  static constexpr bool WRITABLE = true;

  static ExecResult do_read(CoreModel& core, uint32_t* ret) {
    (void)(core);
    (void)(ret);
    unimpl_csr_read("mcounteren");
    return ExecResult::illegal_inst();
  }

  static ExecResult do_write(CoreModel& core, uint32_t value) {
    (void)(core);
    (void)(value);
    unimpl_csr_write("mcounteren");
    return ExecResult::illegal_inst();
  }

  static constexpr bool INSPECTABLE = false;
  static uint32_t do_inspect(CoreModel const& core);
};

struct CSR_MSTATUSH {
  static constexpr uint16_t INDEX = 0x310;
  static constexpr bool WRITABLE = true;

  static ExecResult do_read(CoreModel& core, uint32_t* ret) {
    (void)(core);
    (void)(ret);
    unimpl_csr_read("mstatush");
    return ExecResult::illegal_inst();
  }

  static ExecResult do_write(CoreModel& core, uint32_t value) {
    (void)(core);
    (void)(value);
    unimpl_csr_write("mstatush");
    return ExecResult::illegal_inst();
  }

  static constexpr bool INSPECTABLE = false;
  static uint32_t do_inspect(CoreModel const& core);
};

struct CSR_MEDELEGH {
  static constexpr uint16_t INDEX = 0x312;
  static constexpr bool WRITABLE = true;

  static ExecResult do_read(CoreModel& core, uint32_t* ret) {
    (void)(core);
    (void)(ret);
    unimpl_csr_read("medelegh");
    return ExecResult::illegal_inst();
  }

  static ExecResult do_write(CoreModel& core, uint32_t value) {
    (void)(core);
    (void)(value);
    unimpl_csr_write("medelegh");
    return ExecResult::illegal_inst();
  }

  static constexpr bool INSPECTABLE = false;
  static uint32_t do_inspect(CoreModel const& core);
};

struct CSR_MIP {
  static constexpr uint16_t INDEX = 0x344;
  static constexpr bool WRITABLE = true;

  static ExecResult do_read(CoreModel& core, uint32_t* ret) {
    (void)(core);
    (void)(ret);
    unimpl_csr_read("mip");
    return ExecResult::illegal_inst();
  }

  static ExecResult do_write(CoreModel& core, uint32_t value) {
    (void)(core);
    (void)(value);
    unimpl_csr_write("mip");
    return ExecResult::illegal_inst();
  }

  static constexpr bool INSPECTABLE = false;
  static uint32_t do_inspect(CoreModel const& core);
};

struct CSR_MENVCFG {
  static constexpr uint16_t INDEX = 0x30a;
  static constexpr bool WRITABLE = true;

  static ExecResult do_read(CoreModel& core, uint32_t* ret) {
    (void)(core);
    (void)(ret);
    unimpl_csr_read("menvcfg");
    return ExecResult::illegal_inst();
  }

  static ExecResult do_write(CoreModel& core, uint32_t value) {
    (void)(core);
    (void)(value);
    unimpl_csr_write("menvcfg");
    return ExecResult::illegal_inst();
  }

  static constexpr bool INSPECTABLE = false;
  static uint32_t do_inspect(CoreModel const& core);
};

struct CSR_MENVCFGH {
  static constexpr uint16_t INDEX = 0x31a;
  static constexpr bool WRITABLE = true;

  static ExecResult do_read(CoreModel& core, uint32_t* ret) {
    (void)(core);
    (void)(ret);
    unimpl_csr_read("menvcfgh");
    return ExecResult::illegal_inst();
  }

  static ExecResult do_write(CoreModel& core, uint32_t value) {
    (void)(core);
    (void)(value);
    unimpl_csr_write("menvcfgh");
    return ExecResult::illegal_inst();
  }

  static constexpr bool INSPECTABLE = false;
  static uint32_t do_inspect(CoreModel const& core);
};

struct CSR_MSECCFG {
  static constexpr uint16_t INDEX = 0x747;
  static constexpr bool WRITABLE = true;

  static ExecResult do_read(CoreModel& core, uint32_t* ret) {
    (void)(core);
    (void)(ret);
    unimpl_csr_read("mseccfg");
    return ExecResult::illegal_inst();
  }

  static ExecResult do_write(CoreModel& core, uint32_t value) {
    (void)(core);
    (void)(value);
    unimpl_csr_write("mseccfg");
    return ExecResult::illegal_inst();
  }

  static constexpr bool INSPECTABLE = false;
  static uint32_t do_inspect(CoreModel const& core);
};

struct CSR_MSECCFGH {
  static constexpr uint16_t INDEX = 0x757;
  static constexpr bool WRITABLE = true;

  static ExecResult do_read(CoreModel& core, uint32_t* ret) {
    (void)(core);
    (void)(ret);
    unimpl_csr_read("mseccfgh");
    return ExecResult::illegal_inst();
  }

  static ExecResult do_write(CoreModel& core, uint32_t value) {
    (void)(core);
    (void)(value);
    unimpl_csr_write("mseccfgh");
    return ExecResult::illegal_inst();
  }

  static constexpr bool INSPECTABLE = false;
  static uint32_t do_inspect(CoreModel const& core);
};
