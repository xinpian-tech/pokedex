cfg := "full"

[no-exit-message]
default:
  @just --choose

guidance:
  @echo "Compiling Document"
  @nix build '.#pokdex.{{cfg}}.docs.guidance'
  @echo "File store in ./result/doc.pdf"

build target:
  @echo "Building {{target}} with config {{cfg}}"
  @just _build-{{target}}

develop target:
  @echo "Entering {{target}} shell"
  @echo
  @just _develop-{{target}}

compile target:
  @just _compile-{{target}}


_build-model:
  @nix build '.#pokedex.{{cfg}}.model' -L --out-link result-model
  @echo "Result store in result-model/"

_build-simulator:
  @nix build '.#pokedex.simulator' -L --out-link result-simulator
  @echo "Result store in result-simulator/"


_develop-model:
  @cd model && nix develop '.#pokedex.{{cfg}}.model'

_develop-simulator:
  @cd simulator && nix develop '.#pokedex.simulator.shell'


_compile-model:
  @echo "Debug compiling ASL model with config {{cfg}}"
  @cd model && nix develop '.#pokedex.{{cfg}}.model' \
    -c bash -c 'source "$stdenv/setup" && runPhase configurePhase && runPhase buildPhase'

_compile-simulator:
  @echo "Debug compiling simulator"
  @cd simulator && nix develop '.#pokedex.simulator.shell' -c cargo build
