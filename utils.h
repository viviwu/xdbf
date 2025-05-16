//
// Created by wuxw on 2025/5/15/015.
//

#ifndef C_UTILS_H
#define C_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/timeb.h>  // ����ftime����
#include <time.h>

// ��ȡ���������ַ��� YYYYMMDD
void get_today_date(char* buffer, size_t size)
{
  if (!buffer || size < 9)
    return;
  time_t     rawtime;
  struct tm* timeinfo;
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(buffer, size, "%Y%m%d", timeinfo);
}

// ����OrderAlgo_YYYYMMDD.dbf��ʽ���ļ���
void generate_order_algo_filename(char* buffer, size_t size)
{
  if (!buffer || size < 24)
    return;
  char date[9];
  get_today_date(date, sizeof(date));
  sprintf(buffer, "OrderAlgo_%s.dbf", date);
}

// ����YYYYMMDDsshhmmSSS��ʽ�Ĺ̶�ʱ���ַ���
void generate_fixed_time_string(char* buffer, size_t size, const char* fixed_part)
{
  if (!buffer || size < 18)
    return;
  char date[9];
  get_today_date(date, sizeof(date));
  sprintf(buffer, "%s%s", date, fixed_part);
}

// ���ɵ�ǰʱ�������ȷ���룩YYYYMMDDHHMMSS
void generate_current_timestamp_second(char* buffer, size_t size)
{
  if (!buffer || size < 15)
    return;
  time_t     now;
  struct tm* tm_info;

  time(&now);
  tm_info = localtime(&now);

  sprintf(buffer, "%04d%02d%02d%02d%02d%02d", tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday,
          tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);
}

// ���ɵ�ǰʱ�������ȷ�����룩YYYYMMDDHHMMSSsss
void generate_current_timestamp_millisecond(char* buffer, size_t size)
{
  if (!buffer || size < 18)
    return;
  struct timeb tb;
  struct tm*   tm_info;
  time_t       now;

  time(&now);
  tm_info = localtime(&now);
  ftime(&tb);

  sprintf(buffer, "%04d%02d%02d%02d%02d%02d%03d", tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday,
          tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec, tb.millitm);

  // ������루ע�⣺���ַ���������CLOCKS_PER_SEC�����ܲ��Ǻܾ�ȷ��
  //  unsigned int milliseconds;
  //  clock_now = clock();
  //  milliseconds = (clock_now % CLOCKS_PER_SEC) * 1000 / CLOCKS_PER_SEC;
}

#endif  //C_UTILS_H
