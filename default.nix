let pkgs = import <nixpkgs> {};
in pkgs.clangStdenv.mkDerivation rec {
  pname = "spacegame";
  version = "1.0";

  src = ./.;

  nativeBuildInputs = [ pkgs.cmake ];
  cmakeFlags = [
    "-Dnlohmann_json_DIR=${pkgs.nlohmann_json}/lib/cmake/nlohmann_json"
  ];
  buildInputs = [ pkgs.SDL2 pkgs.SDL2_image pkgs.SDL2_mixer pkgs.SDL2_ttf pkgs.clang-tools pkgs.cppcheck pkgs.inotify-tools pkgs.ninja pkgs.ccls pkgs.jetbrains.clion pkgs.gdb pkgs.nlohmann_json ];
}
