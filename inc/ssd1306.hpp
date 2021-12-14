// MIT License

// Copyright (c) 2021 Chris Sutton

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

// @note See datasheet
// https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf

#ifndef Display_HPP_
#define Display_HPP_

#include <variant>
#include <font.hpp>
#include <sstream>
#include <iostream>
#include <array>
#include <utility>



#ifdef USE_HAL_DRIVER
	// Required when using GCC 10.3.1 arm-none-eabi 
	// warning: compound assignment with 'volatile'-qualified left operand is deprecated [-Wvolatile]
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wvolatile"
		#include "main.h"
		#include "spi.h"	
	#pragma GCC diagnostic pop
#else
    #define UNUSED(X) (void)X 
#endif


namespace ssd1306
{

// @brief 
enum class Colour: uint16_t
{
    Black = 0x00, 
    White = 0x01  
};

// @brief 
class Display
{
public:
	Display() = default;

	virtual ~Display() = default;

	// @brief 
	bool init();


	// @brief 
	// @tparam FONT_SIZE 
	// @param msg 
	// @param font 
	// @param x 
	// @param y 
	// @param bg 
	// @param fg 
	// @param padding 
	// @param update 
	// @return char 
	template<std::size_t FONT_SIZE> 
	char write(std::stringstream &msg, Font<FONT_SIZE> &font, uint8_t x, uint8_t y, Colour bg, Colour fg, bool padding, bool update);


	// @brief Get the display width. Can be used to create a std::array
	// @return constexpr uint16_t 
	static constexpr uint16_t get_display_width() { return m_width; }

	// @brief Get the display height. Can be used to create a std::array
	// @return constexpr uint16_t 
	static constexpr uint16_t get_display_height() { return m_height; }

private:
	
	// @brief 
	// @param x 
	// @param y 
	// @param colour 
	bool draw_pixel(uint8_t x, uint8_t y, Colour colour);

	// @brief 
	// @param colour 
	void fill(Colour colour);

	// @brief 
	bool update_screen();

	// @brief 
	void reset();
	
	// @brief Set the cursor object
	// @param x 
	// @param y 
	bool set_cursor(uint8_t x, uint8_t y);


	// @brief 
	// @param cmd_byte 
	bool write_command(uint8_t cmd_byte);
	
	// @brief 
	// @param data_buffer 
	// @param data_buffer_size 
	bool write_data(uint8_t* data_buffer, size_t data_buffer_size);

	// @brief 
    uint16_t m_currentx {0};

	// @brief 
    uint16_t m_currenty {0};

	// @brief 
    uint8_t m_inverted {0};

	// @brief 
    uint8_t m_initialized {0};

	// @brief The display width in bytes. Used in std::array.
    static const uint16_t m_width {128};

	// @brief The display height, in bytes. Used in std::array.
    static const uint16_t m_height {64};
	
#ifdef USE_HAL_DRIVER

	// @brief 
	SPI_HandleTypeDef m_spi_port {hspi1};
	// @brief 
	uint16_t m_cs_port {0};
	// @brief 
	uint16_t m_cs_pin {0};
	// @brief 
	GPIO_TypeDef* m_dc_port {SPI1_DC_GPIO_Port};
	// @brief 
	uint16_t m_dc_pin {SPI1_DC_Pin};
	// @brief 
	GPIO_TypeDef* m_reset_port {SPI1_RESET_GPIO_Port};
	// @brief 
	uint16_t m_reset_pin {SPI1_RESET_Pin};

#endif

protected:

	// @brief byte buffer for ssd1306. Access to derived classes like ssd1306_tester is permitted.
    std::array<uint8_t, (m_width*m_height)/8> m_buffer;

	// @brief 
	// @tparam FONT_SIZE 
	// @param ss 
	// @param font 
	// @param colour 
	// @param padding 
	// @return char 
	template<std::size_t FONT_SIZE> 
	char write_string(std::stringstream &ss, Font<FONT_SIZE> &font, Colour colour, bool padding);

	// @brief 
	// @tparam FONT_SIZE 
	// @param ch 
	// @param font 
	// @param colour 
	// @param padding 
	// @return char 
	template<std::size_t FONT_SIZE> 
	char write_char(char ch, Font<FONT_SIZE> &font, Colour colour, bool padding);
	

	// @brief Get the buffer object. Used for testing only.
	// @notes use
	// @param buffer 
	//void get_buffer(std::array<uint8_t, (m_width*m_height)/8> &buffer) { buffer = m_buffer; }

};

// Out-of-class definitions of member function templates 

template<std::size_t FONT_SIZE>
char Display::write(std::stringstream &msg, Font<FONT_SIZE> &font, uint8_t x, uint8_t y, Colour bg, Colour fg, bool padding, bool update)
{

    fill(bg);
    if (!set_cursor(x, y))
	{
		return 0;
	}
    char res = write_string(msg, font, fg, padding);
    if (update)
    {
        update_screen();
    }
    return res;
}

template<std::size_t FONT_SIZE>
char Display::write_string(std::stringstream &ss, Font<FONT_SIZE> &font, Colour color, bool padding)
{
    // Write until null-byte
	char ch;
    while (ss.get(ch))
    {
        if (write_char(ch, font, color, padding) != ch)
        {
            // Char could not be written
            return ch;
        }
    }

    // Everything ok
    return ch;
}

template<std::size_t FONT_SIZE>
char Display::write_char(char ch, Font<FONT_SIZE> &font, Colour colour, bool padding)
{

    // Check remaining space on current line
    if (m_width <= (m_currentx + font.height()) ||
        m_width <= (m_currenty + font.height()))
    {
        // Not enough space on current line
        return 0;
    }

    // add extra leading horizontal space
    if (padding)
    {
    	for(size_t n = 0; n < font.height(); n++)
		{
			if (!draw_pixel(m_currentx, (m_currenty + n), Colour::Black))
			{
				return false;
			}
		}
    	m_currentx += 1;
    }

    // Use the font to write
    uint32_t font_data_word;
    for(size_t font_height_idx = 0; font_height_idx < font.height(); font_height_idx++) 
	{
        if (!font.get_pixel( (ch - 32) * font.height() + font_height_idx, font_data_word )) { return false; }

#ifdef ENABLE_SSD1306_TEST_STDOUT
		// separator for the font
        std::cout << std::endl;
#endif

        for(size_t font_width_idx = 0; font_width_idx < font.width(); font_width_idx++) 
		{
            if((font_data_word << font_width_idx) & 0x8000)
            {
            	switch (colour)
				{
					case Colour::White:
						if (!draw_pixel(m_currentx + font_width_idx, m_currenty + font_height_idx, Colour::White))
						{
							return false;
						}
						break;
					
					case Colour::Black:
						if (!draw_pixel(m_currentx + font_width_idx, m_currenty + font_height_idx, Colour::Black))
						{
							return false;
						}
						break;
				}			
            }
            else
            {
            	switch (colour)
				{
					case Colour::White:
						if (!draw_pixel(m_currentx + font_width_idx, m_currenty + font_height_idx, Colour::Black))
						{
							return false;
						}
						break;
					
					case Colour::Black:
						if (!draw_pixel(m_currentx + font_width_idx, m_currenty + font_height_idx, Colour::White))
						{
							return false;
						}
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
    return ch;
}



} // namespace ssd1306

#endif /* Display_HPP_ */
