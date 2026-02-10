
struct CSR_FFLAGS {
  static constexpr uint16_t INDEX = 0x001;
  static constexpr bool WRITABLE = true;

  static ExecResult do_read(CoreModel& core, uint32_t* ret) {
    (void)(core);
    (void)(ret);
    unimpl_csr_read("fflags");
    return ExecResult::illegal_inst();
  }

  static ExecResult do_write(CoreModel& core, uint32_t value) {
    (void)(core);
    (void)(value);
    unimpl_csr_write("fflags");
    return ExecResult::illegal_inst();
  }

  static constexpr bool INSPECTABLE = false;
  static uint32_t do_inspect(CoreModel const& core);
};

struct CSR_FRM {
  static constexpr uint16_t INDEX = 0x002;
  static constexpr bool WRITABLE = true;

  static ExecResult do_read(CoreModel& core, uint32_t* ret) {
    (void)(core);
    (void)(ret);
    unimpl_csr_read("frm");
    return ExecResult::illegal_inst();
  }

  static ExecResult do_write(CoreModel& core, uint32_t value) {
    (void)(core);
    (void)(value);
    unimpl_csr_write("frm");
    return ExecResult::illegal_inst();
  }

  static constexpr bool INSPECTABLE = false;
  static uint32_t do_inspect(CoreModel const& core);
};

struct CSR_FCSR {
  static constexpr uint16_t INDEX = 0x003;
  static constexpr bool WRITABLE = true;

  static ExecResult do_read(CoreModel& core, uint32_t* ret) {
    (void)(core);
    (void)(ret);
    unimpl_csr_read("fcsr");
    return ExecResult::illegal_inst();
  }

  static ExecResult do_write(CoreModel& core, uint32_t value) {
    (void)(core);
    (void)(value);
    unimpl_csr_write("fcsr");
    return ExecResult::illegal_inst();
  }

  static constexpr bool INSPECTABLE = false;
  static uint32_t do_inspect(CoreModel const& core);
};

struct CSR_VSTART {
  static constexpr uint16_t INDEX = 0x008;
  static constexpr bool WRITABLE = true;

  static ExecResult do_read(CoreModel& core, uint32_t* ret) {
    (void)(core);
    (void)(ret);
    unimpl_csr_read("vstart");
    return ExecResult::illegal_inst();
  }

  static ExecResult do_write(CoreModel& core, uint32_t value) {
    (void)(core);
    (void)(value);
    unimpl_csr_write("vstart");
    return ExecResult::illegal_inst();
  }

  static constexpr bool INSPECTABLE = false;
  static uint32_t do_inspect(CoreModel const& core);
};

struct CSR_VXSAT {
  static constexpr uint16_t INDEX = 0x009;
  static constexpr bool WRITABLE = true;

  static ExecResult do_read(CoreModel& core, uint32_t* ret) {
    (void)(core);
    (void)(ret);
    unimpl_csr_read("vxsat");
    return ExecResult::illegal_inst();
  }

  static ExecResult do_write(CoreModel& core, uint32_t value) {
    (void)(core);
    (void)(value);
    unimpl_csr_write("vxsat");
    return ExecResult::illegal_inst();
  }

  static constexpr bool INSPECTABLE = false;
  static uint32_t do_inspect(CoreModel const& core);
};

struct CSR_VXRM {
  static constexpr uint16_t INDEX = 0x00a;
  static constexpr bool WRITABLE = true;

  static ExecResult do_read(CoreModel& core, uint32_t* ret) {
    (void)(core);
    (void)(ret);
    unimpl_csr_read("vxrm");
    return ExecResult::illegal_inst();
  }

  static ExecResult do_write(CoreModel& core, uint32_t value) {
    (void)(core);
    (void)(value);
    unimpl_csr_write("vxrm");
    return ExecResult::illegal_inst();
  }

  static constexpr bool INSPECTABLE = false;
  static uint32_t do_inspect(CoreModel const& core);
};

struct CSR_VCSR {
  static constexpr uint16_t INDEX = 0x00f;
  static constexpr bool WRITABLE = true;

  static ExecResult do_read(CoreModel& core, uint32_t* ret) {
    (void)(core);
    (void)(ret);
    unimpl_csr_read("vcsr");
    return ExecResult::illegal_inst();
  }

  static ExecResult do_write(CoreModel& core, uint32_t value) {
    (void)(core);
    (void)(value);
    unimpl_csr_write("vcsr");
    return ExecResult::illegal_inst();
  }

  static constexpr bool INSPECTABLE = false;
  static uint32_t do_inspect(CoreModel const& core);
};

struct CSR_VL {
  static constexpr uint16_t INDEX = 0xc20;
  static constexpr bool WRITABLE = false;

  static ExecResult do_read(CoreModel& core, uint32_t* ret) {
    (void)(core);
    (void)(ret);
    unimpl_csr_read("vl");
    return ExecResult::illegal_inst();
  }

  static constexpr bool INSPECTABLE = false;
  static uint32_t do_inspect(CoreModel const& core);
};

struct CSR_VTYPE {
  static constexpr uint16_t INDEX = 0xc21;
  static constexpr bool WRITABLE = false;

  static ExecResult do_read(CoreModel& core, uint32_t* ret) {
    (void)(core);
    (void)(ret);
    unimpl_csr_read("vtype");
    return ExecResult::illegal_inst();
  }

  static constexpr bool INSPECTABLE = false;
  static uint32_t do_inspect(CoreModel const& core);
};

struct CSR_VLENB {
  static constexpr uint16_t INDEX = 0xc22;
  static constexpr bool WRITABLE = false;

  static ExecResult do_read(CoreModel& core, uint32_t* ret) {
    (void)(core);
    (void)(ret);
    unimpl_csr_read("vlenb");
    return ExecResult::illegal_inst();
  }

  static constexpr bool INSPECTABLE = false;
  static uint32_t do_inspect(CoreModel const& core);
};

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
