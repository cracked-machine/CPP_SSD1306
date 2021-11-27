
#include <ssd1306_tester.hpp>
#include <catch2/catch_all.hpp>
#include <array>
#include <iomanip>

namespace ssd1306
{



ssd1306_tester::ssd1306_tester()
{
    REQUIRE(init());
}

bool ssd1306_tester::print_buffer_data()
{
    get_buffer(m_buffer);

    uint8_t line_count {0};
    for (auto _byte : m_buffer)
    {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << +_byte << " ";

        if (line_count < 16)
        {
            line_count++;
        }
        else 
        {
            line_count = 0; 
            std::cout << std::endl; 
        }
    }

    return true;
}

} // namespace ssd1306