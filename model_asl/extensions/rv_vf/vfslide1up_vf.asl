// vfslide1up.vf vd, vs2, fs1, vm
// eew(vd, vs2) = sew, w(fs1) = sew
// vd[0] = F[fs1], vd[i+1] = vs2[i], optionally masked by vm, vm is mask for vd (including vd[0])
//
// NOTE: this instructions requires a valid FRM even if it's not used.
func Execute_VFSLIDE1UP_VF(instruction: bits(32)) => Result
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
  let fs1: XRegIdx = UInt(GetRS1(instruction));
  let vm: bit = GetVM(instruction);

  let vlmax: integer = VLMAX;
  let vl: integer = VL;
  let sew: integer{8, 16, 32, 64} = VTYPE.sew;
  let vreg_align: integer{1, 2, 4, 8} = getAlign(VTYPE);

  if vm == '0' && vd == 0 then
    // vd overlap with mask
    return IllegalInstruction();
  end
  if vd MOD vreg_align != 0 then
    // vd is not aligned with lmul group
    return IllegalInstruction();
  end
  if vs2 MOD vreg_align != 0 then
    // vs1 is not aligned with lmul group
    return IllegalInstruction();
  end
  if vd == vs2 then
    // vslideup cannot overlap vd with vs
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
          if idx == 0 then
            VRF_32[vd, idx] = F[fs1];
          else
            VRF_32[vd, idx] = VRF_32[vs2, idx - 1];
          end
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
