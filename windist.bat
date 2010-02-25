
@rem Prepare Windows executable of the latest revision
@rem (requires Bazaar, MinGW, and Qt set up and in the %PATH%)

bzr pull
qmake cluster.pro
mingw32-make release