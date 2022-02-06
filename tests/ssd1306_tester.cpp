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

#include <ssd1306_tester.hpp>
#include <catch2/catch_all.hpp>
#include <array>
#include <iomanip>
#include <numeric>

namespace ssd1306
{

// ssd1306_tester::ssd1306_tester()
// {
//     REQUIRE(power_on_sequence());
// }

// bool ssd1306_tester::validate_buffer(std::vector<uint8_t> &validation_buffer)
// {
//     if (validation_buffer.size() != m_buffer.size())
//     {
//         //std::cout << "Validation buffer error - expected size: " << m_buffer.size() 
//         //    << ", actual size: " << validation_buffer.size() << std::endl;
//         return false;
//     }

//     static ssd1306::FontTest font_under_test;
//     // set the font character
//     std::string msg{font_under_test.character_map[0]};

//     // write the font to the buffer
//     write(msg, font_under_test, 0, 0, ssd1306::Colour::Black, ssd1306::Colour::White, true, true);

//     //get_buffer(m_buffer);

//     auto valid_buffer_iter = validation_buffer.begin();
//     auto valid_buffer_end = validation_buffer.end();

//     for (auto& byte : m_buffer)
//     {
//         if (byte != *valid_buffer_iter)
//         {
//             return false;
//         }
//         if (valid_buffer_iter != valid_buffer_end)
//         {
//             valid_buffer_iter++;
//         }
//     }
//     return true;
// }

// bool ssd1306_tester::get_buffer_page(uint8_t page_number, std::vector<uint16_t> &page)
// {
 
//     page.resize(0);
//     const uint16_t page_start_idx = page_number * m_page_width;
//     for (uint16_t idx = page_start_idx; idx < page_start_idx + m_page_width; idx++)
//     {
//         page.push_back(m_buffer[idx]);
//     }
//     return true;
  
// }

// bool ssd1306_tester::dump_buffer_as_hex()
// {
// #ifdef ENABLE_SSD1306_TEST_STDOUT    
//     //get_buffer(m_buffer);
//     uint8_t row_count {0};
//     uint8_t col_count {0};

//     std::cout << +row_count << ":\t";
//     for (auto _byte : m_buffer)
//     {
//         std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << +_byte << ", " << std::flush;

//         if (col_count >= 15)
//         {
//             col_count = 0; 
//             row_count ++;
            
//             std::cout << std::endl << std::dec << (row_count * 16) << ":\t" << std::flush;             
//         }
//         else 
//         {
//             col_count++;
//         }
//     }
// #endif
//     return true;
// }

} // namespace ssd1306