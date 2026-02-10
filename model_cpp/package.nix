{
  lib,
  stdenv,
  python3,
  meson,
  ninja,
  berkeley-softfloat,
  pokedex-configs,
}:
stdenv.mkDerivation {
  name = "pokedex-model-cpp";
  src =
    with lib.fileset;
    toSource {
      root = ./.;
      fileset = unions [
        ./model
        ./model_helper.h
        ./pokedex_util.h
        ./pokedex_interface.cpp

        ./data_files
        ./gen_dispatch.py

        ./meson.build
        ./meson.options
      ];
    };

  nativeBuildInputs = [
    python3
    meson
    ninja
  ];

  env = {

    SOFTFLOAT_RISCV_INCLUDE = "${berkeley-softfloat}/include";

    SOFTFLOAT_RISCV_LIB = "${berkeley-softfloat}/lib/libsoftfloat.a";
  }
  // lib.optionalAttrs (pokedex-configs != null) {
    POKEDEX_CONFIG = "${pokedex-configs.src}";
  };

  mesonFlags = [
    # Do not let model depend on other parts of pokedex in nix build,
    # therefore directly pull the include directory.
    "-Dpokedex_include_dir=${../simulator/include}"
  ];
}
