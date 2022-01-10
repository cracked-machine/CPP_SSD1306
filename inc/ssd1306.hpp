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

#ifndef __SSD1306_HPP_
#define __SSD1306_HPP_

#include <font.hpp>
#include <array>
#include <string>
#include <memory>

#if defined(USE_SSD1306_LL_DRIVER)
    #include <bitset_utils.hpp>
#endif

#if defined(USE_SSD1306_HAL_DRIVER) || defined(USE_SSD1306_LL_DRIVER)
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wvolatile"
		#include "main.h"
		#include "spi.h"	
	#pragma GCC diagnostic pop
#else
	// only used when unit testing on x86
	#include <iostream>
#endif

// DMA1_Channel1_IRQHandler
#include <dma_ch1_interrupt_handler.hpp>


namespace ssd1306
{

// forward declaration 
class DMA1_CH1_InterruptHandler;

// @brief colour range of the OLED(!)
enum class Colour: uint16_t
{
    Black = 0x00, 
    White = 0x01  
};

// @brief SSD1306 error status
enum class ErrorStatus {
	// @brief operation successful
	OK,
	// @brief write overflowed SSD1306 display width/height
	LINE_OVRFLW,
	// @brief provided pixel index is out of bounds
	PIXEL_OOB,
	// @brief cursor falls outside of display area
	CURSOR_OOB,
	// @brief error sending Page position command to SSD1306
	START_PAGE_ERR,
	// @brief error sending low column value command to SSD1306
	START_LCOL_ERR,
	// @brief error sending high column value command to SSD1306
	START_HCOL_ERR,
	// @brief error sending buffer data command to SSD1306
	SEND_DATA_ERR,
	// @brief bad things happened here
	UNKNOWN_ERR
};

// @brief 
class Display
{
public:
	enum class SPIDMA
	{
		disabled,
		enabled
	};
	
	Display(SPI_TypeDef *spi_handle, SPIDMA dma_option);

	// @brief write setup commands to the IC
	bool init();

	// @brief Write single colour to entire sw buffer
	// @param colour 
	void fill(Colour colour);

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


	// @brief Get the display width. Can be used to create a std::array
	// @return constexpr uint16_t 
	static constexpr uint16_t get_display_width() { return m_page_width; }

	// @brief Get the display height. Can be used to create a std::array
	// @return constexpr uint16_t 
	static constexpr uint16_t get_display_height() { return m_height; }



#if defined(X86_UNIT_TESTING_ONLY) || defined(USE_RTT)
	// @brief Debug function to display entire SW bufefr to console (uses RTT on arm, uses std::cout on x86) 
	// @param hex display in hex or decimal values
	void dump_buffer(bool hex);
#endif

private:

    // @brief The CMSIS mem-mapped SPI device
    std::unique_ptr<SPI_TypeDef> _spi_handle;

    // @brief DMA1_CH1_InterruptHandler* interrupt_ptr; 
    std::unique_ptr<DMA1_CH1_InterruptHandler> interrupt_handler; 	

	// @brief Stored setting for enabling/disabling DMA
	SPIDMA spi_dma_setting {SPIDMA::disabled};

	// @brief Write a pixel to the sw buffer at the corresponding display coordinates 
	// @param x pos
	// @param y pos
	// @param colour white/black
	void draw_pixel(uint8_t x, uint8_t y, Colour colour);

	// @brief Write the sw buffer to the IC GDDRAM (Page Addressing Mode only)
	ErrorStatus update_screen();

	// @brief Reset the Driver IC and SW buffer.
	void reset();
	
	// @brief Set the coordinates to draw to the display
	// @param x 
	// @param y 
	bool set_cursor(uint8_t x, uint8_t y);


	// @brief Send one command over SPI 
	// @param cmd_byte The byte to send
	// @return true if success, false if error
	bool send_command(uint8_t cmd_byte);
	
	// @brief send one page of the display buffer over SPI
	// @param page_pos_gddram The index position of the page within the buffer
	// @return true if success, false if error
	bool send_page_data(uint16_t page_pos_gddram);

	// @brief X coordinate for writing to the display
    uint16_t m_currentx {0};

	// @brief Y coordinate for writing to the display
    uint16_t m_currenty {0};

	// @brief The display height, in bytes. Also the number of pages (8) multiplied by the bits per page column (8)
    static const uint16_t m_height {64};
	
#ifdef USE_SSD1306_HAL_DRIVER
	// @brief The HAL SPI object
	SPI_HandleTypeDef m_spi_port {hspi1};
	// @brief The data/command GPIO port object
	GPIO_TypeDef* m_dc_port {SPI1_DC_GPIO_Port};
	// @brief The data/command GPIO pin
	uint16_t m_dc_pin {SPI1_DC_Pin};
	// @brief The reset GPIO port object
	GPIO_TypeDef* m_reset_port {SPI1_RESET_GPIO_Port};
	// @brief The reset GPIO pin
	uint16_t m_reset_pin {SPI1_RESET_Pin};
#elif defined(USE_SSD1306_LL_DRIVER)
	// @brief The reset GPIO port object
	GPIO_TypeDef *m_reset_port 	{SPI1_RESET_GPIO_Port};
	// @brief The reset GPIO pin
	uint32_t m_reset_pin		{SPI1_RESET_Pin};
	// @brief The data/command GPIO port object	
	GPIO_TypeDef *m_dc_port 	{SPI1_DC_GPIO_Port};
	// @brief The data/command GPIO pin	
	uint32_t m_dc_pin			{SPI1_DC_Pin};
#else
	// x86; define nothing
#endif

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

		// @brief Display off (sleep mode)
		display_mode_sleep =	0xAE,

		// @brief Display on in normal mode
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
		// @brief Set Display Clock Divide Ratio/Oscillator Frequency. [7:4] clk freq, [3:0] clk prescaler
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


protected:

	// @brief The display width in bytes. Also the size of each GDDRAM page
    static const uint16_t m_page_width {128};

	// @brief byte buffer for ssd1306. Access to derived classes like ssd1306_tester is permitted.
    std::array<uint8_t, (m_page_width*m_height)/8> m_buffer;

	// @brief 
	// @tparam FONT_SIZE 
	// @param ss 
	// @param font 
	// @param colour 
	// @param padding 
	// @return ErrorStatus 
	template<std::size_t FONT_SIZE> 
	ErrorStatus write_string(std::string &msg, Font<FONT_SIZE> &font, Colour colour, bool padding);

	// @brief 
	// @tparam FONT_SIZE 
	// @param ch 
	// @param font 
	// @param colour 
	// @param padding 
	// @return ErrorStatus 
	template<std::size_t FONT_SIZE> 
	ErrorStatus write_char(char ch, Font<FONT_SIZE> &font, Colour colour, bool padding);


};

// Out-of-class definitions of member function templates 

template<std::size_t FONT_SIZE>
ErrorStatus Display::write(std::string &msg, Font<FONT_SIZE> &font, uint8_t x, uint8_t y, Colour bg, Colour fg, bool padding, bool update)
{
    fill(bg);
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

template<std::size_t FONT_SIZE>
ErrorStatus Display::write_string(std::string &ss, Font<FONT_SIZE> &font, Colour color, bool padding)
{
    // Write until null-byte
	for (char &c : ss)
	{
		ErrorStatus res = write_char(c, font, color, padding);
		if (res != ErrorStatus::OK)
		{
			return res;
		}
	}
    return ErrorStatus::OK;
}

template<std::size_t FONT_SIZE>
ErrorStatus Display::write_char(char ch, Font<FONT_SIZE> &font, Colour colour, bool padding)
{

    // Check remaining space on current line
    if (m_page_width <= (m_currentx + font.height()) ||
        m_page_width <= (m_currenty + font.height()))
    {
        // Not enough space on current line
        return ErrorStatus::OK;
    }

    // add extra leading horizontal space
    if (padding)
    {
    	for(size_t n = 0; n < font.height(); n++)
		{
			draw_pixel(m_currentx, (m_currenty + n), Colour::Black);
		}
    	m_currentx += 1;
    }

    // Use the font to write
    uint32_t font_data_word;
    for(size_t font_height_idx = 0; font_height_idx < font.height(); font_height_idx++) 
	{
        if (!font.get_pixel( (ch - 32) * font.height() + font_height_idx, font_data_word )) { return ErrorStatus::PIXEL_OOB; }

		#ifdef ENABLE_SSD1306_TEST_STDOUT
				// separator for the font
				std::cout << std::endl;
		#endif

		// get each bit/pixel in the font_data_word and write it to the display output buffer
        for(size_t font_width_idx = 0; font_width_idx < font.width(); font_width_idx++) 
		{
			// shift left and check if MSB is set: write as foreground pixel
            if((font_data_word << font_width_idx) & 0x8000)
            {
            	switch (colour)
				{
					case Colour::White:
						draw_pixel(m_currentx + font_width_idx, m_currenty + font_height_idx, Colour::White);
						break;
					
					case Colour::Black:
						draw_pixel(m_currentx + font_width_idx, m_currenty + font_height_idx, Colour::Black);
						break;
				}			
            }
			// MSB is not set: write as background pixel
            else
            {
            	switch (colour)
				{
					case Colour::White:
						draw_pixel(m_currentx + font_width_idx, m_currenty + font_height_idx, Colour::Black);
						break;
					
					case Colour::Black:
						draw_pixel(m_currentx + font_width_idx, m_currenty + font_height_idx, Colour::White);
						break;
				}						
            }
        }
    }

    // The current space is now taken
    m_currentx += font.width();

    // add extra leading horizontal space
    if (padding)
	{
    	m_currentx += 1;
	}

    // Return written char for validation
    return ErrorStatus::OK;
}



} // namespace ssd1306

#endif /* __SSD1306_HPP_ */
