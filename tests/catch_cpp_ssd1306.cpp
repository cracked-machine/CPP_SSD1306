


#include <catch2/catch_all.hpp>
#include <iostream>
#include <ssd1306.hpp>
#include <array>
#include <sstream>
#include <variant>


TEST_CASE("Test SSD1306 Driver", "[cpp_ssd1306]")
{
    
    ssd1306::FontData fd;
    ssd1306::Font test_font;

    SECTION("3x5Font")
    {
	    
	    REQUIRE(test_font.set_font(fd.Font3x5));
        REQUIRE(test_font.width == 5);
        REQUIRE(test_font.height == 5);
        const auto& font3x5_data = std::get_if<ssd1306::Font3x5_t>(&test_font._data);
        REQUIRE(font3x5_data->size() == 475);
        REQUIRE_FALSE(font3x5_data->size() == 0);
    }
    SECTION("5x7Font")
    {
	    
	    REQUIRE(test_font.set_font(fd.Font5x7));
        REQUIRE(test_font.width == 5);
        REQUIRE(test_font.height == 7);  
        const auto& font5x7_data = std::get_if<ssd1306::Font5x7_t>(&test_font._data);
        REQUIRE(font5x7_data->size() == 680);
        REQUIRE_FALSE(font5x7_data->size() == 0);              
    }    
    SECTION("7x10Font")
    {
	    
	    REQUIRE(test_font.set_font(fd.Font7x10));
        REQUIRE(test_font.width == 7);
        REQUIRE(test_font.height == 10);        
        const auto& font7x10_data = std::get_if<ssd1306::Font7x10_t>(&test_font._data);
        REQUIRE(font7x10_data->size() == 950);
        REQUIRE_FALSE(font7x10_data->size() == 0);        
    }    
    SECTION("11x18Font")
    {
	    
	    REQUIRE(test_font.set_font(fd.Font11x18));
        REQUIRE(test_font.width == 11);
        REQUIRE(test_font.height == 18);        
        const auto& font11x18_data = std::get_if<ssd1306::Font11x18_t>(&test_font._data);
        REQUIRE(font11x18_data->size() == 1710);
        REQUIRE_FALSE(font11x18_data->size() == 0);           
    }    
    SECTION("16x26Font")
    {
	    
	    REQUIRE(test_font.set_font(fd.Font16x26));
        REQUIRE(test_font.width == 16);
        REQUIRE(test_font.height == 26);        
        const auto& font16x26_data = std::get_if<ssd1306::Font16x26_t>(&test_font._data);
        REQUIRE(font16x26_data->size() == 2470);
        REQUIRE_FALSE(font16x26_data->size() == 0);           
    }    

}