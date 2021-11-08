
### Example

Very basic test: flashing text!

```
#ifdef __cplusplus
  extern "C"
  {
#endif

#include "ssd1306.hpp"
#include <sstream>

  void test()
  {
    ssd1306::FontData fd;
    ssd1306::Font xl_font;
    xl_font.init(fd.Font16x26);

    ssd1306::Display oled;
    oled.init();

    while(true)
    {
      oled.fill(ssd1306::Colour::Black);
      oled.set_cursor(2, 0);
      std::stringstream text("TEST");
      oled.write_string(text, xl_font, ssd1306::Colour::White, 0);
      oled.update_screen();

      HAL_Delay(1000);
      oled.fill(ssd1306::Colour::Black);
      oled.update_screen();
      HAL_Delay(1000);
    }
  }

#ifdef __cplusplus
  }
#endif


```
