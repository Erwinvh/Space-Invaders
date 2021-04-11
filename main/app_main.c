#include <string.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_wifi.h"

#include "esp_peripherals.h"
#include "periph_wifi.h"
#include "board.h"

#include "audio_mem.h"
#include "periph_touch.h"
#include "periph_adc_button.h"
#include "periph_button.h"

#include "freertos/timers.h"

#include "core.h"

int app_main(void)
{
    coreStart();

    return 1;
}