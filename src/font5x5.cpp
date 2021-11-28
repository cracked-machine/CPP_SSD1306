#include <font.hpp>


namespace ssd1306
{

// 5x5 - '1'
// 11 LSB DONT CARE
//
// - - o - - x x x x x x x x x x x	0x2000  ROW #0
// - o o - - x x x x x x x x x x x	0x6000  ROW #1
// - - o - - x x x x x x x x x x x	0x2000  ROW #2
// - - o - - x x x x x x x x x x x	0x2000  ROW #3
// - o o o - x x x x x x x x x x x	0x7000  ROW #4

// @brief only 3 MSB of each halfword are used. 1 bit per col.
template<> uint8_t Font5x5::m_width{5};

// @brief 5 rows of two bytes (half-words)   
template<> uint8_t Font5x5::m_height{5};

// @brief The font data, top to bottom.
template<> std::array<uint16_t, 475> Font5x5::data {
//  ROW #0  ROW #1  ROW #2  ROW #3  ROW #4
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  // sp
    0x2000, 0x2000, 0x0000, 0x2000, 0x0000,  // !
    0x5000, 0x0000, 0x0000, 0x0000, 0x0000,  // "
    0x5000, 0xFFFF, 0x5000, 0xFFFF, 0x5000,  // #
    0x7800, 0xA000, 0x7000, 0x2800, 0xF000,  // $
    0x8800, 0x1000, 0x2000, 0x4000, 0x8800,  // %
    0x6000, 0xC000, 0x6000, 0x9000, 0x6800,  // &
    0x2000, 0x0000, 0x0000, 0x0000, 0x0000,  // '
    0x1000, 0x4000, 0x4000, 0x4000, 0x1000,  // (
    0x4000, 0x1000, 0x1000, 0x1000, 0x4000,  // )
    0xA800, 0x7000, 0x5000, 0x7000, 0xA800,  // *
    0x0000, 0x2000, 0x7000, 0x2000, 0x0000,  // +
    0x0000, 0x0000, 0x2000, 0x2000, 0x4000,  // ,
    0x0000, 0x0000, 0x7000, 0x0000, 0x0000,  // -
    0x0000, 0x0000, 0x0000, 0x0000, 0x2000,  // .
    0x0800, 0x1000, 0x2000, 0x4000, 0x8000,  // /
    0x7000, 0x5000, 0x5000, 0x5000, 0x7000,  // 0
    0x2000, 0x6000, 0x2000, 0x2000, 0x7000,  // 1
    0x7000, 0x1000, 0x7000, 0x4000, 0x7000,  // 2
    0x7000, 0x1000, 0x7000, 0x1000, 0x7000,  // 3
    0x8000, 0x8000, 0xA000, 0xFFFF, 0x2000,  // 4
    0x7000, 0x4000, 0x7000, 0x1000, 0x7000,  // 5
    0x7000, 0x4000, 0x7000, 0x5000, 0x7000,  // 6
    0x7000, 0x1000, 0x3800, 0x1000, 0x1000,  // 7
    0x7000, 0x5000, 0x7000, 0x5000, 0x7000,  // 8
    0x7000, 0x5000, 0x7000, 0x1000, 0x1000,  // 9
    0x0000, 0x2000, 0x0000, 0x2000, 0x0000,  // :
    0x2000, 0x0000, 0x2000, 0x4000, 0x0000,  // ;
    0x2000, 0x4000, 0x8000, 0x4000, 0x2000,  // <
    0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000,  // =
    0x8000, 0x1000, 0x0800, 0x1000, 0x8000,  // >
    0x7000, 0x8800, 0x1000, 0x0000, 0x2000,  // ?
    0x7000, 0xA800, 0xB800, 0x8000, 0x7000,  // @
    0x2000, 0x5000, 0x7000, 0x5000, 0x5000,  // A
    0x6000, 0x5000, 0x7000, 0x5000, 0x6000,  // B
    0x7000, 0x4000, 0x4000, 0x4000, 0x7000,  // C
    0x6000, 0x5000, 0x5000, 0x5000, 0x6000,  // D
    0x7000, 0x4000, 0x7000, 0x4000, 0x7000,  // E
    0x7000, 0x4000, 0x7000, 0x4000, 0x4000,  // F
    0x7000, 0x4000, 0x5000, 0x5000, 0x7000,  // G
    0x5000, 0x5000, 0x7000, 0x5000, 0x5000,  // H
    0x2000, 0x2000, 0x2000, 0x2000, 0x2000,  // I
    0x1000, 0x1000, 0x1000, 0x5000, 0x2000,  // J
    0x5000, 0x6000, 0x4000, 0x6000, 0x5000,  // K
    0x4000, 0x4000, 0x4000, 0x4000, 0x7000,  // L
    0x8800, 0xD800, 0xA800, 0x8800, 0x8800,  // M
    0x7000, 0x5000, 0x5000, 0x5000, 0x5000,  // N
    0x7000, 0x5000, 0x5000, 0x5000, 0x7000,  // O
    0x7000, 0x5000, 0x7000, 0x4000, 0x4000,  // P
    0x7000, 0x8800, 0x8800, 0x7000, 0x1800,  // Q
    0x0000, 0x7000, 0x4000, 0x4000, 0x4000,  // R
    0x7000, 0x4000, 0x7000, 0x1000, 0x7000,  // S
    0x7000, 0x2000, 0x2000, 0x2000, 0x2000,  // T
    0x5000, 0x5000, 0x5000, 0x5000, 0x7000,  // U
    0x8800, 0x5000, 0x5000, 0x5000, 0x2000,  // V
    0x8800, 0x8800, 0xA800, 0xA800, 0x5000,  // W
    0x8800, 0x5000, 0x2000, 0x5000, 0x8800,  // X
    0x8800, 0x5000, 0x2000, 0x2000, 0x2000,  // Y
    0x7000, 0x1000, 0x2000, 0x4000, 0x7000,  // Z
    0x7000, 0x4000, 0x4000, 0x4000, 0x7000,  // [
    0x8000, 0x4000, 0x2000, 0x1000, 0x0800,  /* \ */
    0x7000, 0x1000, 0x1000, 0x1000, 0x7000,  // ]
    0x2000, 0x5000, 0x0000, 0x0000, 0x0000,  // ^
    0x0000, 0x0000, 0x0000, 0x0000, 0x7000,  // _
    0x4000, 0x2000, 0x0000, 0x0000, 0x0000,  // `
    0x2000, 0x5000, 0x7000, 0x5000, 0x5000,  // a
    0x6000, 0x5000, 0x7000, 0x5000, 0x6000,  // b
    0x7000, 0x4000, 0x4000, 0x4000, 0x7000,  // c
    0x6000, 0x5000, 0x5000, 0x5000, 0x6000,  // d
    0x7000, 0x4000, 0x7000, 0x4000, 0x7000,  // e
    0x7000, 0x4000, 0x7000, 0x4000, 0x4000,  // f
    0x7000, 0x4000, 0x5000, 0x5000, 0x7000,  // g
    0x5000, 0x5000, 0x7000, 0x5000, 0x5000,  // h
    0x2000, 0x2000, 0x2000, 0x2000, 0x2000,  // i
    0x1000, 0x1000, 0x1000, 0x5000, 0x2000,  // j
    0x5000, 0x6000, 0x4000, 0x6000, 0x5000,  // k
    0x4000, 0x4000, 0x4000, 0x4000, 0x7000,  // l
    0x8800, 0xD800, 0xA800, 0x8800, 0x8800,  // m
    0x7000, 0x5000, 0x5000, 0x5000, 0x5000,  // n
    0x7000, 0x5000, 0x5000, 0x5000, 0x7000,  // o
    0x7000, 0x5000, 0x7000, 0x4000, 0x4000,  // p
    0x7000, 0x8800, 0x8800, 0x7000, 0x1800,  // q
    0x0000, 0x7000, 0x4000, 0x4000, 0x4000,  // r
    0x7000, 0x4000, 0x7000, 0x1000, 0x7000,  // s
    0x7000, 0x2000, 0x2000, 0x2000, 0x2000,  // t
    0x5000, 0x5000, 0x5000, 0x5000, 0x7000,  // u
    0x8800, 0x5000, 0x5000, 0x5000, 0x2000,  // v
    0x8800, 0x8800, 0xA800, 0xA800, 0x5000,  // w
    0x8800, 0x5000, 0x2000, 0x5000, 0x8800,  // x
    0x8800, 0x5000, 0x2000, 0x2000, 0x2000,  // Y
    0x7000, 0x1000, 0x2000, 0x4000, 0x7000,  // z
    0x3000, 0x4000, 0xC000, 0x4000, 0x3000,  // {
    0x1000, 0x1000, 0x1000, 0x1000, 0x1000,  // |
    0x6000, 0x1000, 0x1800, 0x1000, 0x6000,  // }
    0x0000, 0x5000, 0x2800, 0x0000, 0x7000,  // ~
};

}