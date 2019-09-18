################################################################
 #
 # Locate TCL library.
 #
 # Note that the cmake findTcl module is hopeless for OSX
 # because there doesn't appear to be a way to override
 # searching OSX system directories before unix directories.

 set(TCL_POSSIBLE_NAMES tcl87 tcl8.7
   tcl86 tcl8.6
   tcl85 tcl8.5
   tcl84 tcl8.4
   tcl83 tcl8.3
   tcl82 tcl8.2
   )

 # tcl lib path guesses.
 if (NOT TCL_LIB_PATHS)
   if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
     set(TCL_LIB_PATHS /usr/local/lib)
     set(TCL_NO_DEFAULT_PATH TRUE)
   endif()
 elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
   set(TCL_LIB_PATHS /usr/lib
     /usr/local/lib
     )
   set(TCL_NO_DEFAULT_PATH FALSE)
 endif()

 if (NOT TCL_LIB)
   # bagbiter cmake doesn't have a way to pass NO_DEFAULT_PATH as a parameter.
   if (TCL_NO_DEFAULT_PATH)
     find_library(TCL_LIB
       NAMES tcl ${TCL_POSSIBLE_NAMES}
       PATHS ${TCL_LIB_PATHS}
       NO_DEFAULT_PATH
       )
   else()
     find_library(TCL_LIB
       NAMES tcl ${TCL_POSSIBLE_NAMES}
       PATHS ${TCL_LIB_PATHS}
       )
   endif()
 endif()
 message(STATUS "TCL lib: ${TCL_LIB}")

 get_filename_component(TCL_LIB_DIR "${TCL_LIB}" PATH)
 get_filename_component(TCL_LIB_PARENT1 "${TCL_LIB_DIR}" PATH)
 get_filename_component(TCL_LIB_PARENT2 "${TCL_LIB_PARENT1}" PATH)

 # Locate tcl.h
 if (NOT TCL_HEADER)
   find_file(TCL_HEADER tcl.h
     PATHS ${TCL_LIB_PARENT1} ${TCL_LIB_PARENT2}
     PATH_SUFFIXES include include/tcl
     NO_DEFAULT_PATH
     )
 endif()
 message(STATUS "TCL header: ${TCL_HEADER}")
 get_filename_component(TCL_HEADER_DIR "${TCL_HEADER}" PATH)

 ################################################################