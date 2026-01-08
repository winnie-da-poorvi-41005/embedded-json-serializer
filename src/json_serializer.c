#include "json_serializer.h"

#include <stdio.h>    /* snprintf */
#include <string.h>   /* strlen */


/*
 * Safely append formatted text to the output buffer.
 * - Updates write position
 * - Prevents buffer overflow
 * - Returns error if buffer is too small
 */
#define APPEND_FMT(...)                                              \
    do {                                                             \
        int written = snprintf(output + pos,                         \
                               output_size - pos,                   \
                               __VA_ARGS__);                         \
        if (written < 0) {                                           \
            return -3; /* formatting error */                        \
        }                                                            \
        if ((size_t)written >= (output_size - pos)) {               \
            return -2; /* buffer too small */                        \
        }                                                            \
        pos += (size_t)written;                                      \
    } while (0)

/* 
 Public API implementation
 */

int json_serialize_gateway(
    const JsonGateway *gateway,
    char *output,
    size_t output_size
)
{
    size_t pos = 0;

    /*Basic parameter checks
 */
    if (gateway == NULL || output == NULL || output_size == 0) {
        return -1;
    }

    /* Outer JSON array start
 */
    APPEND_FMT("[{");

    /* Gateway-level metadata
 */
    APPEND_FMT("\"gatewayId\":\"%s\",", gateway->gateway_id);
    APPEND_FMT("\"date\":\"%s\",", gateway->date);
    APPEND_FMT("\"deviceType\":\"%s\",", gateway->device_type);
    APPEND_FMT("\"interval_minutes\":%lu,",
               (unsigned long)gateway->interval_minutes);
    APPEND_FMT("\"total_readings\":%lu,",
               (unsigned long)gateway->total_readings);

    /*
     Values object start
 */
    APPEND_FMT("\"values\":{");

    APPEND_FMT("\"device_count\":%lu,",
               (unsigned long)gateway->values.device_count);

    APPEND_FMT("\"readings\":[");

    /* 
     Device readings
 */
    for (size_t i = 0; i < gateway->values.device_count; i++) {
        const JsonDeviceReading *dev =
            &gateway->values.devices[i];

        APPEND_FMT("{");
        APPEND_FMT("\"media\":\"%s\",", dev->media);
        APPEND_FMT("\"meter\":\"%s\",", dev->meter);
        APPEND_FMT("\"deviceId\":\"%s\",", dev->device_id);
        APPEND_FMT("\"unit\":\"%s\",", dev->unit);

        /* 
         Data array
*/
        APPEND_FMT("\"data\":[");

        for (size_t j = 0; j < dev->data_count; j++) {
            const JsonMeterDataPoint *dp = &dev->data[j];

            APPEND_FMT("{");
            APPEND_FMT("\"timestamp\":\"%s\",", dp->timestamp);
            APPEND_FMT("\"meter_datetime\":\"%s\",",
                       dp->meter_datetime);
            APPEND_FMT("\"total_m3\":%.3f,", dp->total_m3);
            APPEND_FMT("\"status\":\"%s\"", dp->status);
            APPEND_FMT("}");

            /* Comma between data points */
            if (j + 1 < dev->data_count) {
                APPEND_FMT(",");
            }
        }

        APPEND_FMT("]}");

        /* Comma between devices */
        if (i + 1 < gateway->values.device_count) {
            APPEND_FMT(",");
        }
    }

    /* 
     * Close all objects
   */
    APPEND_FMT("]}");   /* end readings + values */
    APPEND_FMT("}]");   /* end gateway + array */

    return 0;
}

/*  Maximum output size estimation
 */

size_t json_get_max_output_size(void)
{
    /*
     * Conservative worst-case estimation.
     * This intentionally overestimates to guarantee safety.
     */

    const size_t base_overhead = 256;

    const size_t per_data_point =
        128; /* timestamps, float, status, punctuation */

    const size_t per_device =
        128 + (JSON_MAX_DATA_POINTS * per_data_point);

    return base_overhead +
           (JSON_MAX_DEVICES * per_device);
}
