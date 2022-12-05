{
    description = "Flake for OCR-Lezcollitade EPITA S3 project.";

    inputs = {
        nixpkgs.url = "github:nixos/nixpkgs/nixos-22.11";
    };

    outputs = { self, nixpkgs }: 
        let
            system = "x86_64-linux";
    in {

        packages.x86_64-linux.default = 
            with import nixpkgs { system = system; };
            stdenv.mkDerivation {
                name = "OCR-Lezcollitade";
                src = self;
                version = "1.0.0-ALPHA";

                nativeInputs = with pkgs; [
                    git-lfs
                ];
                buildInputs = with pkgs; [
                    pkg-config
                        gcc
                        gnumake
                        gtk3
                        SDL2
                        SDL2_image
                ];

                buildPhase = "make";
                installPhase = ''
                    # cp $src/bin/Release/Lezcollitade $out/
                '';
        };
    };

}
