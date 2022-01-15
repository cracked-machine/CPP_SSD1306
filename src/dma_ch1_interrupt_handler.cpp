// MIT License

// Copyright (c) 2021 Chris Sutton

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <dma_ch1_interrupt_handler.hpp>
#include <ssd1306.hpp>

namespace ssd1306
{

DMA1_CH1_InterruptHandler::DMA1_CH1_InterruptHandler(std::unique_ptr<ssd1306::Display> &driver_instance)
: _driver_instance (std::move(driver_instance))
{

    std::unique_ptr<ISRManagerBaseSTM32G0> this_ext_interrupt = std::unique_ptr<ISRManagerBaseSTM32G0>(this);
    // Pass the interrupt number/driver pointer up to the base class.
    ISRManagerBaseSTM32G0::register_handler(
        isr::stm32g0::ISRManagerBaseSTM32G0::InterruptList::dma_ch1_irqhandler, 
        this_ext_interrupt);
}

void DMA1_CH1_InterruptHandler::ISR()
{
    _driver_instance->get_display_height();

    // prevent ISR lockup
    LL_DMA_ClearFlag_HT1(DMA1);
    LL_DMA_ClearFlag_TC1(DMA1);
}


} // namespace ssd1306