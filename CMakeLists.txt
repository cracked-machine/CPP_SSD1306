

target_sources(${BUILD_NAME} PRIVATE
    src/font5x5.cpp
    src/font5x7.cpp
    src/font7x10.cpp
    src/font11x18.cpp
    src/font16x26.cpp
    src/ssd1306.cpp
)

target_include_directories(${BUILD_NAME} PRIVATE 
    inc
)