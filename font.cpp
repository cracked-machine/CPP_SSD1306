#include <font.hpp>
#include <array>
#include <cstddef>

namespace ssd1306
{

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

uint16_t Font::getChar(size_t idx)
{

	if (const auto& font3x5_data = std::get_if<Font3x5_t>(&_data))
	{
		return font3x5_data->at(idx);
	}
	else if (const auto& font5x7_data = std::get_if<Font5x7_t>(&_data))
	{
		return font5x7_data->at(idx);
	}
	else if (const auto& font7x10_data = std::get_if<Font7x10_t>(&_data))
	{
		return font7x10_data->at(idx);
	}
	else if (const auto& font11x18_data = std::get_if<Font11x18_t>(&_data))
	{
		return font11x18_data->at(idx);
	}
	else if (const auto& font16x26_data = std::get_if<Font16x26_t>(&_data))
	{
		return font16x26_data->at(idx);
	}

	// Default value in case of monostate.
	// Suppress "-Wreturn-type" warning.
	return 0;
}

} //namespace ssd1306
