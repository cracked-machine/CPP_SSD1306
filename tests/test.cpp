


#include "../../build/catch.hpp"
#include <iostream>
#include <ssd1306.hpp>
#include <array>
#include <sstream>

TEST_CASE("Test SSD1306 Driver", "[ssd1306]")
{
    uint8_t count = 0;
    ssd1306::FontData fd;
	ssd1306::Font xs_font;
	xs_font.set_font(fd.Font3x5);

    ssd1306::Display oled;
    oled.init();

    std::array<char, 10> digit_ascii {'0','1','2','3','4','5','6','7','8','9'};
    std::stringstream msg;
    msg << digit_ascii[count];
    oled.write(msg, xs_font, 2, 2, ssd1306::Colour::Black, ssd1306::Colour::White, 3, true);
    if (count < digit_ascii.size() - 1) { count++; }
    else { count = 0; }	    
    
}