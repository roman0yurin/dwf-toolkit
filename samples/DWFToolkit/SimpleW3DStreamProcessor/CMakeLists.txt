CMAKE_MINIMUM_REQUIRED( VERSION 3.5 )

PROJECT( DWF_SimpleW3DStreamProcessor )
set(CMAKE_CXX_STANDARD 14)


include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_executable(${PROJECT_NAME}
            SimpleW3DStreamProcessor.cpp
            stdafx.cpp
        )

target_link_libraries(${PROJECT_NAME} dwf_rep)