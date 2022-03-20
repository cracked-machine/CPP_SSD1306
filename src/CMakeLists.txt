

target_sources(${BUILD_NAME} PRIVATE
    font5x5.cpp
    font5x7.cpp
    font7x10.cpp
    font11x18.cpp
    font16x26.cpp
    ssd1306.cpp
    ssd1306_common.cpp

)

target_include_directories(${BUILD_NAME} PRIVATE 
    ../include
)