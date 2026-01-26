// vfmv.s.f vd, fs1
// eew(vd) = sew, emul(vd) = 1
// compute vd[0] = F[fs1]
//
// NOTE: this instructions requires a valid FRM even if it's not used.
func Execute_VFMV_S_F(instruction: bits(32)) => Result
begin
  if !isEnabled_VS() then
    return IllegalInstruction();
  end
  if VTYPE.ill then
    return IllegalInstruction();
  end
  if !IsZero(VSTART) then
    return IllegalInstruction();
  end

  if !isEnabled_FS() then
    return IllegalInstruction();
  end
  let (_rm: RM, rm_valid: boolean) = getFrmDynamic();
  if !rm_valid then
    return IllegalInstruction();
  end

  let vd: VRegIdx = UInt(GetRD(instruction));
  let fs1: FRegIdx = UInt(GetRS1(instruction));

  let sew: integer{8, 16, 32, 64} = VTYPE.sew;
  // This instruction explicitly ignores lmul

  // do nothing if vl == 0
  if VL != 0 then
    case sew of
      when 8 => begin
        return IllegalInstruction();
      end

      when 16 => begin
        return IllegalInstruction();
      end

      when 32 => begin
        let src: bits(32) = F[fs1];
        VRF_32[vd, 0] = src;
      end

      when 64 => Todo("support sew=64");

      otherwise => Unreachable();
    end
  end

  logWrite_VREG_1(vd);

  makeDirty_VS();
  clear_VSTART();
  PC = PC + 4;
  return Retired();
end
