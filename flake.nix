{
  description = "ASL RISC-V Golden Model";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-parts.url = "github:hercules-ci/flake-parts";
    treefmt-nix.url = "github:numtide/treefmt-nix";
    pokedex-configs-src = {
      url = "git+ssh://git@github.com/xinpian-tech/pokedex-configs.git?ref=master";
      flake = false;
    };
  };

  outputs =
    inputs@{
      self,
      nixpkgs,
      flake-parts,
      treefmt-nix,
      ...
    }:
    let
      overlay = import ./nix/overlay.nix { inherit (inputs) pokedex-configs-src; };
    in
    flake-parts.lib.mkFlake { inherit inputs; } {
      # Add supported platform here
      systems = [
        "x86_64-linux"
        "aarch64-linux"
        "aarch64-darwin"
      ];

      flake = {
        overlays = {
          default = overlay;
        };
      };

      imports = [
        # Add treefmt flake module to automatically configure and add formatter to this flake
        treefmt-nix.flakeModule
      ];

      perSystem =
        { system, ... }:
        let
          pkgs = import nixpkgs {
            inherit system;
            overlays = [
              overlay
            ];
          };
        in
        {
          _module.args.pkgs = pkgs;
          legacyPackages = pkgs;

          devShells = {
            default = pkgs.mkShell {
              buildInputs = [
                pkgs.just
              ];
            };
          };

          treefmt = {
            projectRootFile = "flake.nix";
            settings.on-unmatched = "debug";
            programs = {
              nixfmt.enable = true;
              black.enable = true;

              # treefmt-nix can not determine edition automatically,
              # unlike 'cargo fmt' which reads from Cargo.toml.
              #
              # rustfmt.enable = true;
            };
          };
        };
    };
}
