mkdir ..\Metronome-Build-VisualStudio
pushd ..\Metronome-Build-VisualStudio
cmake ../Metronome -DCCOLA_DIR="../Metronome/EmbeddedInfraLib/ccola" -DCCOLA_INSTALL_DIR="../Metronome-Install" -G"Visual Studio 14 2015" -DCMAKE_CONFIGURATION_TYPES="Debug;RelWithDebInfo"
popd
