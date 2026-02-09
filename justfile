cfg := "zve32f"
config_dir := ""
nix_args := if config_dir != "" { f"--override-input pokedex-configs-src path:{{canonicalize(config_dir)}}" } else { "" }

[default]
[no-exit-message]
_default:
  @just --list --unsorted

# Build PDF guidance
guidance:
  @echo "Compiling Document"
  nix build {{nix_args}} '.#pokedex.{{cfg}}.docs.guidance'
  @echo "File store in ./result/doc.pdf"

# Build Targets: model, simulator
build target:
  @echo "Building {{target}} with config {{cfg}}"
  @just nix_args="{{nix_args}}" _build-{{target}}

# Develop Targets: model, simulator
develop target:
  @echo "Entering {{target}} shell"
  @echo
  @just nix_args="{{nix_args}}" _develop-{{target}}

# Compile Targets: model, simulator
compile target:
  @just nix_args="{{nix_args}}" _compile-{{target}}


[working-directory: 'tests']
run-test *args:
  #!/usr/bin/env -S nix develop {{nix_args}} '.#pokedex.{{cfg}}.tests.env' -c bash
  set -euo pipefail
  meson setup build_{{cfg}} $MESON_FLAGS --prefix $PWD/tests-output
  meson test -C build_{{cfg}} {{args}}


_build-model:
  @nix build {{nix_args}} '.#pokedex.{{cfg}}.model-asl' -L --out-link result-model
  @echo "Result store in result-model/"

_build-simulator:
  @nix build {{nix_args}} '.#pokedex.simulator' -L --out-link result-simulator
  @echo "Result store in result-simulator/"


[working-directory: 'model-asl']
_develop-model:
  @nix develop {{nix_args}} '.#pokedex.{{cfg}}.model-asl'

[working-directory: 'simulator']
_develop-simulator:
  @nix develop {{nix_args}} '.#pokedex.simulator.shell'


[working-directory: 'model-asl']
_compile-model:
  #!/usr/bin/env -S nix develop {{nix_args}} '.#pokedex.{{cfg}}.model-asl' -c bash
  set -euo pipefail
  echo
  echo "Debug compiling ASL model with config {{BG_BLUE}}{{BOLD}}{{cfg}}{{NORMAL}}"
  echo
  source "$stdenv/setup"
  runPhase configurePhase
  runPhase buildPhase

[working-directory: 'simulator']
_compile-simulator:
  @echo "Debug compiling simulator"
  @nix develop {{nix_args}} '.#pokedex.simulator.shell' -c cargo build
