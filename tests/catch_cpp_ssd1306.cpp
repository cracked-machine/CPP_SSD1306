// MIT License

// Copyright (c) 2022 Chris Sutton

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

#include <catch2/catch_all.hpp>
#include <font.hpp>
#include <iostream>
#include <mock.hpp>
#include <ssd1306.hpp>
#include <ssd1306_tester.hpp>

TEST_CASE ("Test Fonts", "[ssd1306_fonts]")
{

  // SPI peripheral for SSD1306 display driver serial communication
  ssd1306::DriverSerialInterface<STM32G0_ISR> ssd1306_spi_interface (
      SPI1, std::make_pair (GPIOA, GPIO_BSRR_BS0), // PA0 - DC
      std::make_pair (GPIOA, GPIO_BSRR_BS3),       // PA3 - Reset
      STM32G0_ISR::dma1_ch2);

  ssd1306::Driver<STM32G0_ISR> d{
    ssd1306_spi_interface, ssd1306::Driver<STM32G0_ISR>::SPIDMA::enabled
  };
  ssd1306::Font5x7 f5x7;
  std::cout << sizeof (f5x7);
  noarch::containers::StaticString<20> m_display_line1;
  REQUIRE (true);
}

// TEST_CASE("Test Fonts", "[ssd1306_fonts]")
// {
//     SECTION("3x5Font")
//     {
//         static ssd1306::Font5x5 font_under_test;
//         REQUIRE(font_under_test.width() == 5);
//         REQUIRE(font_under_test.height() == 5);
//         REQUIRE(font_under_test.size() == 475);
//     }

//     SECTION("5x7Font")
//     {
//         static ssd1306::Font5x7 font_under_test;
//         REQUIRE(font_under_test.width() == 5);
//         REQUIRE(font_under_test.height() == 7);
//         REQUIRE(font_under_test.size() == 665);
//     }

//     SECTION("7x10Font")
//     {
//         static ssd1306::Font7x10 font_under_test;
//         REQUIRE(font_under_test.width() == 7);
//         REQUIRE(font_under_test.height() == 10);
//         REQUIRE(font_under_test.size() == 950);
//     }

//     SECTION("11x16Font")
//     {
//         static ssd1306::Font11x18 font_under_test;
//         REQUIRE(font_under_test.width() == 11);
//         REQUIRE(font_under_test.height() == 18);
//         REQUIRE(font_under_test.size() == 1710);
//     }

//     SECTION("16x26Font")
//     {
//         static ssd1306::Font16x26 font_under_test;
//         REQUIRE(font_under_test.width() == 16);
//         REQUIRE(font_under_test.height() == 26);
//         REQUIRE(font_under_test.size() == 2470);
//     }

// }

// // @brief Test the public API of the ssd1306::Driver class
// TEST_CASE("Test Display public API", "[ssd1306_display_public]")
// {
//     ssd1306::DriverSerialInterface ssd1306_spi_interface (
//         SPI1,
//         SPI1_DC_GPIO_Port,
//         SPI1_DC_Pin,
//         SPI1_RESET_GPIO_Port,
//         SPI1_RESET_Pin);

//     static ssd1306::Font16x26 font_under_test;
//     static ssd1306::Driver oled(ssd1306_spi_interface,
//     ssd1306::Driver::SPIDMA::enabled); REQUIRE(oled.power_on_sequence());

//     SECTION("Write white on black")
//     {
//         std::string text("TEST");
//         REQUIRE(oled.write(text, font_under_test, 2, 2,
//         ssd1306::Colour::Black, ssd1306::Colour::White, true, true) ==
//         ssd1306::ErrorStatus::OK);
//     }
//     SECTION("Write black on white")
//     {
//         std::string text("TEST");
//         REQUIRE(oled.write(text, font_under_test, 2, 2,
//         ssd1306::Colour::White, ssd1306::Colour::Black, true, true) ==
//         ssd1306::ErrorStatus::OK);
//     }
//     SECTION("Write with invalid x position")
//     {
//         std::string text("TEST");
//         REQUIRE_FALSE(oled.write(text, font_under_test, 255, 2,
//         ssd1306::Colour::Black, ssd1306::Colour::White, true, true) ==
//         ssd1306::ErrorStatus::OK);
//     }
//     SECTION("Write with invalid y position")
//     {

//         std::string text("TEST");
//         REQUIRE_FALSE(oled.write(text, font_under_test, 2, 255,
//         ssd1306::Colour::Black, ssd1306::Colour::White, true, true) ==
//         ssd1306::ErrorStatus::OK);
//     }
// }

// // @brief Test the protected API of the ssd1306::Driver class
// TEST_CASE("Test Display protected API", "[ssd1306_display_protected]")
// {
//     static ssd1306::Font16x26 font_under_test;
//     SECTION("Write invalid char")
//     {
//         ssd1306::ssd1306_tester tester;
//         REQUIRE_FALSE(tester.test_write_char(0, font_under_test,
//         ssd1306::Colour::Black, true));
//     }

// }

// // @brief hidden test to find the sum of Font Test. Not to be used in normal
// test runs TEST_CASE("Check font output", "[dump_fonts][.]")
// {
//     static ssd1306::FontTest font_under_test;
//     ssd1306::ssd1306_tester tester;

//     // use this test to find the sums of the buffers for each font character
//     SECTION("Dump and accumulate")
//     {
//         // process the test font character in FontTest (font_test.cpp)

//         // add font character to a string
//         std::string msg{font_under_test.character_map[0]};

//         // write the string to the buffer
//         tester.write(msg, font_under_test, 0, 0, ssd1306::Colour::Black,
//         ssd1306::Colour::White, true, true); std::cout << std::endl <<
//         "+++++++++++++++++++++++++++++++++++++++++++++ BUFFER HEX DUMP
//         ++++++++++++++++++++++++++++++++++++++++" << std::endl;

//         // dump the hex
//         tester.dump_buffer_as_hex();
//         std::cout << std::endl;
//     }
// }

// // @brief Validate the buffer encoding is correct using known valid encoding
// TEST_CASE("SSD1306 Buffer Validation", "[ssd1306_buffer_check]")
// {
//     ssd1306::ssd1306_tester tester;
//     std::vector<uint8_t> invalid_buffer(1024, 0x00);
//     std::vector<uint8_t> wrong_size_buffer(2);
//     SECTION("Buffer Sum Validation for Font11x18")
//     {
//         REQUIRE_FALSE(tester.validate_buffer(wrong_size_buffer));
//         REQUIRE(tester.validate_buffer(tester.m_valid_fonttest_buffer_contents));
//         REQUIRE_FALSE(tester.validate_buffer(invalid_buffer));
//     }

//     SECTION("Dump Hex")
//     {
//         REQUIRE(tester.dump_buffer_as_hex());
//     }
// }

// TEST_CASE("SSD1306 send_data")
// {
//     ssd1306::DriverSerialInterface ssd1306_spi_interface (
//         SPI1,
//         SPI1_DC_GPIO_Port,
//         SPI1_DC_Pin,
//         SPI1_RESET_GPIO_Port,
//         SPI1_RESET_Pin);

//     ssd1306::Font5x7 font_under_test;
//     static ssd1306::Driver oled(ssd1306_spi_interface,
//     ssd1306::Driver::SPIDMA::enabled);

//     std::string msg("Hello");
//     oled.write(msg, font_under_test, 0, 0, ssd1306::Colour::Black,
//     ssd1306::Colour::White, true, true);

// }

// enforce code coverage with explicit instances of func templates so that
// linker does not drop references
// clang-format off
template bool ssd1306::Font5x5::get_pixel(size_t idx, uint32_t &bit_line);
template uint8_t ssd1306::Font5x5::width();
template uint8_t ssd1306::Font5x5::height();
template size_t ssd1306::Font5x5::size();
enum class DummyInterruptType { usart5, capacity };
template ssd1306::Driver<DummyInterruptType>::Driver(const DriverSerialInterface<DummyInterruptType> &display_spi_interface, SPIDMA dma_option);
template bool ssd1306::Driver<DummyInterruptType>::power_on_sequence();
template void ssd1306::Driver<DummyInterruptType>::dma_isr();
template void ssd1306::Driver<DummyInterruptType>::reset();
template ssd1306::ErrorStatus ssd1306::Driver<DummyInterruptType>::update_screen();
template bool ssd1306::Driver<DummyInterruptType>::send_command(uint8_t page_pos_gddram);
template bool ssd1306::Driver<DummyInterruptType>::send_page_data(uint16_t page_pos_gddram);
template ssd1306::ErrorStatus ssd1306::Driver<DummyInterruptType>::write(noarch::containers::StaticString<1> &msg, ssd1306::Font5x5 &font, uint8_t x, uint8_t y, Colour bg, Colour fg, bool padding, bool update);
template ssd1306::DriverSerialInterface<DummyInterruptType>::DriverSerialInterface(SPI_TypeDef *display_spi, std::pair<GPIO_TypeDef*, uint16_t> dc_gpio, std::pair<GPIO_TypeDef*, uint16_t> reset_gpio, DummyInterruptType dma_isr_type);
template SPI_TypeDef& ssd1306::DriverSerialInterface<DummyInterruptType>::get_spi_handle();
template GPIO_TypeDef& ssd1306::DriverSerialInterface<DummyInterruptType>::get_dc_port();
template uint16_t ssd1306::DriverSerialInterface<DummyInterruptType>::get_dc_pin();
template GPIO_TypeDef& ssd1306::DriverSerialInterface<DummyInterruptType>::get_reset_port();
template uint16_t ssd1306::DriverSerialInterface<DummyInterruptType>::get_reset_pin();
template DummyInterruptType ssd1306::DriverSerialInterface<DummyInterruptType>::get_dma_isr_type();
// clang-format on
