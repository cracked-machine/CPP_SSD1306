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

Driver::Driver(DriverSerialInterface display_spi_interface, SPIDMA dma_option) : spi_dma_setting (dma_option), m_serial_interface(display_spi_interface)
{
}

bool Driver::power_on_sequence()
{

    if (spi_dma_setting == SPIDMA::enabled)
    {
        // register the interrupt with STM32G0InterruptManager
        m_dma_int_handler.register_driver(this);
    }


    LL_SPI_Enable(m_serial_interface.get_spi_handle());
    if (!LL_SPI_IsEnabled(m_serial_interface.get_spi_handle()))
    {
        return false;
    }


	reset();

    // Now wait for the screen to boot
    LL_mDelay(10);


    // put into sleep mode during setup, probably not needed
    if (!send_command( static_cast<uint8_t>(fcmd::display_mode_sleep) )) { return false; } 

    // set fundamental commands
    if (!send_command( static_cast<uint8_t>(fcmd::display_use_ram) )) { return false; }
    if (!send_command( static_cast<uint8_t>(fcmd::display_inverse_off) )) { return false; } 
    if (!send_command( static_cast<uint8_t>(fcmd::set_display_constrast) )) { return false; } 
    if (!send_command( static_cast<uint8_t>(fcmd::max_constrast) )) { return false; } 
    
    
    if (spi_dma_setting == SPIDMA::enabled)
    {
        // set page addressing mode
        if (!send_command( static_cast<uint8_t>(acmd::set_memory_mode) )) { return false; } 
        if (!send_command( static_cast<uint8_t>(acmd::horiz_addr_mode) )) { return false; }
        
        if (!send_command( static_cast<uint8_t>(acmd::set_column_address) )) { return false; }
        if (!send_command( static_cast<uint8_t>(0x00) )) { return false; } 
        if (!send_command( static_cast<uint8_t>(0x7F) )) { return false; } 

        if (!send_command( static_cast<uint8_t>(acmd::set_page_address) )) { return false; }
        if (!send_command( static_cast<uint8_t>(0x00) )) { return false; } 
        if (!send_command( static_cast<uint8_t>(0x07) )) { return false; } 
    }
    else
    {
        // set horizontal addressing mode - can be used with DMA because commands arenot required before every buffer update
        if (!send_command( static_cast<uint8_t>(acmd::set_memory_mode) )) { return false; } 
        if (!send_command( static_cast<uint8_t>(acmd::page_addr_mode) )) { return false; }
        if (!send_command( static_cast<uint8_t>(acmd::start_page_0) )) { return false; } 
        if (!send_command( static_cast<uint8_t>(acmd::start_lcol_0) )) { return false; } 
        if (!send_command( static_cast<uint8_t>(acmd::start_hcol_0) )) { return false; } 
    }

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
    fill(Colour::Black);

    if (spi_dma_setting == SPIDMA::enabled)
    {
        // no more commands to send so set data mode/high signal
        LL_GPIO_SetOutputPin(m_serial_interface.get_dc_port(), m_serial_interface.get_dc_pin());

        LL_SPI_Disable(m_serial_interface.get_spi_handle());
        LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, (uint32_t)m_buffer.size());
        LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_1, (uint32_t)m_buffer.data());
        LL_DMA_SetPeriphAddress(DMA1, LL_DMA_CHANNEL_1, (uint32_t)&SPI1->DR);
        LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_1);
        LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
        // generate DMA request whenever the TX buffer is empty
        LL_SPI_EnableDMAReq_TX(m_serial_interface.get_spi_handle());
        LL_SPI_Enable(m_serial_interface.get_spi_handle());
    
    }

    // Flush buffer to screen
    ErrorStatus res = update_screen();
    if (res != ErrorStatus::OK)
    {
        return false;
    }

    // Set default values for screen object
    m_currentx = 0;
    m_currenty = 0;

    return true;
}


void Driver::fill(Colour color)
{
    for(auto &pixel : m_buffer)
    {
        pixel = (color == Colour::Black) ? 0x00 : 0xFF;
    }
}

void Driver::dma1_ch2_isr()
{
    // prevent ISR lockup
    LL_DMA_ClearFlag_HT1(DMA1);
    LL_DMA_ClearFlag_TC1(DMA1);
}

ErrorStatus Driver::update_screen()
{
    // DMA doesn't require explicitly send of commands or data
    if (spi_dma_setting == SPIDMA::disabled)
    {    
        for(uint8_t page_idx = 0; page_idx < 8; page_idx++)
        {
 
            // Set Page position to write to: 0-7
            if (!send_command( static_cast<uint8_t>(acmd::start_page_0) + page_idx)) { return ErrorStatus::START_PAGE_ERR; }

            // Set the lower start column address of pointer by command 00h~0Fh.
            if (!send_command( static_cast<uint8_t>(acmd::start_lcol_0) )) { return ErrorStatus::START_LCOL_ERR; }

            // Set the upper start column address of pointer by command 10h~1Fh
            if (!send_command( static_cast<uint8_t>(acmd::start_hcol_0) )) { return ErrorStatus::START_HCOL_ERR; }

            // the next page position within the GDDRAM buffer
            uint16_t page_pos_gddram {static_cast<uint16_t>( m_page_width * page_idx )};

            if (!send_page_data(page_pos_gddram)) { return ErrorStatus::SEND_DATA_ERR; }            
        }

        //dump_buffer(true);
    }
        
    return ErrorStatus::OK;
}

bool Driver::send_command(uint8_t cmd_byte [[maybe_unused]])
{
    // #if defined(USE_RTT)
    //     SEGGER_RTT_printf(0, "\nCommand Byte: 0x%02x", +cmd_byte);
    // #endif  

    if (!stm32::spi::ll_wait_for_txe_flag(m_serial_interface.get_spi_handle()))
    {
        #if defined(USE_RTT) 
            SEGGER_RTT_printf(0, "\nwrite_command(): Tx buffer is full"); 
        #endif
        
    }
    if (!stm32::spi::ll_wait_for_bsy_flag(m_serial_interface.get_spi_handle()))
    {
        #if defined(USE_RTT) 
            SEGGER_RTT_printf(0, "\nwrite_command(); SPI bus is busy"); 
        #endif
        
    }  
    // set cmd mode/low signal after we put data into TXFIFO to avoid premature latching
    LL_GPIO_ResetOutputPin(m_serial_interface.get_dc_port(), m_serial_interface.get_dc_pin());      
    LL_SPI_TransmitData8(m_serial_interface.get_spi_handle(), cmd_byte);    
    
    return true;
}

bool Driver::send_page_data(uint16_t page_pos_gddram [[maybe_unused]])
{
    #if defined(X86_UNIT_TESTING_ONLY)
        return true;
    #else
        // transmit bytes from this page (page_pos_gddram -> page_pos_gddram + m_page_width)
        for (uint16_t idx = page_pos_gddram; idx < page_pos_gddram + m_page_width; idx++)
        {
            if (!stm32::spi::ll_wait_for_txe_flag(m_serial_interface.get_spi_handle()))
            {
                #if defined(USE_RTT) 
                    SEGGER_RTT_printf(0, "\nsend_page_data(): Tx buffer is full."); 
                #endif
                
            }
            if (!stm32::spi::ll_wait_for_bsy_flag(m_serial_interface.get_spi_handle()))
            {
                #if defined(USE_RTT) 
                    SEGGER_RTT_printf(0, "\nsend_page_data(): SPI bus is busy."); 
                #endif
                
            }                          
            LL_SPI_TransmitData8(m_serial_interface.get_spi_handle(), m_buffer[idx]);
            // set data mode/high signal after we put data into TXFIFO to avoid premature latching
            LL_GPIO_SetOutputPin(m_serial_interface.get_dc_port(), m_serial_interface.get_dc_pin());         
        }
        return true;
    #endif  // defined(USE_SSD1306_HAL_DRIVER)
} 

void Driver::draw_pixel(uint8_t x, uint8_t y, Colour color)
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
}

bool Driver::set_cursor(uint8_t x, uint8_t y)
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


void Driver::reset()
{
	// Signal the driver IC to reset the OLED display
    LL_GPIO_ResetOutputPin(m_serial_interface.get_reset_port(), m_serial_interface.get_reset_pin());
    LL_mDelay(10);
    LL_GPIO_SetOutputPin(m_serial_interface.get_reset_port(), m_serial_interface.get_reset_pin());
    LL_mDelay(10);

    // reset the sw buffer 
    m_buffer.fill(0);
}

#if defined(X86_UNIT_TESTING_ONLY) || defined(USE_RTT)
void Driver::dump_buffer(bool hex)
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
