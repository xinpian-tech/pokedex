# Web Search

When you feel unconfident and you should ask the user, DO NOT use web search tool without permission.
Tell the user what you need. The user will either answer your question or provide relavant documents.

# Debug Difftest Failure

The meson build directory is at `tests/build_{{config}}`. e.g. When dealing with `zve32f` config, the build directory is at `tests/build_zve32f`.

Under build directory, you will see many `_diff_result.json`, `_pokedex_commit.jsonl`, and `_spike_commit.log` . Look at the `_diff_result.json` first, it tells you where the difftest fails. These files are prefixed by "suite name" dot "case name". E.g., `riscv-tests.rv32ua_amoadd_w_diff_result.json` denotes the difftest result of "riscv-tests" suite, "rv32ua_amoadd_w" case.

For the pokedex model source, you can find the at "model_cpp" directory. Spike is considered the golden standard and the source code is not available here. If you need spike source code, ask the user.

The source of case may not be available. However, you could find elf file in build dir, e.g. for suite "riscv-tests" case "rv32ua_amoadd_w", the elf is "riscv-tests/rv32ua_maoadd_w.elf" at meson build dir.
You may use `llvm-objdump` to disassemble it.

You should analyze the failed case, propose your fix, and discuss with user. Do not modify code directly before user's explicit instruction.

