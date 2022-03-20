
## SSD1306 Library

SSD1306 datasheet can be found [here](https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf)

### Design

![SSD1306 Class Diagram](docs/SSD1306_Class_Diagram.png)

### Usage

This uses DMA ISR with the stm32g0 specialization of the [isr_manager](https://github.com/cracked-machine/stm32_interrupt_managers).

```
#include <isr_manager_stm32g0.hpp>
#include <ssd1306.hpp>

// SPI peripheral for SSD1306 display driver serial communication
ssd1306::DriverSerialInterface<stm32::isr::InterruptTypeStm32g0> ssd1306_spi_interface(
    SPI1, 
    GPIOA, 
    LL_GPIO_PIN_0, 
    GPIOA, 
    LL_GPIO_PIN_3,
    stm32::isr::InterruptTypeStm32g0::dma1_ch2);

// @brief SSD1306 OLED driver
ssd1306::Driver<stm32::isr::InterruptTypeStm32g0> m_oled(
    ssd1306_spi_interface, 
    ssd1306::Driver<stm32::isr::InterruptTypeStm32g0>::SPIDMA::enabled
);

// init SSD1306 IC display driver
m_oled.power_on_sequence()

std::string text = "Say something clever";
m_oled.write(text, m_font, 0, 0, ssd1306::Colour::Black, ssd1306::Colour::White, 3, true);


{
 

```

### Running Units Tests on X86

Running `cmake` in the root directory will invoke the tests.
It is recommended to use VSCode and "CMake Tools" extension with Test Suite, but if you insist on using the CLI:

- Configure the testsuite:
`cmake --no-warn-unused-cli -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_C_COMPILER:FILEPATH=/bin/x86_64-linux-gnu-gcc-10 -Bbuild -G Ninja .`
- Build the testsuite
`cmake --build build --config Debug --target all --`
- Run the testsuite
`./build/test_suite`
- Clean the testsuite
`cmake --build build --config Debug --target clean --`

### Adding this library to your STM32 Project

Include this repo into your project as a submodule and add the following line to your top-level CMakeFiles.txt:

`add_subdirectory(embedded_utils)`

This assumes your project's top-level CMakeFiles.txt is already configured for STM32 platform.

### Debug

`GDB` is enabled in VSCode (F5)

### Code Coverage report

Run the "coverage report" task. Results can be found in `coverage` directory.


