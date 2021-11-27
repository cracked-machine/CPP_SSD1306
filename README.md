
# SSD1306 Library

SSD1306 datasheet can be found [here](https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf)

## Design

### Font class

- The fonts are defined by a templated base class, which is specialised for the different size fonts and their asociated data

![cached_image](http://www.plantuml.com/plantuml/svg/dPDFJy8m5C3l-nJlP202MGTROIuB5qcyw45FncGPjB8c-sDeYTMvx-xeD62TBP4BsNkll_VRwzkIYtGdzaNkECucJpa7pWcQka4OhgfIHAkxsuVa_kRnEkI2X25dxpGHCTJEPLrGaLMaWNLLvR2b8dbXakQ3hWGOaIFOir9ClVRfOir4ahEI3k5uKgNdRVQL4P4DUbCPPTjC38OdgQw1-clZu_gPqRmDrroaWcsEHuj4SOqL7VwgfqQzRhhRfM_HL_a83dE8WQGYTPhUSKsbhqRJmNIJscz-vRuLJdDwRnGSci-mt_M66YQJ2JH_EFWI_zS1sno2kNkM0vR8DKbWWmHoRHOB_pmBX2GAJ1e8cJm2cmV2w4oHG7fcaS0Wubb5l1dk4VcX0D576zUNDOJZ1mxZMEtOKnb5QrQIEBOIV8arWjeG6q5R51snyEq8_PPr1iB83j5lI8UeiPv03hybBSc-o3y1)

### Display class

- The display class communicates with the SSD1306 controller via SPI. 
- The display is initialised by sending the required startup commands.
- Various write functions are available that accept the `Font` template type.

![cached image](http://www.plantuml.com/plantuml/svg/fLB1Rjf04BtxA-OKcOHIt5OHGXIb3QH1gXeKqbRgPRLuntZKzThQNISu5l_UnN4Id61oA0SpdlVclJVUFJEMQvidqWibDmQC4S77_kbmU9xADDSm6g7Aa_4OIW_SxwIC2ANOm5V9mx_m6VhhVh-9_8x9uWCIU1llfUw4J2PvuVHEoWHjdBgHPPfA84NMx-nqRrCI496KVfqbhFuwBmVpJ72Bp8GQKVclOA7v7SjeZTBFITa1ix3kmcDPTA5LFOovXZjD5drZnN1eh2QrSa_a2HmbPjM5YrJPqSNLZnkscFsPZk4eSeqk73HQhfxAo9KEXumBuQIxkmljJy8UGkmBP4nRdkQA3SiMbstfRn-k-P-grKe6BGjpRLAzzn0wdlUkUUuQtOZVSUs6JfqkJHAkn9DKc0YsB2osq0Mt_95x3DitjioZ25rcG_V8x6wFkLObL5_dxLnmwZZ9TW6DogwXx6yE4OhNX465axf5RL6qusI9Ixf_IIcDvPR2XjaT2Hj36NmOl46CaLQnXVBqqmwnkZzSQrwCQlyk-BNeyUDCv_rW34dzORnU3mH6f530pyMKNNxvpYRNir_JQ-ZrdeKNyncxv4f8l2aod63a14n6B4kron2xCcZvUARczDjnKatustnsnMgVOySHOQtXKWHiSiugmjorzcZMV5BFz3cf0-hLdTmrk9ukfZTlUTHJf1f3bTF6EqCbyaJ-0m00)
)

## Example

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
    ssd1306::Font3x5 xs_font;
    ssd1306::Display oled;
    oled.init();

    while(true)
    {
      std::stringstream msg("TEST");
      oled.write(msg, font, 2, 2, ssd1306::Colour::Black, ssd1306::Colour::White, 3, true);
      HAL_Delay(1000);

      std::stringstream msg("");
      oled.write(msg, font, 2, 2, ssd1306::Colour::Black, ssd1306::Colour::White, 3, true);
      HAL_Delay(1000);
    }
  }

#ifdef __cplusplus
  }
#endif


```
