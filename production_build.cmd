
REM first build with

mkdir build-x64-windows-mega
cd build-x64-windows-mega
cmake -G "Visual Studio 16 2019" -A x64 -DMega3rdPartyDir=C:\Users\build\MEGA\build-MEGASync\3rdParty_MSVC2019_20210916\3rdParty_desktop -DVCPKG_TRIPLET=x64-windows-mega -S "..\contrib\cmake" -B .
cmake --build . --config Release
cd ..

mkdir build-x86-windows-mega
cd build-x86-windows-mega
cmake -G "Visual Studio 16 2019" -A Win32 -DMega3rdPartyDir=C:\Users\build\MEGA\build-MEGASync\3rdParty_MSVC2019_20210916\3rdParty_desktop -DVCPKG_TRIPLET=x86-windows-mega -S "..\contrib\cmake" -B .
cmake --build . --config Release
