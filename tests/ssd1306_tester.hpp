#ifndef __SSD1306_TESTER_HPP__
#define __SSD1306_TESTER_HPP__

#include <ssd1306.hpp>

namespace ssd1306
{

// @brief Tester class inherits protected `ssd1106::Display::get_buffer()` accessor
class ssd1306_tester : public ssd1306::Display
{
public:
    ssd1306_tester();

    // @brief Helper function to provide protected access to ssd1306::Display::write_string()
    // @tparam FONT_SIZE 
    // @param ss 
    // @param font 
    // @param colour 
    // @param padding 
    // @return char 
    template<std::size_t FONT_SIZE> 
    char test_write_string(std::stringstream &ss, Font<FONT_SIZE> &font, Colour colour, bool padding);

    // @brief Helper function to provide protected access to ssd1306::Display::write_char()
    // @tparam FONT_SIZE 
    // @param ch 
    // @param font 
    // @param colour 
    // @param padding 
    // @return char 
    template<std::size_t FONT_SIZE>
    char test_write_char(char ch, Font<FONT_SIZE> &font, Colour colour, bool padding);

    // @brief prints the contents of the display buffer. Call write() first or buffer maybe empty.
    // @return always true 
    bool print_buffer_data();

private:
    // @brief The font size under test
    ssd1306::Font16x26 m_font;

    // @brief used to calculate display size
    static const size_t byte_size {8};

    // @brief calculate the display dimensions at compile time
    static constexpr size_t display_size = get_display_width() * get_display_height() / byte_size;

    // @brief local copy of the ssd1306::Display data buffer 
    std::array<uint8_t, display_size> m_buffer;
};


template<std::size_t FONT_SIZE> 
char ssd1306_tester::test_write_string(std::stringstream &ss, Font<FONT_SIZE> &font, Colour colour, bool padding)
{
    return write_string(ss, font, colour, padding);
}

template<std::size_t FONT_SIZE>
char ssd1306_tester::test_write_char(char ch, Font<FONT_SIZE> &font, Colour colour, bool padding)
{
    return write_char(ch, font, colour, padding);
}

} // namespace ssd1306

#endif // __SSD1306_TESTER_HPP__