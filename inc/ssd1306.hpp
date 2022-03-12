// MIT License

// Copyright (c) 2022 Chris Sutton

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

#ifndef __SSD1306_HPP_
#define __SSD1306_HPP_

#include <ssd1306_device.hpp>
#include <cstring>

namespace ssd1306
{


// @brief 
template<typename DEVICE_ISR_ENUM>
class Driver : public RestrictedBase, public CommonFunctions
{
public:
	enum class SPIDMA
	{
		disabled,
		enabled
	};
	
	// @brief Stored setting for enabling/disabling DMA
	SPIDMA spi_dma_setting {SPIDMA::disabled};

	
	Driver(DriverSerialInterface<DEVICE_ISR_ENUM> &display_spi_interface, SPIDMA dma_option) 
	: spi_dma_setting (dma_option), m_serial_interface(display_spi_interface)
	{
	}

	// @brief write setup commands to the IC
	bool power_on_sequence()
	{

		if (spi_dma_setting == SPIDMA::enabled)
		{
			// register the interrupt with InterruptManagerStm32g0
			m_dma_int_handler.register_driver(this);
		}

		#if not defined(X86_UNIT_TESTING_ONLY)
			stm32::spi::enable_spi(m_serial_interface.get_spi_handle());

			reset();

			// Now wait for the screen to boot
			LL_mDelay(10);
		#endif

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
		#if not defined(X86_UNIT_TESTING_ONLY)
				// no more commands to send so set data mode/high signal
				LL_GPIO_SetOutputPin(m_serial_interface.get_dc_port(), m_serial_interface.get_dc_pin());

				// setup the SPI DMA
				stm32::spi::enable_spi(m_serial_interface.get_spi_handle(), false);
				
				LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, (uint32_t)m_buffer.size());
				LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_1, (uint32_t)m_buffer.data());
				LL_DMA_SetPeriphAddress(DMA1, LL_DMA_CHANNEL_1, (uint32_t)&SPI1->DR);
				LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_1);
				LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);

				m_serial_interface.get_spi_handle()->CR2 = m_serial_interface.get_spi_handle()->CR2 | SPI_CR2_TXDMAEN;
				stm32::spi::enable_spi(m_serial_interface.get_spi_handle());
		#endif
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
	// @return ErrorStatus 
	template<std::size_t FONT_SIZE> 
	ErrorStatus write(std::string &msg, Font<FONT_SIZE> &font, uint8_t x, uint8_t y, Colour bg, Colour fg, bool padding, bool update);



	// @brief callback function for InterruptManagerStm32g0 
	// see stm32_interrupt_managers/inc/stm32g0_interrupt_manager_functional.hpp
	void dma_isr()
	{
		#if not defined(X86_UNIT_TESTING_ONLY)
			// prevent ISR lockup
			LL_DMA_ClearFlag_HT1(DMA1);
			LL_DMA_ClearFlag_TC1(DMA1);
		#endif
	}	

#if defined(X86_UNIT_TESTING_ONLY) || defined(USE_RTT)
	// @brief Debug function to display entire SW bufefr to console (uses RTT on arm, uses std::cout on x86) 
	// @param hex display in hex or decimal values
	void dump_buffer(bool hex);
#endif

private:


	// object containing SPI port/pins and pointer to CMSIS defined SPI peripheral
	DriverSerialInterface<DEVICE_ISR_ENUM> m_serial_interface;

	// @brief SSD1306 Fundamental Commands - See Section 9 of datasheet for setting bytes
	enum class fcmd
	{
		// @brief Double byte command to select 1 out of 256 contrast steps. 
		// Contrast increases as the value increases. 
		set_display_constrast = 	0x81,
		// @brief Maximum display contrast value
		max_constrast =			0xFF,
		// @brief Minimum display contrast value
		min_constrast = 		0x01,

		// @brief Output follows RAM content
		display_use_ram = 		0xA4,

		// @brief Output ignores RAM content
		display_ignore_ram = 	0xA5,

		// @brief Normal display - 0 in RAM: OFF in display panel, 1 in RAM: ON in display panel
		display_inverse_off = 	0xA6,

		// @brief Inverse display - 1 in RAM: OFF in display panel, 0 in RAM: ON in display panel
		display_inverse_on = 	0xA7,	

		// @brief Driver off (sleep mode)
		display_mode_sleep =	0xAE,

		// @brief Driver on in normal mode
		display_mode_normal =  	0xAF
		
	};

	// @brief SSD1306 Scrolling Commands - See Section 9 of datasheet for setting bytes
	enum class scmd
	{
		// @brief Right Horizontal Scroll
		horiz_scroll_right = 	0x26,	
		// @brief Left Horizontal Scroll
		horiz_scroll_left = 	0x27,
		// @brief Vertical and right Horizontal Scroll
		both_scroll_right = 	0x29,
		// @brief Vertical and left Horizontal Scroll
		both_scroll_left = 		0x2A,
		// @brief Start scrolling
		activate_scroll = 		0x2F,
		// @brief Stop scrolling
		deactivate_scroll = 	0x2E,
		// @brief Set Vertical scrolling area
		vert_scroll_area = 		0xA3
	};

	// @brief SSD1306 Address Commands
	enum class acmd
	{	
		
		set_memory_mode = 		0x20,		// @brief Set the addressing mode - use one of the below modes
		horiz_addr_mode = 		0x00,		// @brief Horizontal Addressing Mode
		vert_addr_mode = 		0x01,		// @brief Vertical Addressing Mode
		page_addr_mode = 		0x02,		// @brief Page Addressing Mode

		set_column_address = 	0x21,		// Setup column start and end address. Horizontal or vertical addressing mode only.
		set_page_address = 		0x22,		// Setup page start and end address. Horizontal or vertical addressing mode only.

		// @brief Set Page Start Address for Page Addressing Mode
		start_page_0 =			0xB0,	// @brief Start at Page #0	
		start_page_1 = 			0xB1,	// @brief Start at Page #1	
		start_page_2 = 			0xB2, 	// @brief Start at Page #2	
		start_page_3 = 			0xB3,	// @brief Start at Page #3
		start_page_4 = 			0xB4,	// @brief Start at Page #4
		start_page_5 = 			0xB5,	// @brief Start at Page #5
		start_page_6 = 			0xB6,	// @brief Start at Page #6
		start_page_7 = 			0xB7,	// @brief Start at Page #7
		
		// @brief Set the lower nibble of the column start address register for Page Addressing Mode
		start_lcol_0 = 			0x00,	// lowest address
		start_lcol_15 = 		0x0F,	// highest address

		// @brief Set the higher nibble of the column start address register for Page Addressing Mode
		start_hcol_0 = 			0x10,	// lowest address
		start_hcol_15 =			0x1F	// highest address

	};	

	// @brief SSD1306 Hardware Configuration (Panel resolution & layout related) Commands
	enum class hwcmd
	{
		// @brief Set display RAM start line register from 0. Reset to this value during RESET.
		start_line_0 = 			0x40,
		// @brief Set display RAM start line register from 63.
		start_line_31 =			0x7F,

		// @brief Set segment remap: column address 0 is mapped to SEG0 (RESET)
		horiz_flip_inverse = 	0xA0,
		// @brief Set segment remap: column address 127 is mapped to SEG0 (RESET)		
		horiz_flip_normal =		0xA1,

		// @brief Set COM output scan direction: remapped mode. Scan from COM[N-1] to COM0 (where N is multiplex ratio).
		vert_flip_inverse = 	0xC0,
		// @brief Set COM output scan direction: normal mode. Scan from COM0 to COM[N-1] (where N is multiplex ratio).
		vert_flip_normal = 		0xC8,

		// @brief Set Multiplex Ratio with the next byte. Valid ranges: 16d -> 63d (0x10 -> 0x3F)
		set_mux_ratio = 		0xA8,
		default_mux_ratio = 	0x3F,

		// @brief Set vertical COM shift. Valid ranges: 0d -> 63d (0x00 -> 0x3F).
		set_vert_offset = 		0xD3,
		vert_offset_none = 		0x00,	

		// @brief Set COM Pins Hardware Configuration - See section 10.1.18 of datasheet.
		set_com_pin_cfg	=		0xDA,
		// @brief Sequential COM config + Enable COM Left/Right remap
		com_pin_seq_remap_on =	0x22,
		// @brief Sequential COM config + Disable COM Left/Right remap
		com_pin_seq_remap_off = 0x02,
		// @brief Alternative COM config + Enable COM Left/Right remap
		com_pin_alt_remap_on =	0x32,
		// @brief Alternative COM config + Disable COM Left/Right remap
		com_pin_alt_remap_off = 0x12,

	};

	// @brief SSD1306 Timing & Driving Scheme Setting Commands
	enum class tcmd
	{
		// @brief Set Driver Clock Divide Ratio/Oscillator Frequency. [7:4] clk freq, [3:0] clk prescaler
		clk_presc_freq = 		0xD5,
		// @brief Max frequency, no prescaler
		clk_max_setting = 		0xF0,	

		// @brief Set the duration of the pre-charge period. 
		set_precharge_period = 	0xD9,
		default_precharge = 	0x22,

		// @brief Set the VCOMH regulator output
		set_vcomh_lvl =			0xDB,
		// @brief ~ 0.65 x VCC
		vcomh_vcc_065 = 		0x00,
		// @brief ~ 0.77 x VCC (RESET)
		vcomh_vcc_077 = 		0x20,
		// @brief ~ 0.83 x VCC
		vcomh_vcc_083 = 		0x30,

	};

	// @brief callback handler for DMA interrupts
	struct DmaIntHandler : public stm32::isr::InterruptManagerStm32Base<DEVICE_ISR_ENUM>
	{
		// @brief the parent driver class
		Driver *m_parent_driver_ptr;
		// @brief initialise and register this handler instance with InterruptManagerStm32g0
		// @param parent_driver_ptr the instance to register
		void register_driver(Driver *parent_driver_ptr)
		{
			m_parent_driver_ptr = parent_driver_ptr;
			stm32::isr::InterruptManagerStm32Base<DEVICE_ISR_ENUM>::register_handler(m_parent_driver_ptr->m_serial_interface.get_dma_isr_type(), this);
		}
        // @brief Definition of InterruptManagerStm32Base::ISR. This is called by stm32::isr::InterruptManagerStm32Base<DEVICE_ISR_ENUM> specialization 
		virtual void ISR()
		{
			m_parent_driver_ptr->dma_isr();
		}
	};
	// @brief handler object
	DmaIntHandler m_dma_int_handler;


	// @brief Reset the Driver IC and SW buffer.
	void reset()
	{
	#if not defined(X86_UNIT_TESTING_ONLY)
		// Signal the driver IC to reset the OLED display
		LL_GPIO_ResetOutputPin(m_serial_interface.get_reset_port(), m_serial_interface.get_reset_pin());
		LL_mDelay(10);
		LL_GPIO_SetOutputPin(m_serial_interface.get_reset_port(), m_serial_interface.get_reset_pin());
		LL_mDelay(10);
	#endif
		// reset the sw buffer 
		m_buffer.fill(0);
	}


	// @brief Write the sw buffer to the IC GDDRAM (Page Addressing Mode only)
	ErrorStatus update_screen()
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



	// @brief Send one command over SPI 
	// @param cmd_byte The byte to send
	// @return true if success, false if error
	bool send_command(uint8_t cmd_byte)
	{
		// #if defined(USE_RTT)
		//     SEGGER_RTT_printf(0, "\nCommand Byte: 0x%02x", +cmd_byte);
		// #endif  

		if (!stm32::spi::wait_for_txe_flag(m_serial_interface.get_spi_handle()))
		{
			#if defined(USE_RTT) 
				SEGGER_RTT_printf(0, "\nwrite_command(): Tx buffer is full"); 
			#endif
			
		}
		if (!stm32::spi::wait_for_bsy_flag(m_serial_interface.get_spi_handle()))
		{
			#if defined(USE_RTT) 
				SEGGER_RTT_printf(0, "\nwrite_command(); SPI bus is busy"); 
			#endif
			
		}  
		#if not defined(X86_UNIT_TESTING_ONLY)
			// set cmd mode/low signal after we put data into TXFIFO to avoid premature latching
			LL_GPIO_ResetOutputPin(m_serial_interface.get_dc_port(), m_serial_interface.get_dc_pin()); 

			// send the command over SPI bus
			stm32::spi::send_byte(m_serial_interface.get_spi_handle(), cmd_byte);
		#endif    
		return true;
	}
	
	// @brief send one page of the display buffer over SPI
	// @param page_pos_gddram The index position of the page within the buffer
	// @return true if success, false if error
	bool send_page_data(uint16_t page_pos_gddram)
	{
		#if defined(X86_UNIT_TESTING_ONLY)
			return true;
		#else
			// transmit bytes from this page (page_pos_gddram -> page_pos_gddram + m_page_width)
			for (uint16_t idx = page_pos_gddram; idx < page_pos_gddram + m_page_width; idx++)
			{
				if (!stm32::spi::wait_for_txe_flag(m_serial_interface.get_spi_handle()))
				{
					#if defined(USE_RTT) 
						SEGGER_RTT_printf(0, "\nsend_page_data(): Tx buffer is full."); 
					#endif
					
				}
				if (!stm32::spi::wait_for_bsy_flag(m_serial_interface.get_spi_handle()))
				{
					#if defined(USE_RTT) 
						SEGGER_RTT_printf(0, "\nsend_page_data(): SPI bus is busy."); 
					#endif
					
				}                          
				// send the page over SPI bus
				stm32::spi::send_byte(m_serial_interface.get_spi_handle(), m_buffer[idx]);
				
				// set data mode/high signal after we put data into TXFIFO to avoid premature latching
				LL_GPIO_SetOutputPin(m_serial_interface.get_dc_port(), m_serial_interface.get_dc_pin());         
			}
			return true;
		#endif  // defined(USE_SSD1306_HAL_DRIVER)
	} 

};

// Out-of-class definitions of member function templates 
template<typename DEVICE_ISR_ENUM>
template<std::size_t FONT_SIZE>
ErrorStatus Driver<DEVICE_ISR_ENUM>::write(std::string &msg, Font<FONT_SIZE> &font, uint8_t x, uint8_t y, [[maybe_unused]] Colour bg, Colour fg, bool padding, bool update)
{
    // invalid cursor position requested
	if (!set_cursor(x, y))
	{
		return ErrorStatus::CURSOR_OOB;
	}

	ErrorStatus write_res = write_string(msg, font, fg, padding);
	if (write_res != ErrorStatus::OK)
	{
		return write_res;
	}
    
	// if update was selected then we can just return the result now
	if (update)
    {
		return update_screen();
    }
    
	return ErrorStatus::OK;
}





} // namespace ssd1306

#endif /* __SSD1306_HPP_ */
