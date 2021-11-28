
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

bool ssd1306_tester::validate_buffer(uint32_t expected_sum, uint32_t &actual_sum)
{
    get_buffer(m_buffer);
    uint32_t sum = std::accumulate(m_buffer.begin(), m_buffer.end(), 0);
    actual_sum = sum;

    std::cout << std::endl << "BufferValidation - Expected: " << expected_sum << ", Actual: " << actual_sum << std::endl;
    if (sum == expected_sum) { return true; }
    else { return false; }
}

bool ssd1306_tester::dump_buffer_as_hex()
{
    get_buffer(m_buffer);
    uint8_t row_count {0};
    uint8_t col_count {0};

    std::cout << +row_count << ":\t";
    for (auto _byte : m_buffer)
    {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << +_byte << " " << std::flush;

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

    return true;
}

} // namespace ssd1306