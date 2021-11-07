/*
 * Ssd1306.hpp
 *
 *  Created on: 7 Nov 2021
 *      Author: chris
 */

#ifndef SSD1306_HPP_
#define SSD1306_HPP_

#include "stm32g0xx.h"
#include "main.h"
#include "spi.h"
#include "../CPP_SSD1306/fontdef.hpp"

//#define SSD1306_MIRROR_VERT
//#define SSD1306_MIRROR_HORIZ

class Ssd1306
{
public:
	Ssd1306();

	virtual ~Ssd1306() = default;

	enum class Colour: uint16_t
	{
	    Black = 0x00, // Black color, no pixel
	    White = 0x01  // Pixel is set. Color depends on OLED
	};

	// Procedure definitions
	void init(void);
	void fill(Ssd1306::Colour color);
	void update_screen(void);
	void draw_pixel(uint8_t x, uint8_t y, Ssd1306::Colour color);
	char write_char(char ch, Font font, Ssd1306::Colour color, int padding);
	char write_string(std::stringstream &ss, Font font, Ssd1306::Colour color, int padding);
	void set_cursor(uint8_t x, uint8_t y);



private:
	// Low-level procedures
	void reset(void);
	void write_command(uint8_t byte);
	void write_data(uint8_t* buffer, size_t buff_size);

	// display state
    uint16_t current_x {0};
    uint16_t current_y {0};
    uint8_t inverted {0};
    uint8_t initialized {0};

    // OLED dimensions
    static constexpr uint16_t width {128};
    static constexpr uint16_t height {64};
    std::array<uint8_t, (width*height)/8> buffer;

    // SPI configuration
	SPI_HandleTypeDef spi_port {hspi1};
	uint16_t cs_port {0};
	uint16_t cs_pin {0};
	GPIO_TypeDef* dc_port {SPI1_DC_GPIO_Port};
	uint16_t dc_pin {SPI1_DC_Pin};
	GPIO_TypeDef* reset_port {SPI1_RESET_GPIO_Port};
	uint16_t reset_pin {SPI1_RESET_Pin};

};


#endif /* SSD1306_HPP_ */
