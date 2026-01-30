// pmp_rv32.asl
// This file is solely designed for RV32 architecture.

var PMPCFG : array[64] of bits(8);
var PMPADDR : array[64] of bits(32);

func ResetPMP()
begin
  for i = 0 to 63 do
    PMPCFG[i] = Zeros(8);
    PMPADDR[i] = Zeros(32);
  end
end

func _FindPmpMatchIndex(addr_lo : bits(32), addr_hi : bits(32)) => (boolean, integer{0..63})
begin
  // we assume that addr_lo is never larger than addr_hi
  assert(UInt(addr_lo) <= UInt(addr_hi));

  for i = 0 to 63 do
    let cfg = PMPCFG[i];
    let pmpaddr = PMPADDR[i];

    // match PMPCFG[i].A
    case cfg[4:3] of
      // OFF: do nothing and continue to next match
      // when '00' => ();
      // TOR
      when '01' =>
        var pmp_lo = Zeros(32);
        if i != 0 then
          pmp_lo = PMPADDR[i-1];
        end

        if UInt(pmp_lo) <= UInt(addr_lo) then
          if UInt(addr_hi) < UInt(pmpaddr) then
            return (TRUE, i);
          else
            return (FALSE, i);
          end
        end
      // TODO: have a configurable PMP grain, and check granularity before PMP
      // TODO: fix following comparison if we support 34-bit bus
      // NA4, NAPOT
      when '10' =>
        if addr_lo[31:2] == pmpaddr[29:0] then
          if addr_hi[31:2] == pmpaddr[29:0] then
            return (TRUE, i);
          else
            return (FALSE, i);
          end
        end
      when '11' =>
        let mask = NOT (pmpaddr XOR (pmpaddr + 1));
        assert(pmpaddr[31:30] == '00');
        let masked_pmp = pmpaddr[29:0] AND mask[29:0];
        let addr_lo_in_range = (addr_lo[31:2] AND mask[29:0]) == masked_pmp;
        let addr_hi_in_range = (addr_hi[31:2] AND mask[29:0]) == masked_pmp;
        if addr_lo_in_range then
          if addr_hi_in_range then
            return (TRUE, i);
          else
            return (FALSE, i);
          end
        end
    end
  end

  return (FALSE, 63);
end

func CheckPMP(addr : bits(32), width: integer{8, 16, 32}, access_type: AccessType) => boolean
begin
  let (matched, i) = _FindPmpMatchIndex(addr, addr + width);
  if !matched then
    return CURRENT_PRIVILEGE == PRIV_MODE_M;
  end

  let pmpcfg = PMPCFG[i];
  var invalid = FALSE;
  case access_type of
    when AT_Load where pmpcfg[0] == '0' => invalid = TRUE;
    when AT_Store where pmpcfg[1] == '0' => invalid = TRUE;
    when AT_Fetch where pmpcfg[2] == '0' => invalid = TRUE;
  end

  if invalid then
    // PMP applied when current pmpcfg is locked or current privilege is S or U
    let is_locked = pmpcfg[7] == '1';
    return is_locked || CURRENT_PRIVILEGE != PRIV_MODE_M;
  end
end
