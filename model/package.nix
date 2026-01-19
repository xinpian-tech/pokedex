{
  lib,
  stdenv,
  riscv-opcodes-src,
  asl-interpreter,
  python3,
  ninja,
  minijinja,
  aslref,
  berkeley-softfloat,
  pokedex-configs,
}:
stdenv.mkDerivation {
  name = "pokedex-model";
  src =
    with lib.fileset;
    toSource {
      root = ./.;
      fileset = unions [
        ./aslbuild
        ./csr
        ./csrc
        ./data_files
        ./extensions
        ./handwritten
        ./scripts
        ./template
      ];
    };

  nativeBuildInputs = [
    asl-interpreter
    python3
    ninja
    minijinja
    aslref
  ];

  env = {
    RISCV_OPCODES_SRC = "${riscv-opcodes-src}";

    SOFTFLOAT_RISCV_INCLUDE = "${berkeley-softfloat}/include";

    SOFTFLOAT_RISCV_LIB = "${berkeley-softfloat}/lib/libsoftfloat.a";

    # Do not let model depend on other parts of pokedex in nix build,
    # therefore directly pull the include directory.
    POKEDEX_INCLUDE = "${../simulator/include}";

    POKEDEX_CONFIG = "${pokedex-configs.src}";

  }
  // lib.optionalAttrs (!(pokedex-configs.profile.ext ? f && pokedex-configs.profile.ext ? zve32f)) {
    # Disable instruction opcode check because rv_v have both fp and non-fp instruction
    BUILDGEN_NO_CHECK = "1";
  };

  configurePhase = ''
    runHook preConfigure

    python -m scripts.buildgen

    runHook postConfigure
  '';

  # buildPhase will use ninja

  installPhase = ''
    runHook preInstall

    mkdir -p $out/include $out/lib

    cp -v -t $out/include build/2-cgen/*.h
    cp -v -t $out/lib build/3-clib/*.a
    cp -v -t $out/lib build/3-clib/*.so

    cp -v -r build/docs $out/docs

    runHook postInstall
  '';
}
