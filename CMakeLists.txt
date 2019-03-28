CMAKE_MINIMUM_REQUIRED( VERSION 3.5 )

PROJECT( dwf_rep )
set(CMAKE_CXX_STANDARD 14)
#Настройка Warning-ов
add_definitions(-Wno-deprecated -Wno-write-strings -Wno-narrowing -Wno-trigraphs)
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast") #Только для C
#Предопределения -DDWFCORE_BUILD_ZLIB  -DDWFCORE_BUILD_EXPAT
add_definitions(-DDWFCORE_STATIC -DDWFTK_STATIC -DWHIP_USE_WCHAR_STRINGS -DSTDC -D_DWFCORE_X64_SYSTEM -DSQLITE_OMIT_LOAD_EXTENSION)

include_directories(develop/global/src)
include_directories(develop/global/src/dwf)
include_directories(djinni)

file(GLOB_RECURSE DWF_SOURES develop/global/src/*.cpp develop/global/src/*.c djinni/*.cpp)

add_library(${PROJECT_NAME} STATIC ${DWF_SOURES})

find_package( ZLIB REQUIRED )
include_directories( ${ZLIB_INCLUDE_DIRS} )
target_link_libraries( ${PROJECT_NAME} ${ZLIB_LIBRARIES} )


#find_package( EXPAT REQUIRED )
#include_directories( ${EXPAT_INCLUDE_DIRS} )
#target_link_libraries( ${PROJECT_NAME} ${EXPAT_LIBRARIES} )

find_package( JPEG REQUIRED )
include_directories( ${JPEG_INCLUDE_DIRS} )
target_link_libraries( ${PROJECT_NAME} ${JPEG_LIBRARIES} )


find_package ( Threads REQUIRED )
target_link_libraries ( ${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT} expat)

add_subdirectory(samples/DWFToolkit/SimpleW3DStreamProcessor)
add_subdirectory(samples/TestDWF)