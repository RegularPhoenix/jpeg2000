{
	description = "A C++ library for encoding and decoding JPEG2000 images";

	inputs = {
		nixpkgs.url = github:NixOS/nixpkgs/nixos-unstable;
		utils.url = "github:numtide/flake-utils";
		utils.inputs.nixpkgs.follows = "nixpkgs";
	};

	outputs = { self, nixpkgs, ... }@inputs: inputs.utils.lib.eachSystem [
		"x86_64-linux" "i686-linux" "aarch64-linux" "x86_64-darwin"
	] (system: let pkgs = import nixpkgs {
				inherit system;
			};
		in {
			devShell = pkgs.mkShell rec {
				name = "C++";

				packages = with pkgs; [
					llvmPackages_11.clang
					cmake
				];
			};

			defaultPackage = pkgs.callPackage ./default.nix {};
		});
}
