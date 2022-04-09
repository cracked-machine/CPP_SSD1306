
[![CMake](https://github.com/cracked-machine/cpp_ssd1306/actions/workflows/cmake.yml/badge.svg)](https://github.com/cracked-machine/cpp_ssd1306/actions/workflows/cmake.yml)
[![Codecov](https://img.shields.io/codecov/c/github/cracked-machine/cpp_ssd1306)](https://app.codecov.io/gh/cracked-machine/cpp_ssd1306)
## SSD1306 Library

See the [wiki](https://github.com/cracked-machine/cpp_ssd1306/wiki) for documentation / reference

See the [readme](tests) for test info.

#### Adding this library to your STM32 Project

Include this repo into your project as a submodule and add the following line to your top-level CMakeFiles.txt:

`add_subdirectory(cpp_ssd1306)`

This assumes your top-level CMakeFiles.txt is already configured for STM32 platform.