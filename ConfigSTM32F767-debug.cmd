set path=%path:"=%
call "%VS140COMNTOOLS%..\..\VC\vcvarsall.bat"
mkdir ..\Metronome-Build-STM32F767-debug
pushd ..\Metronome-Build-STM32F767-debug
cmake ../Metronome -DCCOLA_DIR="../Metronome/EmbeddedInfraLib/ccola" -DCCOLA_INSTALL_DIR="../Metronome-Install" -DCMAKE_TOOLCHAIN_FILE="../Metronome/EmbeddedInfraLib/ccola/toolchain-gcc-arm-cubeide.cmake" -G"Eclipse CDT4 - NMake Makefiles" -DCMAKE_ECLIPSE_VERSION=4.6 -DCMAKE_SYSTEM_PROCESSOR="stm32f767" -DCMAKE_ECLIPSE_MAKE_ARGUMENTS=-s -DCCOLA_EXTRA_CONFIGURATION=debug
popd