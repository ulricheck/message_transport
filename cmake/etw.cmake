# Event Tracing for Windows setup
#


# Check if OS supports ETW
MACRO(CHECK_ETW)
#  CMAKE_MC_COMPILER - where to find mc.exe
if (WIN32)
  # cmake has CMAKE_RC_COMPILER, but no message compiler
  if ("${CMAKE_GENERATOR}" MATCHES "Visual Studio")
    # this path is only present for 2008+, but we currently require PATH to
    # be set up anyway
    get_filename_component(sdk_dir "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows;CurrentInstallFolder]" REALPATH)
    get_filename_component(kit_dir "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows Kits\\Installed Roots;KitsRoot]" REALPATH)
    if (X64)
      set(sdk_bindir "${sdk_dir}/bin/x64")
      set(kit_bindir "${kit_dir}/bin/x64")
    else (X64)
      set(sdk_bindir "${sdk_dir}/bin")
      set(kit_bindir "${kit_dir}/bin/x86")
    endif (X64)
  endif ()
  find_program(CMAKE_MC_COMPILER mc.exe HINTS "${sdk_bindir}" "${kit_bindir}"
    DOC "path to message compiler")

  # This should probably check for the Windows SDK as well here ..
    
  if (NOT CMAKE_MC_COMPILER)
     SET(ENABLE_ETW OFF CACHE BOOL "Event Tracing for Windows disabled.")
   ELSE()
     SET(ENABLE_ETW ON CACHE BOOL "Enable Event Tracing for Windows")
  endif (NOT CMAKE_MC_COMPILER)
  message(STATUS "Found message compiler: ${CMAKE_MC_COMPILER}")
  mark_as_advanced(CMAKE_MC_COMPILER)
endif(WIN32)

 SET(HAVE_ETW ${ENABLE_ETW})
ENDMACRO()

CHECK_ETW()

# Produce a header file  with
# ETW macros
MACRO (ETW_HEADER provider header_name)
 IF(ENABLE_ETW)
 ADD_CUSTOM_COMMAND(
   OUTPUT  ${header_name}.h ${header_name}.rc ${header_name}Temp.bin
   COMMAND ${CMAKE_MC_COMPILER} -um ${provider} -z ${header_name}
   WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/utDataflow
   COMMENT "Exectuing: mc.exe -um ${provider} -z ${header_name}"
   DEPENDS ${provider}
 )
 ENDIF()
ENDMACRO()


# Create provider headers
IF(ENABLE_ETW)
  CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/doc/etw/ubitrack_etw_providers.man.base
    ${CMAKE_BINARY_DIR}/utDataflow/ubitrack_etw_providers.man COPYONLY)
  ETW_HEADER(
   ${CMAKE_BINARY_DIR}/utDataflow/ubitrack_etw_providers.man
   probes_ubitrack_etw
  )
  MESSAGE(WARN "Please execute these commands manually for now:")
  MESSAGE(STATUS "ETW Generate Headers command: cd  ${CMAKE_BINARY_DIR}/utDataflow")
  MESSAGE(STATUS "ETW Generate Headers command: mc.exe -um ${CMAKE_BINARY_DIR}/utDataflow/ubitrack_etw_providers.man -z probes_ubitrack_etw")
  ADD_CUSTOM_TARGET(gen_etw_header
  DEPENDS  
  ${CMAKE_BINARY_DIR}/utDataflow/ubitrack_etw_providers.man
  ${CMAKE_BINARY_DIR}/utDataflow/probes_ubitrack_etw.h
  ${CMAKE_BINARY_DIR}/utDataflow/probes_ubitrack_etw.rc
  ${CMAKE_BINARY_DIR}/utDataflow/probes_ubitrack_etwTemp.bin
  ) 
ENDIF()

FUNCTION(ETW_INSTRUMENT target)
  IF(ENABLE_ETW)
    ADD_DEPENDENCIES(${target} gen_etw_header)
  ENDIF()
ENDFUNCTION()