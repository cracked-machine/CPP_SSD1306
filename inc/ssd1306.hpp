/*
 * Display.hpp
 *
 *  Created on: 7 Nov 2021
 *      Author: chris
 */

#ifndef Display_HPP_
#define Display_HPP_

#include <variant>
#include <font.hpp>
#include <sstream>
#include <iostream>
#include <array>


#ifdef USE_HAL_DRIVER
	#include "stm32g0xx.h"
	#include "main.h"
	#include "spi.h"
#endif


namespace ssd1306
{


enum class Colour: uint16_t
{
    Black = 0x00, // Black colour, no pixel
    White = 0x01  // Pixel is set. Color depends on Display
};


class Display
{
public:
	Display();

	virtual ~Display() = default;

	// Procedure definitions
	void init(void);
	void fill(Colour colour);
	void update_screen(void);
	void draw_pixel(uint8_t x, uint8_t y, Colour colour);

	template<std::size_t SIZE> char write_char(
		char ch, Font<SIZE> &font, 
		Colour colour, 
		int padding);
	

	template<std::size_t SIZE> char write_string(
		std::stringstream &ss, 
		Font<SIZE> &font, 
		Colour colour, 
		int padding);
	

	template<std::size_t SIZE> char write(
		std::stringstream &msg, 
		Font<SIZE> &font, 
		uint8_t x, 
		uint8_t y, 
		Colour bg, 
		Colour fg, 
		int padding, 
		bool update);

	void set_cursor(uint8_t x, uint8_t y);


private:
	// Low-level procedures
	void reset(void);
	void write_command(uint8_t cmd_byte);
	void write_data(uint8_t* data_buffer, size_t data_buffer_size);

	// display state
    uint16_t current_x {0};
    uint16_t current_y {0};
    uint8_t inverted {0};
    uint8_t initialized {0};

    // Display dimensions
    static constexpr uint16_t width {128};
    static constexpr uint16_t height {64};
    std::array<uint8_t, (width*height)/8> buffer;
#ifdef USE_HAL_DRIVER
    // SPI configuration
	SPI_HandleTypeDef spi_port {hspi1};
	uint16_t cs_port {0};
	uint16_t cs_pin {0};
	GPIO_TypeDef* dc_port {SPI1_DC_GPIO_Port};
	uint16_t dc_pin {SPI1_DC_Pin};
	GPIO_TypeDef* reset_port {SPI1_RESET_GPIO_Port};
	uint16_t reset_pin {SPI1_RESET_Pin};
#endif

};

template<std::size_t SIZE>
char Display::write(std::stringstream &msg, Font<SIZE> &font, uint8_t x, uint8_t y, Colour bg, Colour fg, int padding, bool update)
{

    fill(bg);
    set_cursor(x, y);
    char res = write_string(msg, font, fg, padding);
    if (update)
    {
        update_screen();
    }
    return res;
}

template<std::size_t SIZE>
char Display::write_char(char ch, Font<SIZE> &font, Colour color, int padding)
{

#ifndef USE_HAL_DRIVER
    std::cout << ch << std::endl;
#else
    SEGGER_RTT_printf(0, "%c ", ch);
#endif

    // Check remaining space on current line
    if (width <= (current_x + font.height()) ||
        width <= (current_y + font.height()))
    {
        // Not enough space on current line
        return 0;
    }

    // add extra leading horizontal space
    if (padding == 1)
    {
    	for(size_t n = 0; n < font.height(); n++)
		{
			draw_pixel(current_x, (current_y + n), Colour::Black);
		}
    	current_x += 1;
    }


    // Use the font to write
    uint32_t b;
    for(size_t i = 0; i < font.height(); i++) {
        b = font.get_pixel( (ch - 32) * font.height() + i );
        for(size_t j = 0; j < font.width(); j++) {
            if((b << j) & 0x8000)
            {
            	if (color == (Colour::White))
            	{
            		draw_pixel(current_x + j, (current_y + i), Colour::White);
            	}
            	else
            	{
            		draw_pixel(current_x + j, (current_y + i), Colour::Black);
            	}
            }
            else
            {
            	if (color == (Colour::White))
            	{
            		draw_pixel(current_x + j, (current_y + i), Colour::Black);
            	}
            	else
            	{
            		draw_pixel(current_x + j, (current_y + i), Colour::White);
            	}

            }
        }
    }

    // The current space is now taken
    current_x += font.width();
    // add extra leading horizontal space
    if (padding == 1)
    	current_x += 1;

    // Return written char for validation
    return ch;
}

template<std::size_t SIZE>
char Display::write_string(std::stringstream &ss, Font<SIZE> &font, Colour color, int padding)
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

} // namespace ssd1306

#endif /* Display_HPP_ */
