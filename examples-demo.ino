/*
 * demo.ino
 *
 * Demonstration application for the JSON serialization library.
 *
 * This example:
 *  - Creates fake gateway and meter data
 *  - Calls the serialization library
 *  - Outputs the resulting JSON via Serial
 *
 * Platform: Arduino (AVR)
 */

#include <Arduino.h>

/* Include the pure C library */
extern "C" {
#include "json_serializer.h"
}

/* ------------------------------------------------------------
 * Global data objects
 * ------------------------------------------------------------ */

static JsonGateway gateway;

/* Output buffer for JSON */
static char json_buffer[512];

/* ------------------------------------------------------------
 * Setup function (runs once)
 * ------------------------------------------------------------ */

void setup()
{
    Serial.begin(9600);

    /* Give time for Serial monitor / Proteus terminal */
    delay(1000);

    Serial.println("JSON Serialization Demo");
    Serial.println("-----------------------");

    /* --------------------------------------------------------
     * Populate gateway-level metadata
     * -------------------------------------------------------- */

    gateway.gateway_id     = "gateway_1234 \r\n";
    gateway.date           = "1970-01-01 \r\n";
    gateway.device_type    = "stromleser \r\n";
    gateway.interval_minutes = 15;
    gateway.total_readings   = 1;

    /* --------------------------------------------------------
     * Populate device readings
     * -------------------------------------------------------- */

    gateway.values.device_count = 1;

    JsonDeviceReading *device = &gateway.values.devices[0];

    device->media     = "water \r\n";
    device->meter     = "waterstarm \r\n";
    device->device_id = "stromleser_50898527 \r\n";
    device->unit      = "m3 \r\n";

    /* --------------------------------------------------------
     * Populate data points
     * -------------------------------------------------------- */

    device->data_count = 1;

    device->data[0].timestamp        = "1970-01-01 00:00 \r\n";
    device->data[0].meter_datetime   = "1970-01-01 00:00 \r\n";
    device->data[0].total_m3         = 107.752f;
    device->data[0].status           = "OK \r\n";

    /* --------------------------------------------------------
     * Serialize to JSON
     * -------------------------------------------------------- */

    int result = json_serialize_gateway(
        &gateway,
        json_buffer,
        sizeof(json_buffer)
    );

    /* --------------------------------------------------------
     * Output result
     * -------------------------------------------------------- */

    if (result == 0) {
        Serial.println("Serialization successful:");
        Serial.println();
        Serial.println(json_buffer);
    } else {
        Serial.print("Serialization failed. Error code: ");
        Serial.println(result);
    }
}

/* ------------------------------------------------------------
 * Loop function (unused)
 * ------------------------------------------------------------ */

void loop()
{
    /* Nothing to do here */
}
