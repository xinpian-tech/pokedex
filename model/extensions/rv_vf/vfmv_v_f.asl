// vfmv.v.f vd, fs1
// eew(vd, vs1) = sew, w(rs1) = sew
// compute vd[i] = F[fs1]
//
// NOTE: this instructions requires a valid FRM even if it's not used.
func Execute_VFMV_V_F(instruction: bits(32)) => Result
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

  let vl: integer = VL;
  let sew: integer{8, 16, 32, 64} = VTYPE.sew;
  let vreg_align: integer{1, 2, 4, 8} = getAlign(VTYPE);

  if vd MOD vreg_align != 0 then
    // vd is not aligned with lmul group
    return IllegalInstruction();
  end

  case sew of
    when 8 => begin
      return IllegalInstruction();
    end

    when 16 => begin
      return IllegalInstruction();
    end

    when 32 => begin
      let src: bits(32) = F[fs1];
      for idx = 0 to vl - 1 do
        VRF_32[vd, idx] = src;
      end
    end

    when 64 => Todo("support sew=64");

    otherwise => Unreachable();
  end

  logWrite_VREG_elmul(vd, vreg_align);

  makeDirty_VS();
  clear_VSTART();
  PC = PC + 4;
  return Retired();
end
