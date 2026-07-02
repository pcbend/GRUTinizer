grutinizer_add_library(TGRUTSupport ${CMAKE_CURRENT_SOURCE_DIR}/TGRUTSupport)
grutinizer_add_library(TGRUTUtil ${CMAKE_CURRENT_SOURCE_DIR}/TGRUTUtil)
grutinizer_add_library(TRawFormat ${CMAKE_CURRENT_SOURCE_DIR}/TRawFormat)
grutinizer_add_library(TNSCLFormat ${CMAKE_CURRENT_SOURCE_DIR}/TNSCLFormat)
grutinizer_add_library(TGEBFormat ${CMAKE_CURRENT_SOURCE_DIR}/TGEBFormat)

grutinizer_add_library(TDetector ${CMAKE_CURRENT_SOURCE_DIR}/TDetSystems/TDetector)
grutinizer_add_library(TNSCLScalers ${CMAKE_CURRENT_SOURCE_DIR}/TDetSystems/TNSCLScalers)
grutinizer_add_library(TSega ${CMAKE_CURRENT_SOURCE_DIR}/TDetSystems/TSega)
grutinizer_add_library(TGretina ${CMAKE_CURRENT_SOURCE_DIR}/TDetSystems/TGretina)
grutinizer_add_library(TJanus ${CMAKE_CURRENT_SOURCE_DIR}/TDetSystems/TJanus)
grutinizer_add_library(TPhosWall ${CMAKE_CURRENT_SOURCE_DIR}/TDetSystems/TPhosWall)
grutinizer_add_library(TS800 ${CMAKE_CURRENT_SOURCE_DIR}/TDetSystems/TS800)
grutinizer_add_library(TFastScint ${CMAKE_CURRENT_SOURCE_DIR}/TDetSystems/TFastScint)
grutinizer_add_library(TCaesar ${CMAKE_CURRENT_SOURCE_DIR}/TDetSystems/TCaesar)
grutinizer_add_library(TS800Sim ${CMAKE_CURRENT_SOURCE_DIR}/TDetSystems/TS800Sim)
grutinizer_add_library(TGretSim ${CMAKE_CURRENT_SOURCE_DIR}/TDetSystems/TGretSim)
grutinizer_add_library(TLenda ${CMAKE_CURRENT_SOURCE_DIR}/TDetSystems/TLenda)
grutinizer_add_library(TFSU ${CMAKE_CURRENT_SOURCE_DIR}/TDetSystems/TFSU)
grutinizer_add_library(TUML ${CMAKE_CURRENT_SOURCE_DIR}/TDetSystems/TUML)
grutinizer_add_library(TOBJ ${CMAKE_CURRENT_SOURCE_DIR}/TDetSystems/TOBJ)

grutinizer_add_library(TLoops ${CMAKE_CURRENT_SOURCE_DIR}/TLoops)
grutinizer_add_library(THistogramming ${CMAKE_CURRENT_SOURCE_DIR}/THistogramming)
grutinizer_add_library(TGRUTint ${CMAKE_CURRENT_SOURCE_DIR}/TGRUTint)

set(GRUTINIZER_LIBRARY_TARGETS
  TGRUTSupport
  TGRUTUtil
  TRawFormat
  TNSCLFormat
  TGEBFormat
  TDetector
  TNSCLScalers
  TSega
  TGretina
  TJanus
  TPhosWall
  TS800
  TFastScint
  TCaesar
  TS800Sim
  TGretSim
  TLenda
  TFSU
  TUML
  TOBJ
  TLoops
  THistogramming
  TGRUTint
  PARENT_SCOPE
)
