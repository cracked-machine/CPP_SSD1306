#include "../CPP_SSD1306/fontdef.hpp"

#include <array>
#include <cstddef>

/*
Example
5x7 - #

-	-	-	-	-	0x0000
-	x	-	x	-	0x5000
x	x	x	x	x	0xFFFF
-	x	-	x	-	0x5000
x	x	x	x	x	0xFFFF
-	x	-	x	-	0x5000
-	-	-	-	-	0x0000

*/


void Font::init(std::variant<Font3x5_t, Font5x7_t, Font7x10_t, Font11x18_t, Font16x26_t> data)
{
	if (std::get_if<Font3x5_t>(&data))
	{
		width = 5;
		height = 5;
		_data = data;
	}
	else if (std::get_if<Font5x7_t>(&data))
	{
		width = 5;
		height = 7;
		_data = data;
	}
	else if (std::get_if<Font7x10_t>(&data))
	{
		width = 7;
		height = 10;
		_data = data;
	}
	else if (std::get_if<Font11x18_t>(&data))
	{
		width = 11;
		height = 18;
		_data = data;
	}
	else if (std::get_if<Font16x26_t>(&data))
	{
		width = 16;
		height = 26;
		_data = data;
	}
}

uint16_t Font::getPixel(size_t idx)
{

	if (const auto& data = std::get_if<Font3x5_t>(&_data))
	{
		return data->at(idx);
	}
	else if (const auto& data = std::get_if<Font5x7_t>(&_data))
	{
		return data->at(idx);
	}
	else if (const auto& data = std::get_if<Font7x10_t>(&_data))
	{
		return data->at(idx);
	}
	else if (const auto& data = std::get_if<Font11x18_t>(&_data))
	{
		return data->at(idx);
	}
	else if (const auto& data = std::get_if<Font16x26_t>(&_data))
	{
		return data->at(idx);
	}

	// Default value in case of monostate.
	// Suppress "-Wreturn-type" warning.
	return 0;
}

