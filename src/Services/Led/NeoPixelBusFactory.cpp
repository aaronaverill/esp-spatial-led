#include "Services/Led/NeoPixelBusFactory.h"
#include "Services/Led/NeoPixelBusManager.h"

namespace Services { namespace Led {

  /**
   * Create the appropriate NeoPixelBus object.
   * https://github.com/Makuna/NeoPixelBus/wiki/T_METHOD
   */
  INeoPixelBus *NeoPixelBusFactory::create(Chipset chipset, ColorOrder colorOrder, uint16_t ledCount) {
    switch (chipset) {
      case Chipset::WS2812:
        return NeoPixelBusFactory::createWs2812Controller(chipset, colorOrder, ledCount);
      case Chipset::WS2813:
        return NeoPixelBusFactory::createWs2813Controller(chipset, colorOrder, ledCount);
      default:
        return nullptr;
    }  
  }

  /**
   * Create a controller for WS2812a, WS2812b, WS2812c, etc
   */
  INeoPixelBus *NeoPixelBusFactory::createWs2812Controller(Chipset chipset, ColorOrder colorOrder, uint16_t ledCount) {
    switch (colorOrder) {
      case ColorOrder::RGB:
        return new NeoPixelBusManager<NeoRgbFeature, NeoWs2812xMethod>(chipset, colorOrder, ledCount);
      case ColorOrder::RBG:
        return new NeoPixelBusManager<NeoRbgFeature, NeoWs2812xMethod>(chipset, colorOrder, ledCount);
      case ColorOrder::GBR:
        return new NeoPixelBusManager<NeoGbrFeature, NeoWs2812xMethod>(chipset, colorOrder, ledCount);
      case ColorOrder::GRB:
        return new NeoPixelBusManager<NeoGrbFeature, NeoWs2812xMethod>(chipset, colorOrder, ledCount);
      case ColorOrder::BGR:
        return new NeoPixelBusManager<NeoBgrFeature, NeoWs2812xMethod>(chipset, colorOrder, ledCount);
      case ColorOrder::BRG:
        return new NeoPixelBusManager<NeoBrgFeature, NeoWs2812xMethod>(chipset, colorOrder, ledCount);
      default:
        return nullptr;
    }
  }

  /**
   * Create a controller for WS1813 led strips
  */
  INeoPixelBus *NeoPixelBusFactory::createWs2813Controller(Chipset chipset, ColorOrder colorOrder, uint16_t ledCount) {
    switch (colorOrder) {
      case ColorOrder::RGB:
        return new NeoPixelBusManager<NeoRgbFeature, NeoWs2813Method>(chipset, colorOrder, ledCount);
      case ColorOrder::RBG:
        return new NeoPixelBusManager<NeoRbgFeature, NeoWs2813Method>(chipset, colorOrder, ledCount);
      case ColorOrder::GBR:
        return new NeoPixelBusManager<NeoGbrFeature, NeoWs2813Method>(chipset, colorOrder, ledCount);
      case ColorOrder::GRB:
        return new NeoPixelBusManager<NeoGrbFeature, NeoWs2813Method>(chipset, colorOrder, ledCount);
      case ColorOrder::BGR:
        return new NeoPixelBusManager<NeoBgrFeature, NeoWs2813Method>(chipset, colorOrder, ledCount);
      case ColorOrder::BRG:
        return new NeoPixelBusManager<NeoBrgFeature, NeoWs2813Method>(chipset, colorOrder, ledCount);
      default:
        return nullptr;
    }
  }

  /**
   * Create a controller for SK6812 led strips
   */
  // NeoSk6812Method

  /**
   * Create a controller for APA106 led strips
   */
  // NeoApa106Method
}}