/*
 * Display.hpp
 *
 *  Created on: 7 Nov 2021
 *      Author: chris
 */

#ifndef Display_HPP_
#define Display_HPP_


#include <font3x5.hpp>
#include <font5x7.hpp>
#include <font7x10.hpp>
#include <font11x16.hpp>
#include <font16x26.hpp>

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
	char write_char(char ch, Font &font, Colour colour, int padding);
	char write_string(std::stringstream &ss, Font &font, Colour colour, int padding);
	char write(std::stringstream &msg, Font &font, uint8_t x, uint8_t y, Colour bg, Colour fg, int padding, bool update);
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

} // namespace ssd1306

#endif /* Display_HPP_ */