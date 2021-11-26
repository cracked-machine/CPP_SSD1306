

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


	Font(uint8_t width, uint8_t height) : m_width{width}, m_height{height} 
	{
	};
	
	virtual ~Font() 
	{
	};

	virtual uint16_t get_char(size_t idx) = 0;
	uint8_t width() { return m_width; }
	uint8_t height() { return m_height; }
	virtual size_t size();
protected:

	uint8_t m_width{0}; 
	uint8_t m_height{0};  

};

} // namespace ssd1306

#endif // __FONT_HPP__
