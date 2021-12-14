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

#include "ssd1306.hpp"
#include <iomanip>
#include <bitset>

namespace ssd1306
{

bool Display::init()
{
    bool res = true;
	// Reset Display
	reset();

    // Wait for the screen to boot
#ifdef USE_HAL_DRIVER
    HAL_Delay(100);
#endif
    // Init Display
    if (!write_command(0xAE)) { return false; } //display off
    if (!write_command(0x20)) { return false; } //Set Memory Addressing Mode
    if (!write_command(0x10)) { return false; } // 00,Horizontal Addressing Mode; 01,Vertical Addressing Mode; 10,Page Addressing Mode (RESET); 11,Invalid
    if (!write_command(0xB0)) { return false; } //Set Page Start Address for Page Addressing Mode,0-7
    if (!write_command(0xC8)) { return false; } //Set COM Output Scan Direction
    if (!write_command(0x00)) { return false; } //---set low column address
    if (!write_command(0x10)) { return false; } //---set high column address
    if (!write_command(0x40)) { return false; } //--set start line address - CHECK
    if (!write_command(0x81)) { return false; } //--set contrast control register - CHECK
    if (!write_command(0xFF)) { return false; }
    if (!write_command(0xA1)) { return false; } //--set segment re-map 0 to 127 - CHECK
    if (!write_command(0xA6)) { return false; } //--set normal color
    if (!write_command(0xA8)) { return false; } //--set multiplex ratio(1 to 64) - CHECK
    if (!write_command(0x3F)) { return false; } //
    if (!write_command(0xA4)) { return false; } //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
    if (!write_command(0xD3)) { return false; } //-set display offset - CHECK
    if (!write_command(0x00)) { return false; } //-not offset
    if (!write_command(0xD5)) { return false; } //--set display clock divide ratio/oscillator frequency
    if (!write_command(0xF0)) { return false; } //--set divide ratio
    if (!write_command(0xD9)) { return false; } //--set pre-charge period
    if (!write_command(0x22)) { return false; } //
    if (!write_command(0xDA)) { return false; } //--set com pins hardware configuration - CHECK
    if (!write_command(0x12)) { return false; }
    if (!write_command(0xDB)) { return false; } //--set vcomh
    if (!write_command(0x20)) { return false; } //0x20,0.77xVcc
    if (!write_command(0x8D)) { return false; } //--set DC-DC enable
    if (!write_command(0x14)) { return false; } //
    if (!write_command(0xAF)) { return false; } //--turn on Display panel

    // Clear screen
    fill(Colour::Black);

    // Flush buffer to screen
    update_screen();

    // Set default values for screen object
    m_currentx = 0;
    m_currenty = 0;

    m_initialized = 1;

    return res;
}


void Display::fill(Colour color)
{
    for(auto &pixel : m_buffer)
    {
        pixel = (color == Colour::Black) ? 0x00 : 0xFF;
    }
}

bool Display::update_screen()
{
    for(uint8_t i = 0; i < 8; i++)
    {
        if (!write_command(0xB0 + i)) { return false; }
        if (!write_command(0x00)) { return false; }
        if (!write_command(0x10)) { return false; }
        if (!write_data(&m_buffer[m_width * i], m_width)) { return false; }
    }
    return true;
}

bool Display::draw_pixel(uint8_t x, uint8_t y, Colour color)
{
    // Draw in the right color
    if(color == Colour::White) 
    {
        m_buffer[x + (y / 8) * m_width] |= 1 << (y % 8);
#ifdef ENABLE_SSD1306_TEST_STDOUT
        std::cout << "1";
#endif
    } 
    else 
    {
        m_buffer[x + (y / 8) * m_width] &= ~(1 << (y % 8));
#ifdef ENABLE_SSD1306_TEST_STDOUT
        std::cout << "_";
#endif
    }

    return true;
}

bool Display::set_cursor(uint8_t x, uint8_t y)
{ 
    if(x >= m_width || y >= m_height) 
    {
        return false;
    }
    else
    {
        m_currentx = x;
        m_currenty = y;
    }
    return true;
}


void Display::reset()
{
	// CS = High (not selected)
	//HAL_GPIO_WritePin(Display_CS_Port, Display_CS_Pin, GPIO_PIN_SET);

	// Reset the Display
#ifdef USE_HAL_DRIVER
	HAL_GPIO_WritePin(m_reset_port, m_reset_pin, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(m_reset_port, m_reset_pin, GPIO_PIN_SET);
	HAL_Delay(10);
#endif
}

bool Display::write_command(uint8_t cmd_byte)
{
#ifdef USE_HAL_DRIVER
    HAL_StatusTypeDef res = HAL_OK;
	//HAL_GPIO_WritePin(m_cs_port, m_cs_pin, GPIO_PIN_RESET); // select Display
	HAL_GPIO_WritePin(m_dc_port, m_dc_pin, GPIO_PIN_RESET); // command
	res = HAL_SPI_Transmit(&m_spi_port, (uint8_t *) &cmd_byte, 1, HAL_MAX_DELAY);
    if (res != HAL_OK)
    {
        return false;
    }
    return true;
	//HAL_GPIO_WritePin(m_cs_port, m_cs_pin, GPIO_PIN_SET); // un-select Display
#else
    UNUSED(cmd_byte);
    return true;
#endif
}

bool Display::write_data(uint8_t* data_buffer, size_t data_buffer_size)
{
#ifdef USE_HAL_DRIVER
    HAL_StatusTypeDef res = HAL_OK;
	//HAL_GPIO_WritePin(m_cs_port, m_cs_pin, GPIO_PIN_RESET); // select Display
	HAL_GPIO_WritePin(m_dc_port, m_dc_pin, GPIO_PIN_SET); // data
	res = HAL_SPI_Transmit(&m_spi_port, data_buffer, data_buffer_size, HAL_MAX_DELAY);
    if (res != HAL_OK)
    {
        return false;
    }
    return true;
	//HAL_GPIO_WritePin(m_cs_port, m_cs_pin, GPIO_PIN_SET); // un-select Display
#else
    UNUSED(data_buffer);
    UNUSED(data_buffer_size);
    return true;
#endif



} 

} // namespace ssd1306
