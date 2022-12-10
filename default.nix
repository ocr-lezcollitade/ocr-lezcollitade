{ pkgs ? import <nixpkgs> {}}:

with pkgs;

stdenv.mkDerivation {
    name = "ocr-lezcollitade";
    src = ./.;

    nativeBuildInputs = [
        gdb
        gnumake
        gcc
        pkg-config
        doxygen
        wrapGAppsHook
        glade
        clang-tools
    ];

    buildInputs = [
        makeWrapper
        glib
        gtk3
        SDL2
        SDL2_ttf
        SDL2_image
    ];

    preFixup = ''
        gappsWrapperArgs+=(
                --prefix XDG_DATA_DIRS : "${gdk-pixbuf}/share"
                --prefix XDG_DATA_DIRS : "${librsvg}/share"
                --prefix XDG_DATA_DIRS : "${shared-mime-info}/share"
                )
        '';

    shellHook = ''
    export GSETTINGS_SCHEMA_DIR=${pkgs.gtk3}/share/gsettings-schemas/gtk+3-3.24.34/glib-2.0/schemas
        '';

    buildPhase = ''
        mkdir -p $out/bin
        make
        '';
    installPhase = ''
        cp ./bin/Release/Lezcollitade $out/bin/solver
        '';
}


