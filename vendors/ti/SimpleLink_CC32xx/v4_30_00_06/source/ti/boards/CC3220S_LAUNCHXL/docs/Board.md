# SimpleLink&trade; CC3220S LaunchPad&trade; Settings & Resources

The [__SimpleLink CC3220S LaunchPad__][launchpad] contains a
[__CC3220S__][device] device.


## Jumper Settings

* Close the __`LEDs`__ jumper to enable the on-board LEDs.
* Close the __`RX`__ and __`TX`__ jumpers to enable UART via
the XDS110 on-board USB debugger.
* Close the __`OPAMP EN`__, __`GND`__, and __`BRD`__ jumpers to enable the
operation amplifier circuit on analog pins __`P57`__, __`P58`__, __`P59`__,
and __`P60`__. The operation amplifier circuit--even when disabled--prevents
any output on the respective pins. Additionally, the input voltage is clipped
to 1.4 Volts.


## Header Pins

Header pins designated with an asterisk (__`*`__) on the silkscreen are not
connected to the device by default.


## SysConfig Board File

The [CC3220S_LAUNCHXL.syscfg.json](../.meta/CC3220S_LAUNCHXL.syscfg.json)
is a handcrafted file used by SysConfig. It describes the physical pin-out
and components on the LaunchPad.


## Driver Examples Resources

Examples utilize SysConfig to generate software configurations into
the __ti_drivers_config.c__ and __ti_drivers_config.h__ files. The SysConfig
user interface can be utilized to determine pins and resources used.
Information on pins and resources used is also present in both generated files.


## TI BoosterPacks&trade;

The following BoosterPack(s) are used with some driver examples.

#### [__BOOSTXL-SHARP128 LCD & SD Card BoosterPack__][boostxl-sharp128]

  * The SD card's default SPI bus is not compatible with this LaunchPad's
    header pin layout. To use the SD card, the BoosterPack ___cannot___ be
    placed directly atop the LaunchPad. Use the following modification(s) to
    enable SD card usage with the __fatsd__, __fatsdraw__, and __sdraw__
    example(s).

    * Use a jumper wire to connect:

          |   LaunchPad    | BOOSTXL-SHARP128 |
          |:--------------:|:----------------:|
          | __`3V3`__      | __`3V3`__        |
          | __`GND`__      | __`GND`__        |
          | __`P08/CMD`__  | __`SPI_MOSI`__   |
          | __`P07/CLK`__  | __`SPI_CLK`__    |
          | __`P06/DATA`__ | __`SPI_MISO`__   |

  * The LCD's SPI slave select and power pins are not compatible with this
    LaunchPad's header pin layout. The BoosterPack ___can___ be placed
    directly atop the LaunchPad. Use the following modification(s) to use
    the BOOSTXL-SHARP128 LCD with the __display__ example.

    * Use a jumper wire to connect:

          | LaunchPad | BOOSTXL-SHARP128 |
          |:---------:|:----------------:|
          | __`3V3`__ | __`LCD_PWR`__    |
          | __`P03`__ | __`LCD_CS`__     |

#### [__BP-BASSENSORSMKII BoosterPack__][bp-bassensorsmkii]

  * The OPT3001's interrupt pin is not compatible with this LaunchPad. Use one
    of the following modification(s) to enable the OPT3001's usage with the
    __i2copt3001_cpp__ example.

    * Use a jumper wire to connect  __`O_INT`__ to __`P05`__.

#### [__CC3200 Audio BoosterPack__][cc3200audboost]
  * No modifications are needed.


## Reset Behavior

By default, the application is not retained after a power reset.



[device]: http://www.ti.com/product/CC3220S
[launchpad]: http://www.ti.com/tool/CC3220S-LAUNCHXL
[boostxl-sharp128]: http://www.ti.com/tool/boostxl-sharp128
[bp-bassensorsmkii]: http://www.ti.com/tool/bp-bassensorsmkii
[cc3200audboost]: http://www.ti.com/tool/cc3200audboost