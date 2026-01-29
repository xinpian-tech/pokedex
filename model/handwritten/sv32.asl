// This file depends on states_s_mode.asl

constant SV32_PAGE_SIZE : integer{4096} = 4096;
constant SV32_MAX_LVL : integer{2} = 2;
constant SV32_PTE_SIZE : integer{4} = 4;

func Sv32Walk(addr : bits(32)) => bits(32)
begin
  
end

enumeration VirtAddrTransState {
  VATS_OK,
  VATS_Bare,
  VATS_PageFault,
  VATS_AccessFault;
};

record VirtAddrTransResult {
  paddr : bits(32);
  status : VirtAddrTransState;
};

func Translate(addr : bits(32), access_type : AccessType) => VirtAddrTransResult
begin
  // Address in M mode are all physical address
  if CURRENT_PRIVILEGE == PRIV_MODE_M then
    return VirtAddrTransResult {
      paddr : addr,
      status : VATS_Bare,
    };
  end

  // Current translate mode is bare
  if UInt(SATP_MODE) == 0 then
    return VirtAddrTransResult {
      paddr : addr,
      status : VATS_Bare,
    };
  end

  constant vpn0_lo : integer{12} = 12;
  constant vpn_len : integer{10} = 10;
  var a = UInt(SATP_PPN) * SV32_PAGE_SIZE;
  var i = SV32_MAX_LVL - 1;
  while i >= 0 do
    let vpn_i : bits(10) = addr[(vpn0_lo + 9) + (vpn_len * i) : vpn0_lo + (vpn_len * i)];
    let pte_addr : bits(32) = (a + (UInt(vpn_i) * SV32_PTE_SIZE))[31:0];
    // TODO PMP PMA

    let (pte, result) = ReadMemory(pte_addr, 32);
    if !result.is_ok then
      return VirtAddrTransResult {
        paddr : addr,
        status : VATS_AccessFault,
      };
    end

    let pte_v = pte[0];
    let pte_r = pte[1];
    let pte_w = pte[2];
    let pte_x = pte[3];
    let pte_u = pte[4];
    let pte_g = pte[5];
    let pte_a = pte[6];
    let pte_d = pte[7];
    let ppn_0 = pte[19:10];
    let ppn_1 = pte[31:20];

    if pte_v == '0' || (pte_r == '0' && pte_w == '1') || (pte_x == '0' && pte_w == '1' && pte_r == '0') then
      return VirtAddrTransResult {
        paddr = addr,
        status = VATS_PageFault,
      };
    end

    if pte_r != '0' || pte_x != '0' then
      // Misaligned super page
      if i > 0 && UInt(ppn_0) != 0 then
        return VirtAddrTransResult {
          paddr = addr,
          status = VATS_PageFault,
        };
      end

      if CURRENT_PRIVILEGE == PRIV_MODE_U && pte_u == '0' then
        return VirtAddrTransResult {
          paddr = addr,
          status = VATS_PageFault,
        };
      end

      if CURRENT_PRIVILEGE == PRIV_MODE_S && pte_u == '1' then
        // access user mode memory in supervisor mode depends on mstatus.SUM
        if MSTATUS_SUM == '0' then
          return VirtAddrTransResult {
            paddr = addr,
            status = VATS_PageFault,
          };
        end
      end

      case access_type of
        when AT_Fetch where pte_x == '0' =>
          return VirtAddrTransResult {
            paddr = addr,
            status = VATS_PageFault,
          };
        when AT_Load where pte_r == '0' && !(pte_x == '1' && MSTATUS_MXR == '1') =>
          return VirtAddrTransResult {
            paddr = addr,
            status = VATS_PageFault,
          };
        when AT_Store where pte_w == '0' =>
          return VirtAddrTransResult {
            paddr = addr,
            status = VATS_PageFault,
          };
      end

      // not accessible or store before make dirty
      if pte_a == '0' || access_type == AT_Store && pte_d == '0' then
        return VirtAddrTransResult {
          paddr = addr,
          status = VATS_PageFault,
        };
      end

      // FIXME: change to 34-bit if the bus support it
      var paddr : bits(32) = Zeros(32);
      // pa.PGOF = va.PGOF
      paddr[11:0] = addr[11:0];
      // FIXME: now we just trim the top 2-bit for 32-bit bus
      paddr[31:12] = pte[29:10];
      if i > 0 then
        paddr[21:12] = addr[21:12];
      end
      return VirtAddrTransResult {
        paddr = paddr,
        status = VATS_OK,
      };
    end

    i -= 1;

    if i < 0 then
      return VirtAddrTransResult {
        paddr = addr,
        status = VATS_PageFault,
      };
    else
      a = UInt(pte[31:10]) * SV32_PAGE_SIZE;
    end
  end
end

func PageTableWalk(addr : bits(32)) => bits(32)
begin
  return Sv32Walk(addr);
end
