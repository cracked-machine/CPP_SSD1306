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
#include <cstring>

#include <ll_spi_utils.hpp>

namespace ssd1306
{

bool Display::init()
{
    #if defined(USE_SSD1306_LL_DRIVER)
        LL_SPI_Enable(m_spi_port);
        if (!LL_SPI_IsEnabled(m_spi_port))
        {
            return false;
        }
    #endif

	reset();

    // Now wait for the screen to boot
    #if defined(USE_SSD1306_HAL_DRIVER)
        HAL_Delay(100);
    #elif defined(USE_SSD1306_LL_DRIVER)
        LL_mDelay(10);
    #endif

    // put into sleep mode during setup, probably not needed
    if (!send_command( static_cast<uint8_t>(fcmd::display_mode_sleep) )) { return false; } 

    // set fundamental commands
    if (!send_command( static_cast<uint8_t>(fcmd::display_use_ram) )) { return false; }
    if (!send_command( static_cast<uint8_t>(fcmd::display_inverse_off) )) { return false; } 
    if (!send_command( static_cast<uint8_t>(fcmd::set_display_constrast) )) { return false; } 
    if (!send_command( static_cast<uint8_t>(fcmd::max_constrast) )) { return false; } 
    
    // set addressing setting commands
    if (!send_command( static_cast<uint8_t>(acmd::set_memory_mode) )) { return false; } 
    if (!send_command( static_cast<uint8_t>(acmd::page_addr_mode) )) { return false; }
    if (!send_command( static_cast<uint8_t>(acmd::start_page_0) )) { return false; } 
    if (!send_command( static_cast<uint8_t>(acmd::start_lcol_0) )) { return false; } 
    if (!send_command( static_cast<uint8_t>(acmd::start_hcol_0) )) { return false; } 

    // set hardware config commands
    if (!send_command( static_cast<uint8_t>(hwcmd::start_line_0) )) { return false; } 
    if (!send_command( static_cast<uint8_t>(hwcmd::vert_flip_normal) )) { return false; } 
    if (!send_command( static_cast<uint8_t>(hwcmd::horiz_flip_normal) )) { return false; }     
    if (!send_command( static_cast<uint8_t>(hwcmd::set_mux_ratio) )) { return false; } 
    if (!send_command( static_cast<uint8_t>(hwcmd::default_mux_ratio) )) { return false; } 
    if (!send_command( static_cast<uint8_t>(hwcmd::set_vert_offset) )) { return false; } 
    if (!send_command( static_cast<uint8_t>(hwcmd::vert_offset_none) )) { return false; } 
    if (!send_command( static_cast<uint8_t>(hwcmd::set_com_pin_cfg) )) { return false; } 
    if (!send_command( static_cast<uint8_t>(hwcmd::com_pin_alt_remap_off) )) { return false; }
    
    // set timing/driving commands
    if (!send_command( static_cast<uint8_t>(tcmd::clk_presc_freq) )) { return false; } 
    if (!send_command( static_cast<uint8_t>(tcmd::clk_max_setting) )) { return false; } 
    if (!send_command( static_cast<uint8_t>(tcmd::set_precharge_period) )) { return false; } 
    if (!send_command( static_cast<uint8_t>(tcmd::default_precharge) )) { return false; } 
    if (!send_command( static_cast<uint8_t>(tcmd::set_vcomh_lvl) )) { return false; } 
    if (!send_command( static_cast<uint8_t>(tcmd::vcomh_vcc_077) )) { return false; } 

    // wake up display
    if (!send_command( static_cast<uint8_t>(fcmd::display_mode_normal) )) { return false; } 

    // Clear screen
    fill(Colour::White);

    // Flush buffer to screen
    if (!update_screen())
    {
        return false;
    }

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
        if (!send_command( static_cast<uint8_t>(acmd::start_page_0) + page_idx)) { return false; }

        // Set the lower start column address of pointer by command 00h~0Fh.
        if (!send_command( static_cast<uint8_t>(acmd::start_lcol_0) )) { return false; }

        // Set the upper start column address of pointer by command 10h~1Fh
        if (!send_command( static_cast<uint8_t>(acmd::start_hcol_0) )) { return false; }

        // the next page position within the GDDRAM buffer
        uint16_t page_pos_gddram {static_cast<uint16_t>( m_page_width * page_idx )};

        if (!send_page_data(page_pos_gddram)) { return false; }

        //dump_buffer(true);
    }
        
    return true;
}

bool Display::send_command(uint8_t cmd_byte)
{
    // #if defined(USE_RTT)
    //     SEGGER_RTT_printf(0, "\nCommand Byte: 0x%02x", +cmd_byte);
    // #endif  

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
        if (!embed_utils::spi::ll_wait_for_txe_flag(m_spi_port))
        {
            #if defined(USE_RTT) 
                SEGGER_RTT_printf(0, "\nwrite_command(): Tx buffer is full"); 
            #endif
        }
        if (!embed_utils::spi::ll_wait_for_bsy_flag(m_spi_port))
        {
            #if defined(USE_RTT) 
                SEGGER_RTT_printf(0, "\nwrite_command(); SPI bus is busy"); 
            #endif
        }  
        // set cmd mode/low signal after we put data into TXFIFO to avoid premature latching
        LL_GPIO_ResetOutputPin(m_dc_port, m_dc_pin);      
        LL_SPI_TransmitData8(m_spi_port, cmd_byte);    
        
        return true;
    #else
        UNUSED (cmd_byte);
        return true;
    #endif
    
}

bool Display::send_page_data(uint16_t page_pos_gddram)
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
            if (!embed_utils::spi::ll_wait_for_txe_flag(m_spi_port))
            {
                #if defined(USE_RTT) 
                    SEGGER_RTT_printf(0, "\nsend_page_data(): Tx buffer is full."); 
                #endif
            }
            if (!embed_utils::spi::ll_wait_for_bsy_flag(m_spi_port))
            {
                #if defined(USE_RTT) 
                    SEGGER_RTT_printf(0, "\nsend_page_data(): SPI bus is busy."); 
                #endif
            }                          
            LL_SPI_TransmitData8(m_spi_port, m_buffer[idx]);
            // set data mode/high signal after we put data into TXFIFO to avoid premature latching
            LL_GPIO_SetOutputPin(m_dc_port, m_dc_pin);         
        }
        return true;
    #elif defined(X86_UNIT_TESTING_ONLY)
        UNUSED(page_pos_gddram);
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
        LL_GPIO_ResetOutputPin(m_reset_port, m_reset_pin);
        LL_mDelay(10);
        LL_GPIO_SetOutputPin(m_reset_port, m_reset_pin);
        LL_mDelay(10);
    #endif

    // reset the sw buffer 
    m_buffer.fill(0);
}

#if defined(X86_UNIT_TESTING_ONLY) || defined(USE_RTT)
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
            #elif defined(X86_UNIT_TESTING_ONLY)
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
#endif

} // namespace ssd1306
