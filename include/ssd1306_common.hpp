// MIT License

// Copyright (c) 2022 Chris Sutton

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef __SSD1306_COMMON_HPP__
#define __SSD1306_COMMON_HPP__


#include <font.hpp>
// #define USE_STD_STRING
// #ifdef USE_STD_STRING
// #include <string>
// #endif
#include <isr_manager_stm32g0.hpp>
#include <static_string.hpp>

#if defined(X86_UNIT_TESTING_ONLY)
	// only used when unit testing on x86
	#include <iostream>
#else
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wvolatile"
		#include <stm32g0xx_ll_dma.h>
		#include <stm32g0xx_ll_gpio.h>
	#pragma GCC diagnostic pop
    #include <bitset_utils.hpp>
       
#endif

#include <spi_utils.hpp> 

namespace ssd1306
{

// @brief colour range of the OLED(!)
enum class Colour: uint16_t
{
    Black = 0x00, 
    White = 0x01  
};

// @brief SSD1306 error status
enum class ErrorStatus {
	// @brief operation successful
	OK,
	// @brief write overflowed SSD1306 display width/height
	LINE_OVRFLW,
	// @brief provided pixel index is out of bounds
	PIXEL_OOB,
	// @brief cursor falls outside of display area
	CURSOR_OOB,
	// @brief error sending Page position command to SSD1306
	START_PAGE_ERR,
	// @brief error sending low column value command to SSD1306
	START_LCOL_ERR,
	// @brief error sending high column value command to SSD1306
	START_HCOL_ERR,
	// @brief error sending buffer data command to SSD1306
	SEND_DATA_ERR,
	// @brief bad things happened here
	UNKNOWN_ERR
};

class CommonFunctions
{

public:

	// @brief X coordinate for writing to the display
    uint16_t m_currentx {0};

	// @brief Y coordinate for writing to the display
    uint16_t m_currenty {0};

	// @brief The display width in bytes. Also the size of each GDDRAM page
    static const uint16_t m_page_width {128};

	// @brief The display height, in bytes. Also the number of pages (8) multiplied by the bits per page column (8)
    static const uint16_t m_height {64};

	// @brief byte buffer for ssd1306. Access to derived classes like ssd1306_tester is permitted.
    std::array<uint8_t, (m_page_width*m_height)/8> m_buffer;    

	// @brief Get the display width. Can be used to create a std::array
	// @return constexpr uint16_t 
	static constexpr uint16_t get_display_width() { return m_page_width; }

	// @brief Get the display height. Can be used to create a std::array
	// @return constexpr uint16_t 
	static constexpr uint16_t get_display_height() { return m_height; }

	// @brief Write single colour to entire sw buffer
	// @param colour 
	void fill(Colour colour);    


	// @brief Write a pixel to the sw buffer at the corresponding display coordinates 
	// @param x pos
	// @param y pos
	// @param colour white/black
	void draw_pixel(uint8_t x, uint8_t y, Colour colour);    

	// @brief Set the coordinates to draw to the display
	// @param x 
	// @param y 
	bool set_cursor(uint8_t x, uint8_t y);

protected:

#ifdef USE_STD_STRING	
	// @brief 
	// @tparam FONT_SIZE 
	// @param ss 
	// @param font 
	// @param colour 
	// @param padding 
	// @return ErrorStatus 
	template<std::size_t FONT_SIZE>
	ErrorStatus write_string(std::string &msg, Font<FONT_SIZE> &font, Colour colour, bool padding);
#else
	template<std::size_t FONT_SIZE, std::size_t MSG_SIZE>
	ErrorStatus write_string(noarch::containers::StaticString<MSG_SIZE> &msg, Font<FONT_SIZE> &font, Colour colour, bool padding);	
#endif // #ifdef USE_STD_STRING

	// @brief 
	// @tparam FONT_SIZE 
	// @param ch 
	// @param font 
	// @param colour 
	// @param padding 
	// @return ErrorStatus 
	template<std::size_t FONT_SIZE> 
	ErrorStatus write_char(char ch, Font<FONT_SIZE> &font, Colour colour, bool padding);

};

#ifdef USE_STD_STRING
template<std::size_t FONT_SIZE>
ErrorStatus CommonFunctions::write_string(std::string &msg, Font<FONT_SIZE> &font, Colour color, bool padding)
{
    // Write until null-byte
	for (char &c : msg)
	{
		ErrorStatus res = write_char(c, font, color, padding);
		if (res != ErrorStatus::OK)
		{
			return res;
		}
	}
    return ErrorStatus::OK;
}
#else
template<std::size_t FONT_SIZE, std::size_t MSG_SIZE>
ErrorStatus CommonFunctions::write_string(noarch::containers::StaticString<MSG_SIZE> &msg, Font<FONT_SIZE> &font, Colour colour, bool padding)
{
    // Write until null-byte
	for (char &c : msg.array())
	{
		ErrorStatus res = write_char(c, font, colour, padding);
		if (res != ErrorStatus::OK)
		{
			return res;
		}
	}
    return ErrorStatus::OK;
}
#endif //#ifdef USE_STD_STRING


template<std::size_t FONT_SIZE>
ErrorStatus CommonFunctions::write_char(char ch, Font<FONT_SIZE> &font, Colour colour, bool padding)
{

    // Check remaining space on current line
    if (m_page_width <= (m_currentx + font.height()) ||
        m_page_width <= (m_currenty + font.height()))
    {
        // Not enough space on current line
        return ErrorStatus::OK;
    }

    // add extra leading horizontal space
    if (padding)
    {
    	for(size_t n = 0; n < font.height(); n++)
		{
			draw_pixel(m_currentx, (m_currenty + n), Colour::Black);
		}
    	m_currentx += 1;
    }

    // Use the font to write
    uint32_t font_data_word;
    for(size_t font_height_idx = 0; font_height_idx < font.height(); font_height_idx++) 
	{
        if (!font.get_pixel( (ch - 32) * font.height() + font_height_idx, font_data_word )) { return ErrorStatus::PIXEL_OOB; }

		#ifdef ENABLE_SSD1306_TEST_STDOUT
				// separator for the font
				std::cout << std::endl;
		#endif

		// get each bit/pixel in the font_data_word and write it to the display output buffer
        for(size_t font_width_idx = 0; font_width_idx < font.width(); font_width_idx++) 
		{
			// shift left and check if MSB is set: write as foreground pixel
            if((font_data_word << font_width_idx) & 0x8000)
            {
            	switch (colour)
				{
					case Colour::White:
						draw_pixel(m_currentx + font_width_idx, m_currenty + font_height_idx, Colour::White);
						break;
					
					case Colour::Black:
						draw_pixel(m_currentx + font_width_idx, m_currenty + font_height_idx, Colour::Black);
						break;
				}			
            }
			// MSB is not set: write as background pixel
            else
            {
            	switch (colour)
				{
					case Colour::White:
						draw_pixel(m_currentx + font_width_idx, m_currenty + font_height_idx, Colour::Black);
						break;
					
					case Colour::Black:
						draw_pixel(m_currentx + font_width_idx, m_currenty + font_height_idx, Colour::White);
						break;
				}						
            }
        }
    }

    // The current space is now taken
    m_currentx += font.width();

    // add extra leading horizontal space
    if (padding)
	{
    	m_currentx += 1;
	}

    // Return written char for validation
    return ErrorStatus::OK;
}

} // namespace ssd1306

#endif // __SSD1306_COMMON_HPP__