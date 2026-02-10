/**
* SYSCFG Generated Driver File - Patched for PlatformIO
 */

#include "../../system/utils/compiler.h"
#include "../syscfg.h"

void SYSCFG_Initialize(void)
{
    // SYSCFG.VUSBCTRL = (0 << SYSCFG_USBVREG_bp);  // COMMENTED OUT: Not on DA28
}

uint8_t SYSCFG_GetRevId(void)
{
    return SYSCFG.REVID;
}

inline void SYSCFG_UsbVregEnable(void)
{
    // SYSCFG.VUSBCTRL = SYSCFG_USBVREG_bm; // COMMENTED OUT
}

inline void SYSCFG_UsbVregDisable(void)
{
    // SYSCFG.VUSBCTRL = ~SYSCFG_USBVREG_bm; // COMMENTED OUT
}
