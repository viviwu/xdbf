/**
* @file utils.h
* @brief Common utility functions library
* @author wuxw
* @date 2020/5/15
*
* Provides date/time handling, filename generation and other common utilities
*/

#ifndef ATGO_DBF_UTILS_H
#define ATGO_DBF_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief Generate OrderAlgo filename
 * @param[out] buffer Output buffer
 * @param[in] size Buffer size
 * @return int UTILS_SUCCESS on success, error code on failure
 */
int today_order_algo_filename(char* buffer, size_t size);


#ifdef __cplusplus
}
#endif

#endif /* ATGO_DBF_UTILS_H */
