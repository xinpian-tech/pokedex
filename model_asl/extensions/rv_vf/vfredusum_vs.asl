
func Execute_VFREDUSUM_VS(instruction: bits(32)) => Result
begin
  if !isEnabled_VS() then
    return IllegalInstruction();
  end
  if VTYPE.ill then
    return IllegalInstruction();
  end
  if !IsZero(VSTART) then
    // required by the spec
    return IllegalInstruction();
  end

  if !isEnabled_FS() then
    return IllegalInstruction();
  end
  let (rm: RM, rm_valid: boolean) = getFrmDynamic();
  if !rm_valid then
    return IllegalInstruction();
  end

  let vd: VRegIdx = UInt(GetRD(instruction));
  let vs2: VRegIdx = UInt(GetRS2(instruction));
  let vs1: VRegIdx = UInt(GetRS1(instruction));
  let vm: bit = GetVM(instruction);

  let vl: integer = VL;
  let sew: integer{8, 16, 32, 64} = VTYPE.sew;
  let vreg_align: integer{1, 2, 4, 8} = getAlign(VTYPE);

  if vs2 MOD vreg_align != 0 then
    // vs2 is not aligned with lmul group
    return IllegalInstruction();
  end

  var fflags: bits(5) = Zeros(5);
  case sew of
    when 8 => begin
      return IllegalInstruction();
    end

    when 16 => begin
      return IllegalInstruction();
    end

    when 32 => begin
      if vl != 0 then
        // the spec mandates it do nothing when vl == 0

        // impl defined behavior: it mimics Spike's behavior.
        // - Same as ordered sum when some element is active
        // - Canonicalizes vs1[0] when no element is active

        var acc: bits(32) = VRF_32[vs1, 0];
        for idx = 0 to vl - 1 do
          if vm != '0' || V0_MASK[idx] then
            let src2: bits(32) = VRF_32[vs2, idx];
            let res: F32_Flags = riscv_f32_add(rm, acc, src2);
            acc = res.value;
            fflags = fflags OR res.fflags;
          end
        end

        // when some element is active, acc is guaranteed to be nan-canonicalized,
        // and there's no harm to canonicalize it again.
        if f32_isNan(acc) then
          if f32_isSignalingNan(acc) then
            fflags = fflags OR '10000'; // raise invalid flag
          end
          acc = F32_CANONICAL_NAN;
        end

        VRF_32[vd, 0] = acc;
      end
    end

    when 64 => Todo("support sew=64");

    otherwise => Unreachable();
  end

  if vl != 0 then
    logWrite_VREG_1(vd);

    accureFFlags(fflags);
    makeDirty_FS_VS();
  end

  clear_VSTART();
  PC = PC + 4;
  return Retired();
end
