var commonHeaders = xdc.loadCapsule("ti/drivers/sources_common.xs").commonHeaders;
var commonSources = xdc.loadCapsule("ti/drivers/sources_common.xs").commonSources;

var cc32xxSrcList = commonSources.concat([
    "./adc/ADCCC32XX.c",
    "Camera.c",
    "./camera/CameraCC32XXDMA.c",
    "Capture.c",
    "./capture/CaptureCC32XX.c",
    "./crypto/CryptoCC32XX.c",
    "./cryptoutils/utils/CryptoUtils.c",
    "./dma/UDMACC32XX.c",
    "./gpio/GPIOCC32XX.c",
    "./i2c/I2CCC32XX.c",
    "I2S.c",
    "./i2s/I2SCC32XX.c",
    "ITM.c",
    "./itm/ITMCC32XX.c",
    "./power/PowerCC32XX.c",
    "./power/PowerCC32XX_asm",
    "./pwm/PWMTimerCC32XX.c",
    "./sd/SDHostCC32XX.c",
    "./spi/SPICC32XXDMA.c",
    "./timer/TimerCC32XX.c",
    "./uart2/UART2CC32XX.c",
    "./uart/UARTCC32XX.c",
    "./uart/UARTCC32XXDMA.c",
    "./watchdog/WatchdogCC32XX.c"
]);

var cc32xxHdrList = commonHeaders.concat([
    "./adc/ADCCC32XX.h",
    "Camera.h",
    "./camera/CameraCC32XXDMA.h",
    "Capture.h",
    "./capture/CaptureCC32XX.h",
    "./crypto/CryptoCC32XX.h",
    "./cryptoutils/utils/CryptoUtils.h",
    "./dma/UDMACC32XX.h",
    "./gpio/GPIOCC32XX.h",
    "./i2c/I2CCC32XX.h",
    "I2S.h",
    "./i2s/I2SCC32XX.h",
    "ITM.h",
    "./power/PowerCC32XX.h",
    "./pwm/PWMTimerCC32XX.h",
    "./sd/SDHostCC32XX.h",
    "./spi/SPICC32XXDMA.h",
    "./timer/TimerCC32XX.h",
    "./uart2/UART2CC32XX.h",
    "./uart/UARTCC32XX.h",
    "./uart/UARTCC32XXDMA.h",
    "./watchdog/WatchdogCC32XX.h"
]);
