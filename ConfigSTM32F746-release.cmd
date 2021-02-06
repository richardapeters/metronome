set path=%path:"=%
call "%VS140COMNTOOLS%..\..\VC\vcvarsall.bat"
mkdir ..\Metronome-Build-STM32F746-release
pushd ..\Metronome-Build-STM32F746-release
cmake ../Metronome -DCCOLA_DIR="../Metronome/EmbeddedInfraLib/ccola" -DCCOLA_INSTALL_DIR="../Metronome-Install" -DCMAKE_TOOLCHAIN_FILE="../Metronome/EmbeddedInfraLib/ccola/toolchain-gcc-arm-cubeide.cmake" -G"Eclipse CDT4 - NMake Makefiles" -DCMAKE_ECLIPSE_VERSION=4.6 -DCMAKE_SYSTEM_PROCESSOR="stm32f746" -DCMAKE_ECLIPSE_MAKE_ARGUMENTS=-s -DCCOLA_OPTIMIZE=size -DCCOLA_EXTRA_CONFIGURATION=release
popd
