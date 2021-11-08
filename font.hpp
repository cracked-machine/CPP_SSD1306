#include <stdint.h>

#ifndef __FONT_HPP__
#define __FONT_HPP__

#include <array>
#include <variant>
#include <fontdata.hpp>

namespace ssd1306
{

class Font
{
public:
	Font() = default;

	virtual ~Font() = default;

	void init(std::variant<Font3x5_t, Font5x7_t, Font7x10_t, Font11x18_t, Font16x26_t> data);

	uint8_t width{0};    /*!< Font width in pixels */
	uint8_t height{0};   /*!< Font height in pixels */

	// the font character data
	std::variant<Font3x5_t, Font5x7_t, Font7x10_t, Font11x18_t, Font16x26_t> _data;

	// return value of data at idx
	uint16_t getChar(size_t idx);

};

} // namespace ssd1306

#endif // __FONT_HPP__
