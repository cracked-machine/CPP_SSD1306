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

#include <font.hpp>
#include <ssd1306_tester.hpp>

namespace ssd1306
{

// FontTest is declared in ssd1306_tester.hpp

// 16x26 - '0xDEADBEEF'
// 0 LSB DONT CARE
//
// o o - o | o o o - | o - o - | o o - o | o - o o | o o o - | o o o - | o o o o 	0x00F0  ROW #0
// o o - o | o o o - | o - o - | o o - o | o - o o | o o o - | o o o - | o o o o 	0x00F0  ROW #1
// o o - o | o o o - | o - o - | o o - o | o - o o | o o o - | o o o - | o o o o 	0x00F0  ROW #2
// o o - o | o o o - | o - o - | o o - o | o - o o | o o o - | o o o - | o o o o 	0x00F0  ROW #3
// o o - o | o o o - | o - o - | o o - o | o - o o | o o o - | o o o - | o o o o 	0x00F0  ROW #4
// o o - o | o o o - | o - o - | o o - o | o - o o | o o o - | o o o - | o o o o 	0x00F0  ROW #5
// o o - o | o o o - | o - o - | o o - o | o - o o | o o o - | o o o - | o o o o 	0x00F0  ROW #6
// o o - o | o o o - | o - o - | o o - o | o - o o | o o o - | o o o - | o o o o 	0x00F0  ROW #7
// o o - o | o o o - | o - o - | o o - o | o - o o | o o o - | o o o - | o o o o 	0x00F0  ROW #8
// o o - o | o o o - | o - o - | o o - o | o - o o | o o o - | o o o - | o o o o 	0x00F0  ROW #9
// o o - o | o o o - | o - o - | o o - o | o - o o | o o o - | o o o - | o o o o 	0x00F0  ROW #10
// o o - o | o o o - | o - o - | o o - o | o - o o | o o o - | o o o - | o o o o 	0x00F0  ROW #11
// o o - o | o o o - | o - o - | o o - o | o - o o | o o o - | o o o - | o o o o 	0x00F0  ROW #12
// o o - o | o o o - | o - o - | o o - o | o - o o | o o o - | o o o - | o o o o 	0x00F0  ROW #13
// o o - o | o o o - | o - o - | o o - o | o - o o | o o o - | o o o - | o o o o 	0x00F0  ROW #14
// o o - o | o o o - | o - o - | o o - o | o - o o | o o o - | o o o - | o o o o 	0x00F0  ROW #15


// // @brief only 3 MSB of each halfword are used. 1 bit per col.
// template<> const uint8_t ssd1306::FontTest::m_width{16};

// // @brief 5 rows of two bytes (half-words)   
// template<> const uint8_t ssd1306::FontTest::m_height{26};

// // @brief The font data, top to bottom.
// template<> std::array<uint16_t, 26> ssd1306::FontTest::data {
// //  ROW #0  ROW #1  ROW #2  ROW #3  ROW #4  ROW #5  ROW #6  ROW #7  ROW #8  ROW #9  ROW#10  ROW#11  ROW#12  ROW#13  ROW#14  ROW#15  ROW#16  ROW#17  ROW#18  ROW#19  ROW#20  ROW#21  ROW#22  ROW#23  ROW#24  ROW#25  
//     0xDEAD, 0xBEEF, 0xDEAD, 0xBEEF, 0xDEAD, 0xBEEF, 0xDEAD, 0xBEEF, 0xDEAD, 0xBEEF, 0xDEAD, 0xBEEF, 0xDEAD, 0xBEEF, 0xDEAD, 0xBEEF, 0xDEAD, 0xBEEF, 0xDEAD, 0xBEEF, 0xDEAD, 0xBEEF, 0xDEAD, 0xBEEF, 0xDEAD, 0xBEEF
// };

}