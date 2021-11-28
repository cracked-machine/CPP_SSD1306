
#include <catch2/catch_all.hpp>
#include <ssd1306.hpp>
#include <ssd1306_tester.hpp>

TEST_CASE("Test Fonts", "[ssd1306_fonts]")
{

    SECTION("3x5Font")
    {
        static ssd1306::Font5x5 test_font;
        REQUIRE(test_font.width() == 5);
        REQUIRE(test_font.height() == 5);
        REQUIRE(test_font.size() == 475);
        uint32_t result{0};
        REQUIRE(test_font.get_pixel(200, result));
        REQUIRE(result ==  20480);       
    }

    SECTION("5x7Font")
    {
        static ssd1306::Font5x7 test_font;
        REQUIRE(test_font.width() == 5);
        REQUIRE(test_font.height() == 7);
        REQUIRE(test_font.size() == 680);   
        uint32_t result{0};     
        REQUIRE(test_font.get_pixel(7, result));
        REQUIRE(result ==  0x1000);
    }    

    SECTION("7x10Font")
    {
        static ssd1306::Font7x10 test_font;
        REQUIRE(test_font.width() == 7);
        REQUIRE(test_font.height() == 10);
        REQUIRE(test_font.size() == 950);
        uint32_t result{0};
        REQUIRE(test_font.get_pixel(200, result));
        REQUIRE(result ==  2048);
    }

    SECTION("11x16Font")
    {
        static ssd1306::Font11x18 test_font;
        REQUIRE(test_font.width() == 11);
        REQUIRE(test_font.height() == 18);
        REQUIRE(test_font.size() == 1710);
        uint32_t result{0};
        REQUIRE(test_font.get_pixel(179, result));
        REQUIRE(result ==  8192);
    }

    SECTION("16x26Font")
    {
        static ssd1306::Font16x26 test_font;
        REQUIRE(test_font.width() == 16);
        REQUIRE(test_font.height() == 26);
        REQUIRE(test_font.size() == 2470);
        uint32_t result{0};
        REQUIRE(test_font.get_pixel(150, result));
        REQUIRE(result ==  57407);
    }

}

TEST_CASE("Test Display public API", "[ssd1306_display_public]")
{
    static ssd1306::Font16x26 font;
    static ssd1306::Display oled;
    REQUIRE(oled.init());
    
    SECTION("Write white on black")
    {
        std::stringstream text("TEST");
        REQUIRE(oled.write(text, font, 2, 2, ssd1306::Colour::Black, ssd1306::Colour::White, true, true));
    }
    SECTION("Write black on white")
    {
        std::stringstream text("TEST");
        REQUIRE(oled.write(text, font, 2, 2, ssd1306::Colour::White, ssd1306::Colour::Black, true, true));
    }
    SECTION("Write with invalid x position")
    {
        std::stringstream text("TEST");
        REQUIRE_FALSE(oled.write(text, font, 255, 2, ssd1306::Colour::Black, ssd1306::Colour::White, true, true));
    }
    SECTION("Write with invalid y position")
    {

        std::stringstream text("TEST");
        REQUIRE_FALSE(oled.write(text, font, 2, 255, ssd1306::Colour::Black, ssd1306::Colour::White, true, true));
    }


   
}

TEST_CASE("Test Display protected API", "[ssd1306_display_protected]")
{
    static ssd1306::Font16x26 font;
    SECTION("Write invalid char")
    {
        ssd1306::ssd1306_tester tester;
        REQUIRE_FALSE(tester.test_write_char(0, font, ssd1306::Colour::Black, true));
    }

    SECTION("Write uninitialised string")
    {
        std::stringstream text;
        ssd1306::ssd1306_tester tester;
        REQUIRE_FALSE(tester.test_write_string(text, font, ssd1306::Colour::White, true));
    }
}

TEST_CASE("Check font output", "[font_check]")
{

    SECTION("Print buffer as hex")
    {
        static ssd1306::Font16x26 font;
        ssd1306::ssd1306_tester tester;
        std::stringstream text("TEST");
        REQUIRE(tester.write(text, font, 2, 2, ssd1306::Colour::Black, ssd1306::Colour::White, true, true));
#ifdef ENABLE_SSD1306_STDOUT
        tester.print_buffer_data();
#endif
        
    }

    SECTION("Print individual characters to stdout")
    {
        static ssd1306::Font5x7 font;
        ssd1306::ssd1306_tester tester;
        for (uint8_t count = 0; count < font.character_map.size(); count++)
        {
            std::stringstream msg;
	        msg << font.character_map[count];

#ifdef ENABLE_SSD1306_STDOUT
            std::cout << std::endl << std::endl << "---" << std::endl << "|" << 
                font.character_map[count] << 
                "|" << std::endl << "---" << std::endl <<
                " ^ " << std::endl << " | " << std::endl;
#endif
		    tester.write(msg, font, 2, 2, ssd1306::Colour::Black, ssd1306::Colour::White, true, true);

        }
    }
}