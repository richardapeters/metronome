set path=%path:"=%
call "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars32.bat"
mkdir ..\Metronome-Build-STM32F746-debug
pushd ..\Metronome-Build-STM32F746-debug
cmake ../Metronome -DCCOLA_DIR="../Metronome/EmbeddedInfraLib/ccola" -DCCOLA_INSTALL_DIR="../Metronome-Install" -DCMAKE_TOOLCHAIN_FILE="../Metronome/EmbeddedInfraLib/ccola/toolchain-gcc-arm-cubeide.cmake" -G"Eclipse CDT4 - NMake Makefiles" -DCMAKE_ECLIPSE_VERSION=4.6 -DCMAKE_SYSTEM_PROCESSOR="stm32f746" -DCMAKE_ECLIPSE_MAKE_ARGUMENTS=-s -DCCOLA_EXTRA_CONFIGURATION=debug
popd
