/*
 * Display.cpp
 *
 *  Created on: 7 Nov 2021
 *      Author: chris
 */


#include "ssd1306.hpp"
#include <sstream>

namespace ssd1306
{

Display::Display()
{
	// do nothing else
}


void Display::init(void)
{
	// Reset Display
	reset();

    // Wait for the screen to boot
    HAL_Delay(100);

    // Init Display
    write_command(0xAE); //display off

    write_command(0x20); //Set Memory Addressing Mode
    write_command(0x10); // 00,Horizontal Addressing Mode; 01,Vertical Addressing Mode;
                                // 10,Page Addressing Mode (RESET); 11,Invalid

    write_command(0xB0); //Set Page Start Address for Page Addressing Mode,0-7


    write_command(0xC8); //Set COM Output Scan Direction


    write_command(0x00); //---set low column address
    write_command(0x10); //---set high column address

    write_command(0x40); //--set start line address - CHECK

    write_command(0x81); //--set contrast control register - CHECK
    write_command(0xFF);


    write_command(0xA1); //--set segment re-map 0 to 127 - CHECK



    write_command(0xA6); //--set normal color


    write_command(0xA8); //--set multiplex ratio(1 to 64) - CHECK
    write_command(0x3F); //

    write_command(0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content

    write_command(0xD3); //-set display offset - CHECK
    write_command(0x00); //-not offset

    write_command(0xD5); //--set display clock divide ratio/oscillator frequency
    write_command(0xF0); //--set divide ratio

    write_command(0xD9); //--set pre-charge period
    write_command(0x22); //

    write_command(0xDA); //--set com pins hardware configuration - CHECK
    write_command(0x12);

    write_command(0xDB); //--set vcomh
    write_command(0x20); //0x20,0.77xVcc

    write_command(0x8D); //--set DC-DC enable
    write_command(0x14); //
    write_command(0xAF); //--turn on Display panel

    // Clear screen
    fill(Colour::Black);

    // Flush buffer to screen
    update_screen();

    // Set default values for screen object
    current_x = 0;
    current_y = 0;

    initialized = 1;
}


void Display::fill(Colour color)
{
    for(auto &pixel : buffer)
    {
        pixel = (color == Colour::Black) ? 0x00 : 0xFF;
    }
}

void Display::update_screen(void)
{
    for(uint8_t i = 0; i < 8; i++)
    {
        write_command(0xB0 + i);
        write_command(0x00);
        write_command(0x10);
        write_data(&buffer[width * i], width);
    }
}

void Display::draw_pixel(uint8_t x, uint8_t y, Colour color)
{
    if(x >= width || y >= height) {
        // Don't write outside the buffer
        return;
    }

    // Draw in the right color
    if(color == Colour::White) {
        buffer[x + (y / 8) * width] |= 1 << (y % 8);
    } else {
        buffer[x + (y / 8) * width] &= ~(1 << (y % 8));
    }
}

char Display::write_char(char ch, Font font, Colour color, int padding)
{
    // Check remaining space on current line
    if (width <= (current_x + font.width) ||
        width <= (current_y + font.height))
    {
        // Not enough space on current line
        return 0;
    }

    // add extra leading horizontal space
    if (padding == 1)
    {
    	for(size_t n = 0; n < font.height; n++)
		{
			draw_pixel(current_x, (current_y + n), Colour::Black);
		}
    	current_x += 1;
    }


    // Use the font to write
    uint32_t b;
    for(size_t i = 0; i < font.height; i++) {
        b = font.getChar( (ch - 32) * font.height + i );
        for(size_t j = 0; j < font.width; j++) {
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
    current_x += font.width;
    // add extra leading horizontal space
    if (padding == 1)
    	current_x += 1;

    // Return written char for validation
    return ch;
}

char Display::write_string(std::stringstream &ss, Font font, Colour color, int padding)
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

void Display::set_cursor(uint8_t x, uint8_t y)
{
    current_x = x;
    current_y = y;
}


// Low-level procedures
void Display::reset(void)
{
	// CS = High (not selected)
	//HAL_GPIO_WritePin(Display_CS_Port, Display_CS_Pin, GPIO_PIN_SET);

	// Reset the Display
	HAL_GPIO_WritePin(reset_port, reset_pin, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(reset_port, reset_pin, GPIO_PIN_SET);
	HAL_Delay(10);
}

void Display::write_command(uint8_t cmd_byte)
{
	//HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_RESET); // select Display
	HAL_GPIO_WritePin(dc_port, dc_pin, GPIO_PIN_RESET); // command
	HAL_SPI_Transmit(&spi_port, (uint8_t *) &cmd_byte, 1, HAL_MAX_DELAY);
	//HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_SET); // un-select Display
}

void Display::write_data(uint8_t* data_buffer, size_t data_buffer_size)
{
	//HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_RESET); // select Display
	HAL_GPIO_WritePin(dc_port, dc_pin, GPIO_PIN_SET); // data
	HAL_SPI_Transmit(&spi_port, data_buffer, data_buffer_size, HAL_MAX_DELAY);
	//HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_SET); // un-select Display
}

} // namespace ssd1306
