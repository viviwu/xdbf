/**
 * @file utils.c
 * @brief Implementation of utility functions
 * @author wuxw
 * @date 2020/5/16
 */

#include "at_util.h"

#include <stdio.h>
#include <sys/timeb.h>
#include <time.h>

/* Constants */
#define DATE_BUFFER_SIZE 9  // YYYYMMDD\0
#define FILENAME_BUFFER_SIZE 24  // OrderAlgo_YYYYMMDD.dbf\0

/* Error codes */
#define UTILS_SUCCESS 0
#define UTILS_ERROR_INVALID_ARG -1
#define UTILS_ERROR_BUFFER_SIZE -2

int today_formatted_date(char* buffer, size_t size)
{
  if (!buffer || size < DATE_BUFFER_SIZE)
    return UTILS_ERROR_INVALID_ARG;

  time_t     rawtime;
  struct tm* timeinfo;

  time(&rawtime);
  timeinfo = localtime(&rawtime);

  if (strftime(buffer, size, "%Y%m%d", timeinfo) == 0)
    return UTILS_ERROR_BUFFER_SIZE;

  return UTILS_SUCCESS;
}

int today_order_algo_filename(char* buffer, size_t size)
{
  if (!buffer || size < FILENAME_BUFFER_SIZE)
    return UTILS_ERROR_INVALID_ARG;

  char date[DATE_BUFFER_SIZE];
  int  ret = today_formatted_date(date, sizeof(date));
  if (ret != UTILS_SUCCESS)
    return ret;

  if (snprintf(buffer, size, "OrderAlgo_%s.dbf", date) >= size)
    return UTILS_ERROR_BUFFER_SIZE;

  return UTILS_SUCCESS;
}
