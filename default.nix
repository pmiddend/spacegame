let pkgs = import <nixpkgs> {};
in pkgs.clangStdenv.mkDerivation rec {
  pname = "spacegame";
  version = "1.0";

  src = ./.;

  nativeBuildInputs = [ pkgs.cmake ];
  buildInputs = [ pkgs.SDL2 pkgs.SDL2_image pkgs.SDL2_ttf pkgs.clang-tools pkgs.cppcheck pkgs.inotify-tools pkgs.ninja pkgs.cquery ];
}
