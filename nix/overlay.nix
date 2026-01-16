final: prev: rec {
  pokedex = final.callPackage ../package.nix { };

  asl-interpreter = final.callPackage ./pkgs/asl-interpreter.nix { };

  aslref = final.callPackage ./pkgs/aslref.nix { };

  riscv-vector-tests = final.callPackage ./pkgs/riscv-vector-tests.nix { };

  rv32-llvm-compiler-rt = final.callPackage ./pkgs/rv32-llvm-compiler-rt.nix { };

  rv32-stdenv =
    let
      rv32Pkgs = final.pkgsCross.riscv32-embedded;
      rv32BuildPkgs = rv32Pkgs.buildPackages;
      rv32llvmPkgs = rv32BuildPkgs.llvmPackages;
    in
    rv32Pkgs.stdenv.override {
      cc =
        let
          major = final.lib.versions.major rv32llvmPkgs.release_version;

          # By default, compiler-rt and newlib for rv32 are built with double float point abi by default.
          # We need to override it with `-mabi=ilp32f`

          # compiler-rt requires the compilation flag -fforce-enable-int128, only clang provides that

          newlib = rv32Pkgs.stdenv.cc.libc.overrideAttrs (oldAttrs: {
            CFLAGS_FOR_TARGET = "-march=rv32imacf_zvl128b_zve32f -mabi=ilp32f";
          });
        in
        rv32BuildPkgs.wrapCCWith rec {
          cc = rv32llvmPkgs.clang-unwrapped;
          libc = newlib;
          bintools = rv32Pkgs.stdenv.cc.bintools.override {
            inherit libc; # we must keep consistency of bintools libc and compiler libc
            inherit (rv32llvmPkgs.bintools) bintools;
          };

          # common steps to produce clang resource directory
          extraBuildCommands = ''
            rsrc="$out/resource-root"
            mkdir "$rsrc"
            ln -s "${cc.lib}/lib/clang/${major}/include" "$rsrc"
            echo "-resource-dir=$rsrc" >> $out/nix-support/cc-cflags
            ln -s "${rv32-llvm-compiler-rt}/lib" "$rsrc/lib"
            ln -s "${rv32-llvm-compiler-rt}/share" "$rsrc/share"
          '';

          # link against emurt
          extraPackages = [ final.emurt ];
          nixSupport.cc-cflags = [ "-lemurt" ];
        };
    };

  riscv-opcodes-src = final.fetchFromGitHub {
    owner = "riscv";
    repo = "riscv-opcodes";
    rev = "9fd70d9b4f54a364043d9901f5b54e32b28fc89b"; # 2026-01-24
    hash = "sha256-qHezPhmisDHu/IxO1LSg2vmX09dqRQO1J4LdVOU/1UM=";
  };
}
