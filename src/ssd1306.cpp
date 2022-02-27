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


namespace ssd1306
{



// #if defined(X86_UNIT_TESTING_ONLY) || defined(USE_RTT)
// void Driver::dump_buffer(bool hex)
// {
//     uint16_t byte_count {0};
//     for (auto& byte : m_buffer)
//     {
//         #if defined(USE_RTT)
//             // separate the buffer into pages
//             if ((byte_count) % m_page_width == 0)
//             {
//                 SEGGER_RTT_printf(0, "\n\n");   
//                 SEGGER_RTT_printf(0, "Page #%u:\n", ((byte_count) / m_page_width));
//             }      
//             // seperate the page into lines of 32 bytes
//             if ((byte_count) % 32 == 0)
//             {
//                 SEGGER_RTT_printf(0, "\n");   
//             }                 
//             if (hex)
//             {
//                 SEGGER_RTT_printf(0, "0x%02x ", +byte);
//             }
//             else 
//             {
//                 SEGGER_RTT_printf(0, "%u ", +byte);
//             }
//         #elif defined(X86_UNIT_TESTING_ONLY)
//             if ((byte_count) % m_page_width == 0)
//             {
//                 std::cout << std::endl << std::endl;
//             } 
//             if (hex)
//             {
//                 std::cout << "0x" << std::hex << std::setfill ('0') <<  std::setw(2) << +byte << " ";
//             }
//             else
//             {
//                 std::cout << +byte << " ";
//             }

//         #endif   

//         byte_count ++;
//     }
// }
// #endif

} // namespace ssd1306
