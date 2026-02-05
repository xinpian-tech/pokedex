// vfclass.v vd, vs2, vm; vm mask is optional
// eew(vd, vs2) = sew
//
// NOTE: this instructions requires a valid FRM even if it's not used.
func Execute_VFCLASS_V(instruction: bits(32)) => Result
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
  let vs2: VRegIdx = UInt(GetRS2(instruction));
  let vm: bit = GetVM(instruction);

  let vl: integer = VL;
  let sew: integer{8, 16, 32, 64} = VTYPE.sew;
  let vreg_align: integer{1, 2, 4, 8} = getAlign(VTYPE);

  if vm == '0' && vd == 0 then
    // overlap with mask
    return IllegalInstruction();
  end
  if vd MOD vreg_align != 0 then
    // vd is not aligned with lmul group
    return IllegalInstruction();
  end
  if vs2 MOD vreg_align != 0 then
    // vs2 is not aligned with elmul group
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
      for idx = 0 to vl - 1 do
        if vm != '0' || V0_MASK[idx] then
          let src2: bits(32) = VRF_32[vs2, idx];
          let mask: bits(10) = riscv_fclass_f32(src2);
          VRF_32[vd, idx] = ZeroExtend(mask, 32);
        end
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

