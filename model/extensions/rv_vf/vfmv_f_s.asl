// vfmv.f.s rd, vs2
// eew(vs1) = sew, emul(vs1) = 1
// compute F[fd] = vs2[0]
//
// NOTE: this instructions requires a valid FRM even if it's not used.
func Execute_VFMV_F_S(instruction: bits(32)) => Result
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

  let fd: FRegIdx = UInt(GetRD(instruction));
  let vs2: VRegIdx = UInt(GetRS2(instruction));

  let sew: integer{8, 16, 32, 64} = VTYPE.sew;
  // This instruction explicitly ignores lmul

  case sew of
    when 8 => begin
      return IllegalInstruction();
    end

    when 16 => begin
      return IllegalInstruction();
    end

    when 32 => begin
      let src : bits(32) = VRF_32[vs2, 0];
      F[fd] = src;
    end

    when 64 => Todo("support sew=64");

    otherwise => Unreachable();
  end

  makeDirty_FS();
  // no makeDirty_VS;
  clear_VSTART();
  PC = PC + 4;
  return Retired();
end
