{ lib, llvmPackages_11, cmake }:

llvmPackages_11.stdenv.mkDerivation rec {
	pname = "cjp2";
	version = "0.1.0";

	src = ./.;

	nativeBuildInputs = [ cmake ];

	cmakeFlags = [
		"-DENABLE_INSTALL=ON"
	];

	meta = with lib; {
		homepage = "https://github.com/RegularPhoenix/jpeg2000";
		description = "A C++ library for encoding and decoding JPEG2000 images";
		platforms = with platforms; linux ++ darwin;
	};
}
