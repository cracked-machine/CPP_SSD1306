
#include <ssd1306_tester.hpp>
#include <catch2/catch_all.hpp>
#include <array>
#include <iomanip>
#include <numeric>

namespace ssd1306
{

ssd1306_tester::ssd1306_tester()
{
    REQUIRE(init());
}

bool ssd1306_tester::validate_buffer(std::vector<uint8_t> &validation_buffer)
{
    if (validation_buffer.size() != m_buffer.size())
    {
        std::cout << "Validation buffer error - expected size: " << m_buffer.size() 
            << ", actual size: " << validation_buffer.size() << std::endl;
        return false;
    }

    static ssd1306::FontTest font_under_test;
    // set the font character
    std::stringstream msg;
    msg << font_under_test.character_map[0];

    // write the font to the buffer
    write(msg, font_under_test, 0, 0, ssd1306::Colour::Black, ssd1306::Colour::White, true, true);

    //get_buffer(m_buffer);

    auto valid_buffer_iter = validation_buffer.begin();
    auto valid_buffer_end = validation_buffer.end();

    for (auto& byte : m_buffer)
    {
        if (byte != *valid_buffer_iter)
        {
            return false;
        }
        if (valid_buffer_iter != valid_buffer_end)
        {
            valid_buffer_iter++;
        }
    }
    return true;
}

bool ssd1306_tester::dump_buffer_as_hex()
{
#ifdef ENABLE_SSD1306_TEST_STDOUT    
    //get_buffer(m_buffer);
    uint8_t row_count {0};
    uint8_t col_count {0};

    std::cout << +row_count << ":\t";
    for (auto _byte : m_buffer)
    {
        std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << +_byte << ", " << std::flush;

        if (col_count >= 15)
        {
            col_count = 0; 
            row_count ++;
            
            std::cout << std::endl << std::dec << (row_count * 16) << ":\t" << std::flush;             
        }
        else 
        {
            col_count++;
        }
    }
#endif
    return true;
}

} // namespace ssd1306