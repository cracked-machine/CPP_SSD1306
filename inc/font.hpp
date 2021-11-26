

#ifndef __FONT_HPP__
#define __FONT_HPP__

#include <stdint.h>
#include <array>
//#include <variant>
//#include <fontdata.hpp>



namespace ssd1306
{

class Font
{

public:

	// @brief Construct a new Font object
	// @param width font width, passed by derived class constructor
	// @param height font height, passed by derived class constructor
	Font(uint8_t width, uint8_t height) : m_width{width}, m_height{height} 
	{
	};
	
	// @brief Destroy the Font object
	virtual ~Font() 
	{
	};

	// @brief Pure Virtual base function to get the char object.
	// Must be overidden in derived font class.
	// @param idx The position in the font data array to retrieve data
	// @return uint16_t The halfword of data we retrieve
	virtual uint16_t get_char(size_t idx) = 0;

	// @brief get the width member variable 
	// @return uint8_t the width value
	uint8_t width() { return m_width; }

	// @brief get te height member variable 
	// @return uint8_t the height value
	uint8_t height() { return m_height; }

	// @brief Pure virtual helper function to get the size of the private font data array.
	// The font data array size is implementation-specific and is stored in the derived class.
	// @return size_t the array size
	virtual size_t size() = 0;

protected:

	// @brief The width of the font 
	uint8_t m_width{0}; 

	// @brief The height of the font 
	uint8_t m_height{0};  

private:


};

} // namespace ssd1306

#endif // __FONT_HPP__
