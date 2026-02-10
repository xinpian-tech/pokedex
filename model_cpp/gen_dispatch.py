#!/usr/bin/env python3
"""Generate instruction dispatch code from YAML instruction definitions."""

import argparse
import re
import os
import yaml
from dataclasses import dataclass
from pathlib import Path

PRIV_PATTERN = re.compile(r"^[uhsm]r[wo]$")


@dataclass
class Inst:
    name: str
    encoding: str


@dataclass
class InstGroup:
    name: str
    conditions: list[str]
    instructions: list[Inst]


@dataclass
class Csr:
    name: str
    priv: str  # e.g. "urw", "mro", "uro"
    idx: int
    conditions: list[str]

    @property
    def writable(self):
        return self.priv[2] == "w"


@dataclass
class CsrGroup:
    name: str
    conditions: list[str]
    csrs: list[Csr]


CONDITIONS = {
    "rv32": "CoreModel::XLEN == 32",
    "rv64": "CoreModel::XLEN == 64",
    "a": "true",
    "c": "true",
    "f": "CoreModel::FLEN >= 32",
    "d": "CoreModel::FLEN >= 64",
    "v": "CoreModel::VLEN > 0",
    "mmode": "true",
    "smode": "false",
}


def validate_encoding(
    encoding: str, expected_bits: int, inst_name: str, group_name: str
):
    """Validate an encoding string has correct length and only valid characters."""
    if len(encoding) != expected_bits:
        raise ValueError(
            f"{group_name}/{inst_name}: encoding length {len(encoding)}, expected {expected_bits}"
        )
    invalid = set(encoding) - {"0", "1", "-"}
    if invalid:
        raise ValueError(
            f"{group_name}/{inst_name}: invalid characters in encoding: {invalid}"
        )


def format_encoding(encoding: str) -> tuple[int, int]:
    """Convert encoding string to (mask, base) values.

    Fixed bits ('0' and '1') set the corresponding mask bit to 1.
    Don't-care bits ('-') set the corresponding mask bit to 0.
    The base value has the actual bit values where the mask is 1.
    """
    mask_str = encoding.replace("0", "1").replace("-", "0")
    base_str = encoding.replace("-", "0")
    mask = int(mask_str, 2)
    base = int(base_str, 2)
    return mask, base


def extract_instructions(yaml_data: dict, expected_bits: int) -> list[InstGroup]:
    """Extract and validate all instruction groups from YAML data."""
    inst_groups = []

    for group_name, group_data in yaml_data.items():
        if not isinstance(group_data, dict) or "instructions" not in group_data:
            raise ValueError(
                f"group '{group_name}': expected dict with 'instructions' key"
            )
        if "conditions" not in group_data:
            raise ValueError(f"group '{group_name}': missing 'conditions' key")

        conditions = group_data["conditions"]
        if not isinstance(conditions, list):
            raise ValueError(f"group '{group_name}': 'conditions' must be a list")
        for cond in conditions:
            if cond not in CONDITIONS:
                raise ValueError(
                    f"group '{group_name}': unknown condition '{cond}' "
                    f"(known: {', '.join(CONDITIONS)})"
                )

        instructions = []
        for inst in group_data["instructions"]:
            if "name" not in inst:
                raise ValueError(f"group '{group_name}': instruction missing 'name'")
            if "encoding" not in inst:
                raise ValueError(
                    f"group '{group_name}/{inst['name']}': missing 'encoding'"
                )
            validate_encoding(inst["encoding"], expected_bits, inst["name"], group_name)
            instructions.append(Inst(name=inst["name"], encoding=inst["encoding"]))

        if instructions:
            inst_groups.append(
                InstGroup(
                    name=group_name,
                    conditions=conditions,
                    instructions=instructions,
                )
            )

    return inst_groups


def gen_unimpl(
    inst_groups: list[InstGroup], is_compressed: bool, unimpl_set: set[str]
) -> str:
    """Generate unimplemented instruction macro definitions.

    Only emits macros for instructions listed in unimpl_set.
    Deduplicates macros for instructions appearing in multiple groups
    (e.g. slli in both I_rv32 and I_rv64).
    """
    unimpl_func = "unimpl_cinst" if is_compressed else "unimpl_inst"
    lines = []
    seen = set()
    for inst_group in inst_groups:
        for inst in inst_group.instructions:
            if inst.name not in unimpl_set:
                continue
            macro_name = f"EXECUTE_{inst.name.upper()}"
            if macro_name in seen:
                continue
            seen.add(macro_name)
            display_name = inst.name.replace("_", ".")
            lines += [
                f'#define {macro_name}(core, inst, mem, npc) {unimpl_func}("{display_name}")'
            ]
    return "\n".join(lines) + "\n"


def gen_do_execute(inst_groups: list[InstGroup], is_compressed: bool) -> str:
    """Generate a C++ instruction dispatch function."""
    if is_compressed:
        func_name = "do_execute_c"
        inst_type = "CInst"
        hex_width = 4
    else:
        func_name = "do_execute"
        inst_type = "Inst"
        hex_width = 8

    fmt = f"0x{{:0{hex_width}x}}"
    lines = []

    lines += [
        "// Auto-generated by gen_do_execute.py",
        "",
        f"ExecResult {func_name}(CoreModel& core, {inst_type} inst, MemCallback mem, NextPc& npc) {{",
    ]

    # Only emit condition variables that are actually referenced
    used_conditions = set()
    for ig in inst_groups:
        used_conditions.update(ig.conditions)
    for name, expr in CONDITIONS.items():
        if name in used_conditions:
            lines += [f"  constexpr bool ext_{name} = {expr};"]

    for inst_group in inst_groups:
        if len(inst_group.conditions) == 0:
            cond_expr = "true"
        else:
            cond_expr = " && ".join(f"ext_{x}" for x in inst_group.conditions)
        lines += [
            "",
            f"  // inst group: {inst_group.name}",
            f"  if constexpr ({cond_expr}) {{",
        ]
        for inst in inst_group.instructions:
            display_name = inst.name.replace("_", ".")
            mask, base = format_encoding(inst.encoding)
            lines += [
                f"    if (inst.match({fmt.format(mask)}, {fmt.format(base)})) {{",
                f"      // {display_name}",
                f"      return EXECUTE_{inst.name.upper()}(core, inst, mem, npc);",
                f"    }}",
            ]
        lines += ["  }"]

    lines += [
        "",
        "  return ExecResult::illegal_inst();",
        "}",
    ]
    return "\n".join(lines) + "\n"


def extract_csrs(yaml_data: dict) -> list[CsrGroup]:
    """Extract and validate all CSR groups from YAML data."""
    csr_groups = []

    for group_name, group_data in yaml_data.items():
        if not isinstance(group_data, dict) or "csr" not in group_data:
            raise ValueError(f"group '{group_name}': expected dict with 'csr' key")
        if "conditions" not in group_data:
            raise ValueError(f"group '{group_name}': missing 'conditions' key")

        conditions = group_data["conditions"]
        if not isinstance(conditions, list):
            raise ValueError(f"group '{group_name}': 'conditions' must be a list")
        for cond in conditions:
            if cond not in CONDITIONS:
                raise ValueError(
                    f"group '{group_name}': unknown condition '{cond}' "
                    f"(known: {', '.join(CONDITIONS)})"
                )

        csrs = []
        for csr_name, csr_data in group_data["csr"].items():
            if "priv" not in csr_data:
                raise ValueError(f"group '{group_name}/{csr_name}': missing 'priv'")
            if "idx" not in csr_data:
                raise ValueError(f"group '{group_name}/{csr_name}': missing 'idx'")
            priv = csr_data["priv"]
            if not PRIV_PATTERN.match(priv):
                raise ValueError(
                    f"group '{group_name}/{csr_name}': invalid priv '{priv}' "
                    f"(expected [uhsm]r[wo])"
                )
            csr_conditions = csr_data.get("conditions", [])
            if not isinstance(csr_conditions, list):
                raise ValueError(
                    f"group '{group_name}/{csr_name}': 'conditions' must be a list"
                )
            for cond in csr_conditions:
                if cond not in CONDITIONS:
                    raise ValueError(
                        f"group '{group_name}/{csr_name}': unknown condition '{cond}' "
                        f"(known: {', '.join(CONDITIONS)})"
                    )
            csrs.append(
                Csr(
                    name=csr_name,
                    priv=priv,
                    idx=csr_data["idx"],
                    conditions=csr_conditions,
                )
            )

        if csrs:
            csr_groups.append(
                CsrGroup(
                    name=group_name,
                    conditions=conditions,
                    csrs=csrs,
                )
            )

    return csr_groups


def gen_unimpl_csr(csr_groups: list[CsrGroup], unimpl_set: set[str]) -> str:
    """Generate unimplemented CSR struct definitions.

    Only emits structs for CSRs listed in unimpl_set.
    """
    lines = []
    for csr_group in csr_groups:
        for csr in csr_group.csrs:
            if csr.name not in unimpl_set:
                continue
            upper = csr.name.upper()

            lines += [
                "",
                f"struct CSR_{upper} {{",
                f"  static constexpr uint16_t INDEX = 0x{csr.idx:03x};",
                f"  static constexpr bool WRITABLE = {'true' if csr.writable else 'false'};",
                "",
                "  static ExecResult do_read(CoreModel& core, uint32_t* ret) {",
                "    (void)(core);",
                "    (void)(ret);",
                f'    unimpl_csr_read("{csr.name}");',
                "    return ExecResult::illegal_inst();",
                "  }",
            ]
            if csr.writable:
                lines += [
                    "",
                    "  static ExecResult do_write(CoreModel& core, uint32_t value) {",
                    "    (void)(core);",
                    "    (void)(value);",
                    f'    unimpl_csr_write("{csr.name}");',
                    "    return ExecResult::illegal_inst();",
                    "  }",
                    # '',
                    # '  static ExecResult do_rmw([[maybe_unused]] CoreModel& core, CsrOp op, uint32_t value, uint32_t* ret) {',
                    # '    (void)(op);',
                    # '    (void)(value);',
                    # '    (void)(ret);',
                    # f'    unimpl_csr_write("{csr.name}");',
                    # '    return ExecResult::illegal_inst();',
                    # '  }',
                ]
            lines += [
                "",
                "  static constexpr bool INSPECTABLE = false;",
                "  static uint32_t do_inspect(CoreModel const& core);",
                "};",
            ]

    return "\n".join(lines) + "\n"


def gen_do_csr(csr_groups: list[CsrGroup]) -> str:
    """Generate CSR read/write/rmw dispatch functions."""
    lines = []

    # Collect all referenced conditions (group-level + per-CSR)
    used_conditions = set()
    for cg in csr_groups:
        used_conditions.update(cg.conditions)
        for csr in cg.csrs:
            used_conditions.update(csr.conditions)

    cond_lines = []
    for name, expr in CONDITIONS.items():
        if name in used_conditions:
            cond_lines += [f"  constexpr bool ext_{name} = {expr};"]

    def csr_cond_expr(csr: Csr) -> str | None:
        """Build a per-CSR if constexpr expression, or None if no per-CSR conditions."""
        if not csr.conditions:
            return None
        return " && ".join(f"ext_{x}" for x in csr.conditions)

    def emit_csr_read_body(csr: Csr, indent: str) -> list[str]:
        """Emit the body of a CSR read case."""
        return [
            f"{indent}// {csr.name}",
            f"{indent}return CSR_{csr.name.upper()}::do_read(core, ret);",
        ]

    def emit_csr_write_body(csr: Csr, indent: str) -> list[str]:
        """Emit the body of a CSR write case."""
        return [
            f"{indent}// {csr.name}",
            f"{indent}return CSR_{csr.name.upper()}::do_write(core, value);",
        ]

    def emit_csr_dispatch(csr: Csr, body_fn, base_indent: str) -> list[str]:
        """Emit an if (csr.idx == ...) block, wrapped in if constexpr when per-CSR conditions exist."""
        per_csr_cond = csr_cond_expr(csr)
        if per_csr_cond is not None:
            return [
                f"{base_indent}if constexpr ({per_csr_cond}) {{",
                f"{base_indent}  if (csr.idx == 0x{csr.idx:03x}) {{",
                *body_fn(csr, base_indent + "    "),
                f"{base_indent}  }}",
                f"{base_indent}}}",
            ]
        else:
            return [
                f"{base_indent}if (csr.idx == 0x{csr.idx:03x}) {{",
                *body_fn(csr, base_indent + "  "),
                f"{base_indent}}}",
            ]

    lines += ["// Auto-generated by gen_do_execute.py", ""]

    # --- do_csr_read ---
    lines += ["ExecResult do_csr_read(CoreModel& core, CsrIdx csr, uint32_t* ret) {"]
    lines += cond_lines

    for csr_group in csr_groups:
        if len(csr_group.conditions) == 0:
            group_cond_expr = "true"
        else:
            group_cond_expr = " && ".join(f"ext_{x}" for x in csr_group.conditions)
        lines += [
            "",
            f"  // CSR group: {csr_group.name}",
            f"  if constexpr ({group_cond_expr}) {{",
        ]
        for csr in csr_group.csrs:
            lines += emit_csr_dispatch(csr, emit_csr_read_body, "    ")
        lines += ["  }"]

    lines += [
        "",
        "  return ExecResult::illegal_inst();",
        "}",
        "",
    ]

    # --- do_csr_write ---
    lines += ["ExecResult do_csr_write(CoreModel& core, CsrIdx csr, uint32_t value) {"]
    lines += cond_lines

    for csr_group in csr_groups:
        if len(csr_group.conditions) == 0:
            group_cond_expr = "true"
        else:
            group_cond_expr = " && ".join(f"ext_{x}" for x in csr_group.conditions)
        lines += [
            "",
            f"  // CSR group: {csr_group.name}",
            f"  if constexpr ({group_cond_expr}) {{",
        ]
        for csr in csr_group.csrs:
            if csr.writable:
                lines += emit_csr_dispatch(csr, emit_csr_write_body, "    ")
        lines += ["  }"]

    lines += [
        "",
        "  return ExecResult::illegal_inst();",
        "}",
    ]

    # --- do_csr_rmw ---
    # lines += [
    #     'ExecResult do_csr_rmw(CoreModel& core, CsrIdx csr, CsrOp op, uint32_t value, uint32_t* ret) {',
    #     '  ExecResult res = do_csr_read(core, csr, ret);',
    #     '  if (!res.is_ok()) return res;',
    #     '',
    #     '  uint32_t new_value;',
    #     '  switch (op) {',
    #     '    case CsrOp::READ_WRITE: new_value = value; break;',
    #     '    case CsrOp::READ_SET:   new_value = *ret | value; break;',
    #     '    case CsrOp::READ_CLEAR: new_value = *ret & ~value; break;',
    #     '  }',
    #     '',
    #     '  return do_csr_write(core, csr, new_value);',
    #     '}',
    # ]

    # --- do_csr_inspetc ---
    lines += [
        "",
        "uint32_t do_csr_inspect(CoreModel const& core, CsrIdx csr) {",
    ]

    for csr_group in csr_groups:
        for csr in csr_group.csrs:
            upper_name = csr.name.upper()

            lines += [
                f"  if constexpr (CSR_{upper_name}::INSPECTABLE) {{",
                f"    if (csr.idx == 0x{csr.idx:03x}) {{",
                f"      return CSR_{upper_name}::do_inspect(core);",
                f"    }}",
                f"  }}",
            ]

    lines += [
        "",
        "  unimpl_csr_inspect(csr);",
        "}",
    ]

    return "\n".join(lines) + "\n"


def write_file(path: Path, content: str):
    path.parent.mkdir(parents=True, exist_ok=True)
    with open(path, "w") as f:
        f.write(content)


def load_and_extract(
    script_dir: Path, yaml_file: str, is_compressed: bool
) -> list[InstGroup]:
    """Load a YAML file and extract validated instruction groups."""
    expected_bits = 16 if is_compressed else 32
    with open(script_dir / yaml_file) as f:
        yaml_data = yaml.safe_load(f)
    return extract_instructions(yaml_data, expected_bits)


def main():
    parser = argparse.ArgumentParser(
        description="Generate instruction dispatch code from YAML definitions",
    )
    parser.add_argument("-d", "--data", default="data_files")
    parser.add_argument(
        "-o", "--output", default="model/_generated", help="Output C++ directory"
    )
    args = parser.parse_args()

    data_dir = Path(args.data)
    output_dir = Path(args.output)

    # Load unimpl.yaml to know which instructions/CSRs are not yet implemented
    with open(data_dir / "unimpl.yaml") as f:
        unimpl_data = yaml.safe_load(f)
    unimpl_inst_set = set(unimpl_data.get("inst", []))
    unimpl_inst_c_set = set(unimpl_data.get("inst_c", []))
    unimpl_csr_set = set(unimpl_data.get("csr", []))

    # Compressed instructions (16-bit, rvc.yaml)
    rvc_data = load_and_extract(data_dir, "rvc.yaml", is_compressed=True)
    write_file(
        output_dir / "do_execute_c.inc", gen_do_execute(rvc_data, is_compressed=True)
    )
    write_file(
        output_dir / "unimpl_c.inc",
        gen_unimpl(rvc_data, is_compressed=True, unimpl_set=unimpl_inst_c_set),
    )

    # Non-compressed instructions (32-bit, rvi.yaml)
    rvi_groups = load_and_extract(data_dir, "rvi.yaml", is_compressed=False)
    rvf_groups = load_and_extract(data_dir, "rvf.yaml", is_compressed=False)
    rv_misc_groups = load_and_extract(data_dir, "rv_misc.yaml", is_compressed=False)
    rvi_data = rvi_groups + rvf_groups + rv_misc_groups
    write_file(
        output_dir / "do_execute.inc", gen_do_execute(rvi_data, is_compressed=False)
    )
    write_file(
        output_dir / "unimpl.inc",
        gen_unimpl(rvi_data, is_compressed=False, unimpl_set=unimpl_inst_set),
    )

    # CSR dispatch (from csr.yaml)
    with open(data_dir / "csr.yaml") as f:
        csr_yaml = yaml.safe_load(f)
    csr_groups = extract_csrs(csr_yaml)
    write_file(output_dir / "do_csr.inc", gen_do_csr(csr_groups))
    write_file(
        output_dir / "unimpl_csr.inc",
        gen_unimpl_csr(csr_groups, unimpl_set=unimpl_csr_set),
    )


if __name__ == "__main__":
    main()
