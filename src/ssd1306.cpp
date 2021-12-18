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
#include <type_traits>
#include <cstring>

namespace ssd1306
{

bool Display::init()
{
    #if defined(USE_SSD1306_LL_DRIVER)
        LL_SPI_Enable(SPI1);
    #endif

    bool res = true;

	reset();

    // Wait for the screen to boot
    #if defined(USE_SSD1306_HAL_DRIVER)
        HAL_Delay(100);
    #elif defined(USE_SSD1306_LL_DRIVER)
        LL_mDelay(100);
    #endif

    if (!write_command(0xAE)) { return false; } // display off
    if (!write_command(0x20)) { return false; } // Set Memory Addressing Mode
    if (!write_command(0x10)) { return false; } // 00,Horizontal Addressing Mode; 01,Vertical Addressing Mode; 10,Page Addressing Mode (RESET); 11,Invalid
    if (!write_command(0xB0)) { return false; } // Set Page Start Address for Page Addressing Mode,0-7
    if (!write_command(0xC8)) { return false; } // Set COM Output Scan Direction - 0xC0/0xC8 (mirror vertically)
    if (!write_command(0x00)) { return false; } // set low column address
    if (!write_command(0x10)) { return false; } // set high column address
    if (!write_command(0x40)) { return false; } // set start line address
    if (!write_command(0x81)) { return false; } // set contrast control register 
    if (!write_command(0xFF)) { return false; } // set constrast value
    if (!write_command(0xA0)) { return false; } // set segment re-map 0 to 127 - 0xA0/0xA1 (mirror horizontally)
    if (!write_command(0xA6)) { return false; } // set normal color - 0xA6/0xA7 (colour inverse)
    if (!write_command(0xA8)) { return false; } // set multiplex ratio(1 to 64)
    if (!write_command(0x3F)) { return false; } //
    if (!write_command(0xA4)) { return false; } // 0xA4 output follows RAM content, 0xA5 output ignores RAM content
    if (!write_command(0xD3)) { return false; } // set display offset
    if (!write_command(0x00)) { return false; } // not offset
    if (!write_command(0xD5)) { return false; } // set display clock divide ratio/oscillator frequency
    if (!write_command(0xF0)) { return false; } // set divide ratio
    if (!write_command(0xD9)) { return false; } // set pre-charge period
    if (!write_command(0x22)) { return false; } //
    if (!write_command(0xDA)) { return false; } // set com pins hardware configuration
    if (!write_command(0x12)) { return false; }
    if (!write_command(0xDB)) { return false; } // set vcomh
    if (!write_command(0x20)) { return false; } // 0x20,0.77xVcc
    if (!write_command(0x8D)) { return false; } // set DC-DC enable
    if (!write_command(0x14)) { return false; } //
    if (!write_command(0xAF)) { return false; } // turn on Display panel

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
        // Set Page position to write to: 0-7
        if (!write_command(0xB0 + i)) { return false; }

        // Set the lower start column address of pointer by command 00h~0Fh.
        if (!write_command(0x00)) { return false; }

        // Set the upper start column address of pointer by command 10h~1Fh
        if (!write_command(0x10)) { return false; }

        // the index of the page within the buffer
        uint16_t page_idx {static_cast<uint16_t>( m_page_width * i )};

        if (!write_data(page_idx)) { return false; }
    }
        
    return true;
}

bool Display::draw_pixel(uint8_t x, uint8_t y, Colour color)
{
    // Draw in the right color
    if(color == Colour::White) 
    {
        m_buffer[x + (y / 8) * m_page_width] |= 1 << (y % 8);
        #ifdef ENABLE_SSD1306_TEST_STDOUT
                std::cout << "1";
        #endif
    } 
    else 
    {
        m_buffer[x + (y / 8) * m_page_width] &= ~(1 << (y % 8));
        #ifdef ENABLE_SSD1306_TEST_STDOUT
                std::cout << "_";
        #endif
    }

    return true;
}

bool Display::set_cursor(uint8_t x, uint8_t y)
{ 
    if(x >= m_page_width || y >= m_height) 
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
    #ifdef USE_SSD1306_HAL_DRIVER
        HAL_GPIO_WritePin(m_reset_port, m_reset_pin, GPIO_PIN_RESET);
        HAL_Delay(10);
        HAL_GPIO_WritePin(m_reset_port, m_reset_pin, GPIO_PIN_SET);
        HAL_Delay(10);
    #elif defined(USE_SSD1306_LL_DRIVER)
        LL_GPIO_ResetOutputPin(SPI1_RESET_GPIO_Port, SPI1_RESET_Pin);
        LL_mDelay(10);
        LL_GPIO_SetOutputPin(SPI1_RESET_GPIO_Port, SPI1_RESET_Pin);
        LL_mDelay(10);
    #endif
}

bool Display::write_command(uint8_t cmd_byte)
{
    #if defined(USE_SSD1306_HAL_DRIVER)
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
    #elif defined(USE_SSD1306_LL_DRIVER)
        // low signal is command mode
        LL_GPIO_ResetOutputPin(SPI1_DC_GPIO_Port, SPI1_DC_Pin);
        LL_SPI_TransmitData8(SPI1, cmd_byte);
        return true;
    #else
        UNUSED (cmd_byte);
        return true;
    #endif
    
}

bool Display::write_data(uint16_t page_start_idx)
{
    #if defined(USE_SSD1306_HAL_DRIVER)
        HAL_StatusTypeDef res = HAL_OK;
        //HAL_GPIO_WritePin(m_cs_port, m_cs_pin, GPIO_PIN_RESET); // select Display
        HAL_GPIO_WritePin(m_dc_port, m_dc_pin, GPIO_PIN_SET); // data
        res = HAL_SPI_Transmit(&m_spi_port, data_buffer[page_start_idx], m_page_width, HAL_MAX_DELAY);
        if (res != HAL_OK)
        {
            return false;
        }
        return true;
        //HAL_GPIO_WritePin(m_cs_port, m_cs_pin, GPIO_PIN_SET); // un-select Display
    #else
        #if defined(USE_SSD1306_LL_DRIVER)
            // high signal is data mode
            LL_GPIO_SetOutputPin(SPI1_DC_GPIO_Port, SPI1_DC_Pin);
        #endif

        // transmit bytes from this page (page_start_idx -> page_start_idx + m_page_width) over SPI
        uint8_t byte_len {8};
        for (uint16_t idx = page_start_idx; idx < page_start_idx + m_page_width; idx += byte_len)
        {
            if (idx < m_buffer.size())
            {
                #if defined(USE_SSD1306_LL_DRIVER)
                    LL_SPI_TransmitData8(SPI1, m_buffer[idx]);
                #else
                    std::cout << "0x" << std::hex << std::setfill ('0') <<  std::setw(2) << +m_buffer[page_start_idx] << " ";
                #endif
            }
        }
        return true;
    #endif
} 

} // namespace ssd1306
