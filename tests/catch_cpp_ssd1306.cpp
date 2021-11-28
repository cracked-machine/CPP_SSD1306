
#include <catch2/catch_all.hpp>
#include <ssd1306.hpp>
#include <ssd1306_tester.hpp>

TEST_CASE("Test Fonts", "[ssd1306_fonts]")
{
    SECTION("3x5Font")
    {
        static ssd1306::Font5x5 font_under_test;
        REQUIRE(font_under_test.width() == 5);
        REQUIRE(font_under_test.height() == 5);
        REQUIRE(font_under_test.size() == 475);
    }

    SECTION("5x7Font")
    {
        static ssd1306::Font5x7 font_under_test;
        REQUIRE(font_under_test.width() == 5);
        REQUIRE(font_under_test.height() == 7);
        REQUIRE(font_under_test.size() == 680);   
    }    

    SECTION("7x10Font")
    {
        static ssd1306::Font7x10 font_under_test;
        REQUIRE(font_under_test.width() == 7);
        REQUIRE(font_under_test.height() == 10);
        REQUIRE(font_under_test.size() == 950);
    }

    SECTION("11x16Font")
    {
        static ssd1306::Font11x18 font_under_test;
        REQUIRE(font_under_test.width() == 11);
        REQUIRE(font_under_test.height() == 18);
        REQUIRE(font_under_test.size() == 1710);
    }

    SECTION("16x26Font")
    {
        static ssd1306::Font16x26 font_under_test;
        REQUIRE(font_under_test.width() == 16);
        REQUIRE(font_under_test.height() == 26);
        REQUIRE(font_under_test.size() == 2470);
    }

}

// @brief Test the public API of the ssd1306::Display class
TEST_CASE("Test Display public API", "[ssd1306_display_public]")
{
    static ssd1306::Font16x26 font_under_test;
    static ssd1306::Display oled;
    REQUIRE(oled.init());
    
    SECTION("Write white on black")
    {
        std::stringstream text("TEST");
        REQUIRE(oled.write(text, font_under_test, 2, 2, ssd1306::Colour::Black, ssd1306::Colour::White, true, true));
    }
    SECTION("Write black on white")
    {
        std::stringstream text("TEST");
        REQUIRE(oled.write(text, font_under_test, 2, 2, ssd1306::Colour::White, ssd1306::Colour::Black, true, true));
    }
    SECTION("Write with invalid x position")
    {
        std::stringstream text("TEST");
        REQUIRE_FALSE(oled.write(text, font_under_test, 255, 2, ssd1306::Colour::Black, ssd1306::Colour::White, true, true));
    }
    SECTION("Write with invalid y position")
    {

        std::stringstream text("TEST");
        REQUIRE_FALSE(oled.write(text, font_under_test, 2, 255, ssd1306::Colour::Black, ssd1306::Colour::White, true, true));
    }
}

// @brief Test the protected API of the ssd1306::Display class
TEST_CASE("Test Display protected API", "[ssd1306_display_protected]")
{
    static ssd1306::Font16x26 font_under_test;
    SECTION("Write invalid char")
    {
        ssd1306::ssd1306_tester tester;
        REQUIRE_FALSE(tester.test_write_char(0, font_under_test, ssd1306::Colour::Black, true));
    }

    SECTION("Write uninitialised string")
    {
        std::stringstream text;
        ssd1306::ssd1306_tester tester;
        REQUIRE_FALSE(tester.test_write_string(text, font_under_test, ssd1306::Colour::White, true));
    }
}

// @brief hidden test to find the sum of Font Test. Not to be used in normal test runs 
TEST_CASE("Check font output", "[dump_fonts][.]")
{
    static ssd1306::FontTest font_under_test;
    ssd1306::ssd1306_tester tester;

    // use this test to find the sums of the buffers for each font character
    SECTION("Dump and accumulate")
    {
        // process the test font character in FontTest (font_test.cpp)

        // add font character to a string
        std::stringstream msg;
        msg << font_under_test.character_map[0];

        // write the string to the buffer
        tester.write(msg, font_under_test, 0, 0, ssd1306::Colour::Black, ssd1306::Colour::White, true, true);
        std::cout << std::endl << "++++++++++++++++++++++++++++++++++++++++" << std::endl;

        // dump the hex
        tester.dump_buffer_as_hex();

        // accumulate the sum of the buffer and add to results
        uint32_t result {0};
        tester.validate_buffer(0, result);
        std::cout << std::endl << result << std::endl;

    }

}

// @brief Validate the buffer encoding is correct using FontTest
TEST_CASE("SSD1306 Buffer Validation", "[ssd1306_buffer_check]")
{

    ssd1306::ssd1306_tester tester;
    
    SECTION("Buffer Sum Validation for Font11x18")
    {
        static ssd1306::FontTest font_under_test;

        uint32_t font_test_sumcheck {9472};


        // set the font character
        std::stringstream msg;
        msg << font_under_test.character_map[0];

        // write the font to the buffer
        tester.write(msg, font_under_test, 0, 0, ssd1306::Colour::Black, ssd1306::Colour::White, true, true);
        uint32_t __attribute__((unused)) result;
        REQUIRE(tester.validate_buffer(font_test_sumcheck, result));
    
    }
}

