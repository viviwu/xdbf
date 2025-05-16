/**
 * @file utils.c
 * @brief Implementation of utility functions
 * @author wuxw
 * @date 2020/5/16
 */

#include "utils.h"

#include <stdio.h>
#include <sys/timeb.h>
#include <time.h>


int get_today_date(char* buffer, size_t size)
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

int generate_order_algo_filename(char* buffer, size_t size)
{
  if (!buffer || size < FILENAME_BUFFER_SIZE)
    return UTILS_ERROR_INVALID_ARG;

  char date[DATE_BUFFER_SIZE];
  int  ret = get_today_date(date, sizeof(date));
  if (ret != UTILS_SUCCESS)
    return ret;

  if (snprintf(buffer, size, "OrderAlgo_%s.dbf", date) >= size)
    return UTILS_ERROR_BUFFER_SIZE;

  return UTILS_SUCCESS;
}

int generate_fixed_time_string(char* buffer, size_t size, const char* fixed_part)
{
  if (!buffer || !fixed_part || size < TIMESTAMP_MSEC_SIZE)
    return UTILS_ERROR_INVALID_ARG;

  char date[DATE_BUFFER_SIZE];
  int  ret = get_today_date(date, sizeof(date));
  if (ret != UTILS_SUCCESS)
    return ret;

  if (snprintf(buffer, size, "%s%s", date, fixed_part) >= size)
    return UTILS_ERROR_BUFFER_SIZE;

  return UTILS_SUCCESS;
}

int generate_current_timestamp_second(char* buffer, size_t size)
{
  if (!buffer || size < TIMESTAMP_SEC_SIZE)
    return UTILS_ERROR_INVALID_ARG;

  time_t     now;
  struct tm* tm_info;

  time(&now);
  tm_info = localtime(&now);

  if (snprintf(buffer, size, "%04d%02d%02d%02d%02d%02d", tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday,
               tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec)
      >= size)
    return UTILS_ERROR_BUFFER_SIZE;

  return UTILS_SUCCESS;
}

int generate_current_timestamp_millisecond(char* buffer, size_t size)
{
  if (!buffer || size < TIMESTAMP_MSEC_SIZE)
    return UTILS_ERROR_INVALID_ARG;

  struct timeb tb;
  struct tm*   tm_info;
  time_t       now;

  time(&now);
  tm_info = localtime(&now);
  ftime(&tb);

  if (snprintf(buffer, size, "%04d%02d%02d%02d%02d%02d%03d", tm_info->tm_year + 1900, tm_info->tm_mon + 1,
               tm_info->tm_mday, tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec, tb.millitm)
      >= size)
    return UTILS_ERROR_BUFFER_SIZE;

  return UTILS_SUCCESS;
}
