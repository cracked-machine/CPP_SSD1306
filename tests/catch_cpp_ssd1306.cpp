
#include <catch2/catch_all.hpp>
#include <ssd1306.hpp>



TEST_CASE("Test Fonts", "[cpp_ssd1306]")
{
    SECTION("BaseFont")
    {
        static ssd1306::Font test_font(0, 0);
        REQUIRE(test_font.width() == 0);
        REQUIRE(test_font.height() == 0);
        REQUIRE(test_font.size() == 0);
        REQUIRE(test_font.get_char(0) == 0);
    }

    SECTION("3x5Font")
    {
        static ssd1306::Font3x5 test_font;
        REQUIRE(test_font.width() == 5);
        REQUIRE(test_font.height() == 5);
        REQUIRE(test_font.size() == 475);
        REQUIRE(test_font.get_char(200) == 20480);       
    }

    SECTION("5x7Font")
    {
        static ssd1306::Font5x7 test_font;
        REQUIRE(test_font.width() == 5);
        REQUIRE(test_font.height() == 7);
        REQUIRE(test_font.size() == 680);        
        REQUIRE(test_font.get_char(200) == 20480);
    }    

    SECTION("7x10Font")
    {
        static ssd1306::Font7x10 test_font;
        REQUIRE(test_font.width() == 7);
        REQUIRE(test_font.height() == 10);
        REQUIRE(test_font.size() == 950);
        REQUIRE(test_font.get_char(200) == 2048);
    }

    SECTION("11x16Font")
    {
        static ssd1306::Font11x16 test_font;
        REQUIRE(test_font.width() == 11);
        REQUIRE(test_font.height() == 16);
        REQUIRE(test_font.size() == 1710);
        REQUIRE(test_font.get_char(179) == 8192);
    }

    SECTION("16x26Font")
    {
        static ssd1306::Font16x26 test_font;
        REQUIRE(test_font.width() == 16);
        REQUIRE(test_font.height() == 26);
        REQUIRE(test_font.size() == 2470);
        REQUIRE(test_font.get_char(150) == 57407);
    }

}