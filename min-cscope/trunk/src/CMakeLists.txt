INCLUDE (CheckIncludeFiles)
INCLUDE (CheckLibraryExists)
INCLUDE (CheckFunctionExists) 
INCLUDE (CheckSymbolExists)

# Source files
SET(MIN_CSCOPE_SRCS alloc.c basename.c build.c compath.c crossref.c dir.c
                    display.c exec.c find.c history.c input.c invlib.c
                    logdir.c lookup.c main.c mygetenv.c mypopen.c vpaccess.c
                    vpfopen.c vpinit.c vpopen.c 
                    ${CMAKE_CURRENT_BINARY_DIR}/fscanner.c
                    ${CMAKE_CURRENT_BINARY_DIR}/egrep.c
)

# Curses interface
IF(NOT NO_CURSES)
    CHECK_INCLUDE_FILES(ncurses.h HAVE_NCURSES)
    IF(HAVE_NCURSES)
        CHECK_LIBRARY_EXISTS(ncurses initscr "${WITH_NCURSES_LIB_PATH}" HAVE_NCURSES_LIB)
    ELSE(HAVE_NCURSES)
        CHECK_INCLUDE_FILES(curses.h HAVE_CURSES)
        IF(HAVE_CURSES)
            CHECK_LIBRARY_EXISTS(curses initscr "${WITH_CURSES_LIB_PATH}" HAVE_CURSES_LIB)
            CHECK_LIBRARY_EXISTS(pdcurses initscr "${WITH_CURSES_LIB_PATH}" HAVE_PDCURSES_LIB)
        ENDIF(HAVE_CURSES)
    ENDIF(HAVE_NCURSES)
    IF(HAVE_NCURSES OR HAVE_CURSES)
        MESSAGE("Building with curses-based interface")
        SET(MIN_CSCOPE_SRCS ${MIN_CSCOPE_SRCS} command.c edit.c help.c mouse.c)
    ENDIF(HAVE_NCURSES OR HAVE_CURSES)
ENDIF(NOT NO_CURSES)

# Check for include files
CHECK_INCLUDE_FILES(fcntl.h HAVE_FCNTL_H)
CHECK_INCLUDE_FILES(wait.h HAVE_WAIT_H)

# Check for regex.h
# This file may be in a non-standard directory, so we allow for the user to
# specify a path for it, in one of two ways:
# 1. Setting WITH_REGEX_PATH to the root directory of the regex installation,
#    expecting an include and a lib subdirectories
# 2. Setting WITH_REGEX_INC_PATH and WITH_REGEX_LIB_PATH separately
IF(WITH_REGEX_PATH)
    SET(WITH_REGEX_INC_PATH ${WITH_REGEX_PATH}/include)
    SET(WITH_REGEX_LIB_PATH ${WITH_REGEX_PATH}/lib)
ENDIF(WITH_REGEX_PATH)

IF(WITH_REGEX_INC_PATH)
    SET(CMAKE_REQUIRED_INCLUDES ${WITH_REGEX_INC_PATH})
ENDIF(WITH_REGEX_INC_PATH)
CHECK_INCLUDE_FILES(regex.h HAVE_REGEX_H)
IF(WIN32)
    CHECK_LIBRARY_EXISTS(regex regexec "${WITH_REGEX_LIB_PATH}" HAVE_REGEX_LIB)
ELSE(WIN32)
    CHECK_FUNCTION_EXISTS(regexec HAVE_REGEX_LIB)
ENDIF(WIN32)
IF(NOT HAVE_REGEX_H OR NOT HAVE_REGEX_LIB)
    MESSAGE(SEND_ERROR "Failed to find regex.h/libregex")
    MESSAGE("Please install the GNU regular expression library")
    IF(WIN32)
        MESSAGE("available on http://gnuwin32.sourceforge.net/packages/regex.htm")
    ENDIF(WIN32)
    MESSAGE("and make sure the include and library paths are set up properly.")
ENDIF(NOT HAVE_REGEX_H OR NOT HAVE_REGEX_LIB)

# Check for some functions
CHECK_FUNCTION_EXISTS(lstat HAVE_LSTAT) 
CHECK_FUNCTION_EXISTS(strerror HAVE_STRERROR) 
CHECK_FUNCTION_EXISTS(__sigsetjmp HAVE_SIGSETJMP) 

# Check for signals
CHECK_SYMBOL_EXISTS(SIGQUIT signal.h HAVE_SIGQUIT) 
CHECK_SYMBOL_EXISTS(SIGHUP signal.h HAVE_SIGHUP) 
CHECK_SYMBOL_EXISTS(SIGHUP signal.h HAVE_SIGPIPE) 
CHECK_SYMBOL_EXISTS(SIGHUP signal.h HAVE_SIGSTP) 
CHECK_SYMBOL_EXISTS(SIGWINCH signal.h HAVE_SIGWINCH) 

# Check for F_DUPFD
IF(HAVE_FCNTL_H)
    CHECK_SYMBOL_EXISTS(F_DUPFD fcntl.h HAVE_F_DUPFD) 
ENDIF(HAVE_FCNTL_H)

# Create the configuration header
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

# Find flex
FIND_PROGRAM(FLEX_EXECUTABLE
             NAMES flex
             DOC "Flex executable")
IF(${FLEX_EXECUTABLE} STREQUAL "FLEX_EXECUTABLE-NOTFOUND")
    MESSAGE(SEND_ERROR "Could not find a lexer generator (flex)")
ENDIF(${FLEX_EXECUTABLE} STREQUAL "FLEX_EXECUTABLE-NOTFOUND")

# Find bison
FIND_PROGRAM(YACC_EXECUTABLE
             NAMES bison
             DOC "Yacc or Bison executable")
IF(${YACC_EXECUTABLE} STREQUAL "YACC_EXECUTABLE-NOTFOUND")
    MESSAGE(SEND_ERROR "Could not find a parser generator (bison)")
ENDIF(${YACC_EXECUTABLE} STREQUAL "YACC_EXECUTABLE-NOTFOUND")

# Generate the parser
ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/fscanner.c
    COMMAND ${FLEX_EXECUTABLE} -o${CMAKE_CURRENT_BINARY_DIR}/fscanner.c ${CMAKE_CURRENT_SOURCE_DIR}/fscanner.l
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/fscanner.l
)

ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/egrep.c
    COMMAND ${YACC_EXECUTABLE} -y -o ${CMAKE_CURRENT_BINARY_DIR}/egrep.c ${CMAKE_CURRENT_SOURCE_DIR}/egrep.y
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/egrep.y
)

# Include directories.
# The source directory is required since the scanner and parser source files are
# created in the build directory
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
IF(USE_SORTLIB)
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../sort)
    ADD_DEFINITIONS(-DUSE_SORTLIB)
ENDIF(USE_SORTLIB)
IF(WITH_REGEX_INC_PATH)
    INCLUDE_DIRECTORIES(${WITH_REGEX_INC_PATH})
ENDIF(WITH_REGEX_INC_PATH)

IF(WIN32)
    ADD_DEFINITIONS(-DWIN32)
ENDIF(WIN32)

# Comment to have statistics on the build process
Message("CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
IF(CMAKE_BUILD_TYPE STREQUAL "Release")
    ADD_DEFINITIONS(-DNSTATS)
ENDIF(CMAKE_BUILD_TYPE STREQUAL "Release")

# Library directories
IF(WITH_REGEX_LIB_PATH)
    LINK_DIRECTORIES(${WITH_REGEX_LIB_PATH})
ENDIF(WITH_REGEX_LIB_PATH)

# Required libraries
SET(MIN_CSCOPE_LIBS)
IF(DEFINED USE_SORTLIB)
    SET(MIN_CSCOPE_LIBS sort)
ENDIF(DEFINED USE_SORTLIB)
IF(WIN32)
    SET(MIN_CSCOPE_LIBS ${MIN_CSCOPE_LIBS} regex)
ENDIF(WIN32)
IF(HAVE_NCURSES_LIB)
    SET(MIN_CSCOPE_LIBS ${MIN_CSCOPE_LIBS} ncurses)
ELSEIF(HAVE_CURSES_LIB)
    SET(MIN_CSCOPE_LIBS ${MIN_CSCOPE_LIBS} curses)
ELSEIF(HAVE_PDCURSES_LIB)
    SET(MIN_CSCOPE_LIBS ${MIN_CSCOPE_LIBS} pdcurses)
ENDIF(HAVE_NCURSES_LIB)

ADD_EXECUTABLE(min-cscope ${MIN_CSCOPE_SRCS})
TARGET_LINK_LIBRARIES(min-cscope ${MIN_CSCOPE_LIBS})
