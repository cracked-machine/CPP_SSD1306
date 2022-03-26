target_sources(${BUILD_NAME} PRIVATE
    catch_cpp_ssd1306.cpp
    font_test.cpp
    ssd1306_tester.cpp
    ${CMAKE_BINARY_DIR}/embedded_utils/src/restricted_base.cpp
)

target_include_directories(${BUILD_NAME} PRIVATE 
    .
    ${CMAKE_BINARY_DIR}/stm32_interrupt_managers/include
    ${CMAKE_BINARY_DIR}/embedded_utils/include
)