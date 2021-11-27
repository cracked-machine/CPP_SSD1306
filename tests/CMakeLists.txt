target_sources(${BUILD_NAME} PRIVATE
    catch_cpp_ssd1306.cpp
    ssd1306_tester.cpp
)

target_include_directories(${BUILD_NAME} PRIVATE 
    .
)