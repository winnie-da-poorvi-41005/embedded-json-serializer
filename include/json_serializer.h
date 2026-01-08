#ifndef JSON_SERIALIZER_H
#define JSON_SERIALIZER_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*  Configuration Limits

 These limits define the maximum supported sizes.
  They can be adjusted based on memory constraints.
*/

#define JSON_MAX_DEVICES        8
#define JSON_MAX_DATA_POINTS   16

/*
  Data Model Definitions
 */

/**
 Represents a single meter data point.
 Corresponds to one entry inside the "data" array.
 */
typedef struct {
    const char *timestamp;        /* Format: YYYY-MM-DD HH:MM */
    const char *meter_datetime;   /* Format: YYYY-MM-DD HH:MM */
    float       total_m3;          /* Meter value (numeric, not string) */
    const char *status;            /* Example: "OK" */
} JsonMeterDataPoint;

/**
  Represents one device and its associated meter readings.
  Corresponds to one entry inside the "readings" array.
 */
typedef struct {
    const char *media;             /* Example: "water" */
    const char *meter;             /* Example: "waterstarm" */
    const char *device_id;         /* Example: "stromleser_50898527" */
    const char *unit;              /* Example: "m3" */

    JsonMeterDataPoint data[JSON_MAX_DATA_POINTS];
    size_t             data_count; /* Number of valid data points */
} JsonDeviceReading;

/**
 Represents the "values" object in the JSON structure.
 */
typedef struct {
    JsonDeviceReading devices[JSON_MAX_DEVICES];
    size_t            device_count;
} JsonValues;

/**
  Represents gateway-level metadata and readings.
  Corresponds to one object inside the outer JSON array.
 */
typedef struct {
    const char *gateway_id;        /* "gatewayId" */
    const char *date;              /* Format: YYYY-MM-DD */
    const char *device_type;       /* Example: "stromleser" */

    uint32_t    interval_minutes;
    uint32_t    total_readings;

    JsonValues  values;
} JsonGateway;

/*Public API */

/**
 * Serializes gateway data into a JSON string.
 *
 * The function is transport-agnostic and does not perform
 * any I/O operations. The caller is responsible for providing
 * a sufficiently large output buffer.
 *
 * @param gateway      Pointer to gateway data structure
 * @param output       Caller-provided character buffer
 * @param output_size  Size of output buffer in bytes
 *
 * @return  0  Success
 *         -1  Invalid input parameters
 *         -2  Output buffer too small
 *         -3  Formatting error
 */
int json_serialize_gateway(
    const JsonGateway *gateway,
    char *output,
    size_t output_size
);

/**
 * Returns the maximum possible JSON size (in bytes)
 * based on configured limits.
 *
 * This can be used by the caller to size the output buffer.
 */
size_t json_get_max_output_size(void);

#ifdef __cplusplus
}
#endif

#endif /* JSON_SERIALIZER_H */
