/**
* @file utils.h
* @brief Common utility functions library
* @author wuxw
* @date 2020/5/15
*
* Provides date/time handling, filename generation and other common utilities
*/

#ifndef X_CSTR_UTILS_H
#define X_CSTR_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* Constants */
#define DATE_BUFFER_SIZE 9  // YYYYMMDD\0
#define TIMESTAMP_SEC_SIZE 15  // YYYYMMDDHHMMSS\0
#define TIMESTAMP_MSEC_SIZE 18  // YYYYMMDDHHMMSSsss\0

/* Error codes */
#define UTILS_SUCCESS 0
#define UTILS_ERROR_INVALID_ARG -1
#define UTILS_ERROR_BUFFER_SIZE -2

/**
 * @brief Get today's date string in YYYYMMDD format
 * @param[out] buffer Output buffer
 * @param[in] size Buffer size
 * @return int UTILS_SUCCESS on success, error code on failure
 */
int formatted_today_date(char* buffer, size_t size);

/**
 * @brief Generate fixed time string
 * @param[out] buffer Output buffer
 * @param[in] size Buffer size
 * @param[in] fixed_part Fixed time part to append
 * @return int UTILS_SUCCESS on success, error code on failure
 */
int today_fixed_time_string(char* buffer, size_t size, const char* fixed_part);

/**
 * @brief Generate current timestamp (second precision)
 * @param[out] buffer Output buffer
 * @param[in] size Buffer size
 * @return int UTILS_SUCCESS on success, error code on failure
 */
int formatted_current_timestamp_second(char* buffer, size_t size);

/**
 * @brief Generate current timestamp (millisecond precision)
 * @param[out] buffer Output buffer
 * @param[in] size Buffer size
 * @return int UTILS_SUCCESS on success, error code on failure
 */
int formatted_current_timestamp_millisecond(char* buffer, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* X_CSTR_UTILS_H */
