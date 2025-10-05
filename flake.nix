{
  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs = { self, nixpkgs }:
    let
      supportedSystems = [ "x86_64-linux" "x86_64-darwin" "aarch64-linux" "aarch64-darwin" ];
      forAllSystems = nixpkgs.lib.genAttrs supportedSystems;
    in
    {
      devShell = forAllSystems (system: with import nixpkgs { inherit system;};
        mkShell {
          buildInputs = [
            python311Packages.numpy
            (python311.withPackages (ps: with ps; [matplotlib seaborn pandas]))
            gcc
          ];
        }
      );
    };
}
