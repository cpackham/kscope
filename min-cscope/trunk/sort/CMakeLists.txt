INCLUDE (CheckFunctionExists) 

# sort library sources
SET(SORT_SRCS file.c page.c cache.c extsort.c)
IF(WIN32)
    SET(SORT_SRCS ${SORT_SRCS} os_win.c)
ELSE(WIN32)
    SET(SORT_SRCS ${SORT_SRCS} os_posix.c)
ENDIF(WIN32)

# Check for some functions
CHECK_FUNCTION_EXISTS(qsort_r HAVE_QSORT_R) 
CHECK_FUNCTION_EXISTS(qsort_s HAVE_QSORT_S) 

# Create the configuration header
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

# Modify to a value of your choice. However, 2^PAGE_SIZE_ORDER must be a 
# multiple of the native page size.
ADD_DEFINITIONS(-DPAGE_SIZE_ORDER=22)

# Comment to have statistics on the build process
IF(CMAKE_BUILD_TYPE STREQUAL "Release")
    ADD_DEFINITIONS(-DNSTATS)
ENDIF(CMAKE_BUILD_TYPE STREQUAL "Release")

ADD_LIBRARY(sort STATIC ${SORT_SRCS})
