{
	description = "A C++ library for encoding and decoding JPEG2000 images";

	inputs = {
		nixpkgs.url = github:NixOS/nixpkgs/nixos-unstable;
		utils.url = "github:numtide/flake-utils";
	};

	outputs = { self, nixpkgs, utils }:
		utils.lib.eachDefaultSystem (system:
			let
				pkgs = nixpkgs.legacyPackages.${system};
			in {
				packages = rec {
					default = pkgs.stdenv.mkDerivation rec {
						name = "cjp2";
						src = self;

						nativeBuildInputs = with pkgs; [
							meson
							ninja
							pkg-config
						];

						buildInputs = with pkgs; [
							spdlog
							pugixml
							fmt
						];

						installPhase = "mkdir $out; cp libjpeg2000.so $out";

						enableParallelBuildding = true;

						meta = with pkgs.lib; {
							homepage = "https://github.com/RegularPhoenix/jpeg2000";
							description = "A C++ library for encoding and decoding JPEG2000 images";
							platforms = with platforms; linux ++ darwin;
						};
					};
				};

				devShells = rec {
					default = pkgs.mkShell rec {
						name = "C++";

						packages = with pkgs; [
							meson
							ninja
							pkg-config
							spdlog
							pugixml
							fmt
						];
					};
				};

				apps = rec {
					default = utils.lib.mkApp {
						drv = self.packages.${system}.default;
						name = "cjp2";
					};
				};
			}
		);
}
