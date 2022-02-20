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

#include <ssd1306_common.hpp>

namespace ssd1306
{

void CommonFunctions::fill(Colour colour)
{
    for(auto &pixel : m_buffer)
    {
        pixel = (colour == Colour::Black) ? 0x00 : 0xFF;
    }
}

void CommonFunctions::draw_pixel(uint8_t x, uint8_t y, Colour colour)
{
    // Draw in the right color
    if(colour == Colour::White) 
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

bool CommonFunctions::set_cursor(uint8_t x, uint8_t y)
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




} // namespace ssd1306