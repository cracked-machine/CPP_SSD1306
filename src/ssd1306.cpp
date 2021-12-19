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

	reset();

    // Now wait for the screen to boot
    #if defined(USE_SSD1306_HAL_DRIVER)
        HAL_Delay(100);
    #elif defined(USE_SSD1306_LL_DRIVER)
        LL_mDelay(10);
    #endif

    if (!send_command(0xAE)) { return false; } // display off
    if (!send_command(0x20)) { return false; } // Set Memory Addressing Mode
    if (!send_command(0x10)) { return false; } // 00,Horizontal Addressing Mode; 01,Vertical Addressing Mode; 10,Page Addressing Mode (RESET); 11,Invalid
    if (!send_command(0xB0)) { return false; } // Set Page Start Address for Page Addressing Mode,0-7
    if (!send_command(0xC8)) { return false; } // Set COM Output Scan Direction - 0xC0/0xC8 (mirror vertically)
    if (!send_command(0x00)) { return false; } // set low column address
    if (!send_command(0x10)) { return false; } // set high column address
    if (!send_command(0x40)) { return false; } // set start line address
    if (!send_command(0x81)) { return false; } // set contrast control register 
    if (!send_command(0xFF)) { return false; } // set constrast value
    if (!send_command(0xA1)) { return false; } // set segment re-map 0 to 127 - 0xA0/0xA1 (mirror horizontally)
    if (!send_command(0xA6)) { return false; } // set normal color - 0xA6/0xA7 (colour inverse)
    if (!send_command(0xA8)) { return false; } // set multiplex ratio(1 to 64)
    if (!send_command(0x3F)) { return false; } //
    if (!send_command(0xA4)) { return false; } // 0xA4 output follows RAM content, 0xA5 output ignores RAM content
    if (!send_command(0xD3)) { return false; } // set display offset
    if (!send_command(0x00)) { return false; } // not offset
    if (!send_command(0xD5)) { return false; } // set display clock divide ratio/oscillator frequency
    if (!send_command(0xF0)) { return false; } // set divide ratio
    if (!send_command(0xD9)) { return false; } // set pre-charge period
    if (!send_command(0x22)) { return false; } //
    if (!send_command(0xDA)) { return false; } // set com pins hardware configuration
    if (!send_command(0x12)) { return false; }
    if (!send_command(0xDB)) { return false; } // set vcomh
    if (!send_command(0x20)) { return false; } // 0x20,0.77xVcc
    if (!send_command(0x8D)) { return false; } // set DC-DC enable
    if (!send_command(0x14)) { return false; } //
    if (!send_command(0xAF)) { return false; } // turn on Display panel

    // Clear screen
    fill(Colour::White);

    // Flush buffer to screen
    update_screen();

    // Set default values for screen object
    m_currentx = 0;
    m_currenty = 0;

    return true;
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
    for(uint8_t page_idx = 0; page_idx < 8; page_idx++)
    {
        // Set Page position to write to: 0-7
        if (!send_command(0xB0 + page_idx)) { return false; }

        // Set the lower start column address of pointer by command 00h~0Fh.
        if (!send_command(0x00)) { return false; }

        // Set the upper start column address of pointer by command 10h~1Fh
        if (!send_command(0x10)) { return false; }

        // the next page position within the GDDRAM buffer
        uint16_t page_pos_gddram {static_cast<uint16_t>( m_page_width * page_idx )};

        if (!write_data(page_pos_gddram)) { return false; }

        //dump_buffer(true);
    }
        
    return true;
}

bool Display::send_command(uint8_t cmd_byte)
{
    #if defined(USE_RTT)
        SEGGER_RTT_printf(0, "\nCommand Byte: 0x%02x", +cmd_byte);
    #endif  

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
            if (!check_txe_flag_status())
            {
                #if defined(USE_RTT) 
                    SEGGER_RTT_printf(0, "\nwrite_command(): Tx buffer is full"); 
                #endif
            }
            if (!check_bsy_flag_status())
            {
                #if defined(USE_RTT) 
                    SEGGER_RTT_printf(0, "\nwrite_command(); SPI bus is busy"); 
                #endif
            }  
        // set cmd mode/low signal after we put data into TXFIFO to avoid premature latching
        LL_GPIO_ResetOutputPin(SPI1_DC_GPIO_Port, SPI1_DC_Pin);      
        LL_SPI_TransmitData8(SPI1, cmd_byte);    
        
        return true;
    #else
        UNUSED (cmd_byte);
        return true;
    #endif
    
}

bool Display::write_data(uint16_t page_pos_gddram)
{
    #if defined(USE_SSD1306_HAL_DRIVER)

        HAL_StatusTypeDef res = HAL_OK;        
        HAL_GPIO_WritePin(m_dc_port, m_dc_pin, GPIO_PIN_SET); // data
        res = HAL_SPI_Transmit(&m_spi_port, data_buffer[page_pos_gddram], m_page_width, HAL_MAX_DELAY);
        if (res != HAL_OK)
        {
            return false;
        }
        return true;

    #elif defined(USE_SSD1306_LL_DRIVER)

        // transmit bytes from this page (page_pos_gddram -> page_pos_gddram + m_page_width)
        for (uint16_t idx = page_pos_gddram; idx < page_pos_gddram + m_page_width; idx++)
        {
            if (!check_txe_flag_status())
            {
                #if defined(USE_RTT) 
                    SEGGER_RTT_printf(0, "\nwrite_data(): Tx buffer is full."); 
                #endif
            }
            if (!check_bsy_flag_status())
            {
                #if defined(USE_RTT) 
                    SEGGER_RTT_printf(0, "\nwrite_data(): SPI bus is busy."); 
                #endif
            }                          
            LL_SPI_TransmitData8(SPI1, m_buffer[idx]);
            // set data mode/high signal after we put data into TXFIFO to avoid premature latching
            LL_GPIO_SetOutputPin(SPI1_DC_GPIO_Port, SPI1_DC_Pin);         
        }
        return true;
    #endif  // defined(USE_SSD1306_HAL_DRIVER)
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
	// Signal the driver IC to reset the OLED display
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

    // reset the sw buffer 
    m_buffer.fill(0);
}


bool Display::check_txe_flag_status(uint32_t delay_ms)
{
    // The TXE flag is set when transmission TXFIFO has enough space to store data to send.
    if (LL_SPI_IsActiveFlag_TXE(SPI1) == 0)
    {
        // give TX FIFO a chance to clear before checking again
        LL_mDelay(delay_ms);
        if (!LL_SPI_IsActiveFlag_TXE(SPI1)  == 0) 
        { 
            return false;
        }
        
    }
    return true;
}

// check SPI bus has finished sending the last byte
bool Display::check_bsy_flag_status(uint32_t delay_ms)
{
    // When BSY is set, it indicates that a data transfer is in progress on the SPI
    if (LL_SPI_IsActiveFlag_BSY(SPI1)) 
    {
        // give SPI bus a chance to finish sending data before checking again
        LL_mDelay(delay_ms);
        if (LL_SPI_IsActiveFlag_BSY(SPI1)) 
        { 
            return false;
        }
    }    
    return true; 
}

void Display::dump_buffer(bool hex)
{

    uint16_t byte_count {0};
    for (auto& byte : m_buffer)
    {
        #if defined(USE_RTT)
            // separate the buffer into pages
            if ((byte_count) % m_page_width == 0)
            {
                SEGGER_RTT_printf(0, "\n\n");   
                SEGGER_RTT_printf(0, "Page #%u:\n", ((byte_count) / m_page_width));
            }      
            // seperate the page into lines of 32 bytes
            if ((byte_count) % 32 == 0)
            {
                SEGGER_RTT_printf(0, "\n");   
            }                 
            if (hex)
            {
                SEGGER_RTT_printf(0, "0x%02x ", +byte);
            }
            else 
            {
                SEGGER_RTT_printf(0, "%u ", +byte);
            }
      
        #elif !defined(USE_SSD1306_LL_DRIVER) || !defined(USE_SSD1306_HAL_DRIVER)
            if ((byte_count) % m_page_width == 0)
            {
                std::cout << std::endl << std::endl;
            } 
            if (hex)
            {
                std::cout << "0x" << std::hex << std::setfill ('0') <<  std::setw(2) << +byte << " ";
            }
            else
            {
                std::cout << +byte << " ";
            }
           
        #endif   

        byte_count ++;

    }

}

} // namespace ssd1306
