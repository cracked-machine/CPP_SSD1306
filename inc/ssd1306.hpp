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



#if defined(USE_SSD1306_HAL_DRIVER) || defined(USE_SSD1306_LL_DRIVER)
	// Required when using GCC 10.3.1 arm-none-eabi 
	// warning: compound assignment with 'volatile'-qualified left operand is deprecated [-Wvolatile]
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wvolatile"
		#include "main.h"
		#include "spi.h"	
	#pragma GCC diagnostic pop
#endif
// this macro is defined in HAL but we still need it when using LL or no stm32 framework
#if !defined(USE_SSD1306_HAL_DRIVER)
    #define UNUSED(X) (void)X 
#endif


namespace ssd1306
{

// @brief colour range of the OLED(!)
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

	// @brief write setup commands to the IC
	bool init();

	// @brief Convenience function to write msg to the display.
	// @tparam FONT_SIZE The size of the font data, Uses template argument deduction.
	// @param msg The message to display
	// @param font The font size object: Font5x5, Font5x7, Font7x10, Font11x18, Font16x26
	// @param x pos
	// @param y pos
	// @param bg The background colour
	// @param fg The foreground colour
	// @param padding add an extra pixel to the vertical edge of the character
	// @param update write the sw buffer to the IC
	// @return char 
	template<std::size_t FONT_SIZE> 
	char write(std::stringstream &msg, Font<FONT_SIZE> &font, uint8_t x, uint8_t y, Colour bg, Colour fg, bool padding, bool update);


	// @brief Get the display width. Can be used to create a std::array
	// @return constexpr uint16_t 
	static constexpr uint16_t get_display_width() { return m_page_width; }

	// @brief Get the display height. Can be used to create a std::array
	// @return constexpr uint16_t 
	static constexpr uint16_t get_display_height() { return m_height; }

	// @brief Debug function to display entire SW bufefr to console (uses RTT on arm, uses std::cout on x86) 
	// @param hex display in hex or decimal values
	void dump_buffer(bool hex);

private:
	
	// @brief Write a pixel to the sw buffer at the corresponding display coordinates 
	// @param x pos
	// @param y pos
	// @param colour white/black
	bool draw_pixel(uint8_t x, uint8_t y, Colour colour);

	// @brief Write single colour to entire sw buffer
	// @param colour 
	void fill(Colour colour);

	// @brief Write the sw buffer to the IC GDDRAM (Page Addressing Mode only)
	bool update_screen();

	// @brief Reset the Driver IC and SW buffer.
	void reset();
	
	// @brief Set the coordinates to draw to the display
	// @param x 
	// @param y 
	bool set_cursor(uint8_t x, uint8_t y);


	// @brief Send one command over SPI 
	// @param cmd_byte The byte to send
	// @return true if success, false if error
	bool send_command(uint8_t cmd_byte);
	
	// @brief send one page of the display buffer over SPI
	// @param page_pos_gddram The index position of the page within the buffer
	// @return true if success, false if error
	bool write_data(uint16_t page_pos_gddram);

	// @brief Check and retry (with timeout) the SPIx_SR TXE register.
	// @param delay_ms The timeout
	// @return true if TX FIFO is empty, false if TX FIFO is full
	bool check_txe_flag_status(uint32_t delay_ms = 1);

	// @brief Check and retry (with timeout) the SPIx_SR BSY register.
	// @param delay_ms The timeout
	// @return true if SPI bus is busy, false if SPI bus is not busy.
	bool check_bsy_flag_status(uint32_t delay_ms = 1);

	// @brief X coordinate for writing to the display
    uint16_t m_currentx {0};

	// @brief Y coordinate for writing to the display
    uint16_t m_currenty {0};

	// @brief The display width in bytes. Also the size of each GDDRAM page
    static const uint16_t m_page_width {128};

	// @brief The display height, in bytes. Also the number of pages (8) multiplied by the bits per page column (8)
    static const uint16_t m_height {64};
	
#ifdef USE_SSD1306_HAL_DRIVER

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
    std::array<uint8_t, (m_page_width*m_height)/8> m_buffer;

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
    if (m_page_width <= (m_currentx + font.height()) ||
        m_page_width <= (m_currenty + font.height()))
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

		// get each bit/pixel in the font_data_word and write it to the display output buffer
        for(size_t font_width_idx = 0; font_width_idx < font.width(); font_width_idx++) 
		{
			// shift left and check if MSB is set: write as foreground pixel
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
			// MSB is not set: write as background pixel
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
