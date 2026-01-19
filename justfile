cfg := "full"
config_dir := ""
nix_args := if config_dir != "" { f"--override-input pokedex-configs path:{{config_dir}}" } else { "" }

[default]
[no-exit-message]
_default:
  @just --list --unsorted

# Build PDF guidance
guidance:
  @echo "Compiling Document"
  @nix {{nix_args}} build '.#pokdex.{{cfg}}.docs.guidance'
  @echo "File store in ./result/doc.pdf"

# Build Targets: model, simulator
build target:
  @echo "Building {{target}} with config {{cfg}}"
  @just _build-{{target}}

# Develop Targets: model, simulator
develop target:
  @echo "Entering {{target}} shell"
  @echo
  @just _develop-{{target}}

# Compile Targets: model, simulator
compile target:
  @just _compile-{{target}}


_build-model:
  @nix {{nix_args}} build '.#pokedex.{{cfg}}.model' -L --out-link result-model
  @echo "Result store in result-model/"

_build-simulator:
  @nix {{nix_args}} build '.#pokedex.simulator' -L --out-link result-simulator
  @echo "Result store in result-simulator/"


[working-directory: 'model']
_develop-model:
  @nix {{nix_args}} develop '.#pokedex.{{cfg}}.model'

[working-directory: 'simulator']
_develop-simulator:
  @nix {{nix_args}} develop '.#pokedex.simulator.shell'


[working-directory: 'model']
_compile-model:
  #!/usr/bin/env -S nix {{nix_args}} develop '.#pokedex.{{cfg}}.model' -c bash
  set -euo pipefail
  echo "Debug compiling ASL model with config {{cfg}}"
  source "$stdenv/setup"
  runPhase configurePhase
  runPhase buildPhase

[working-directory: 'simulator']
_compile-simulator:
  @echo "Debug compiling simulator"
  @nix {{nix_args}} develop '.#pokedex.simulator.shell' -c cargo build
