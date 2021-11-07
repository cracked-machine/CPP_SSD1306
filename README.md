
### Example

Very basic test: flashing text!

```
#ifdef __cplusplus
  extern "C"
  {
#endif

#include "ssd1306.hpp"

  void test()
  {
    Font xl_font;
    xl_font.init(Fonts::Font16x26);

    Ssd1306 oled;
    oled.init();

    while(true)
    {
      oled.fill(Ssd1306::Colour::Black);
      oled.set_cursor(2, 0);
      std::stringstream text("TEST");
      oled.write_string(text, xl_font, Ssd1306::Colour::White, 0);
      oled.update_screen();

      HAL_Delay(1000);
      oled.fill(Ssd1306::Colour::Black);
      oled.update_screen();
      HAL_Delay(1000);
    }
  }

#ifdef __cplusplus
  }
#endif


```
