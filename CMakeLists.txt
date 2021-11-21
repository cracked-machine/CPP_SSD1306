target_sources(${BUILD_NAME} PRIVATE
    src/font.cpp
    src/ssd1306.cpp
)

target_include_directories(${BUILD_NAME} PRIVATE inc)