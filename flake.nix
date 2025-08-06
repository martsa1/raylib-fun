# Inspired by https://github.com/Miou-zora/Raylib-Zig-Nix
{
  inputs = {

    nixpkgs.url = "github:nixos/nixpkgs/nixpkgs-unstable";

    flake-utils = {
      url = "github:numtide/flake-utils";
    };

    zig-overlay = {
      url = "github:mitchellh/zig-overlay";
      inputs.nixpkgs.follows = "nixpkgs";
    };
  };

  outputs = { self, nixpkgs, flake-utils, zig-overlay }:
    flake-utils.lib.eachSystem [ "x86_64-linux" ] (system:
      let
        # Refer to https://github.com/mitchellh/zig-overlay if you want to use a specific version of Zig
        zigPackage = zig-overlay.packages.${system}."0.14.1";
        pkgs = nixpkgs.legacyPackages.${system};
      in
      {
        formatter = pkgs.nixpkgs-fmt;
        devShells.default = pkgs.mkShell {
          name = "name_of_your_project";
          packages = with pkgs; [
            raylib
            patchelf
          ];
          nativeBuildInputs = [
            zigPackage
          ];
        };
        packages.default = pkgs.stdenv.mkDerivation {
          name = "name_of_your_project";
          src = ./.;

          XDG_CACHE_HOME = "${placeholder "out"}";

          buildInputs = [ pkgs.raylib ];
          buildPhase = ''
            ${zigPackage}/bin/zig build
          '';

          installPhase = ''
            ${zigPackage}/bin/zig build install --prefix $out
            rm -rf $out/zig # remove cache
          '';
        };
      });
}
