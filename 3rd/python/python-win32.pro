
MAKE_DIR                = $$PACKAGE_DIR\PCbuild
PACKAGE_MAKE            = vcbuild

PACKAGE_MAKEFILE        = pcbuild.sln
PACKAGE_CLEANOPTS       = /clean

PACKAGE_MAKEFILEOPT     =

debug:PACKAGE_TARGET       = "Debug|Win32"
release:PACKAGE_TARGET     = "Release|Win32"

# for msvc 2003

vc2003:MAKE_DIR                = $$PACKAGE_DIR\PC\VS7.1

vc2003:PACKAGE_MAKE            = cmd.exe /c devenv

debug:PACKAGE_MAKEOPTS_2K3   = Debug
release:PACKAGE_MAKEOPTS_2K3 = Release

vc2003:PACKAGE_MAKEOPTS = $$PACKAGE_MAKEOPTS_2K3

vc2003:PACKAGE_TARGET = /build
