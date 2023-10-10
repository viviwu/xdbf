/**
* @program: libdbf
* @description: [...]
* @author: viviwu [Administrator]
* @create: 2022-05-24 11:32
**/
// // created by Administrator on 2022/5/24.

#include "dbf_writer.h"

#include <time.h>

//DbfWriter::DbfWriter(const char *filename, DB_FIELD *fields, int numfields):
DbfWriter::DbfWriter(const char *filename, int numfields):
        columns(numfields)
        ,reclen(0)
        ,cur_record(0)
{
    ofs_.open(filename, std::ofstream::out|std::ofstream::binary);
    
    if (NULL == (header = (DB_HEADER *) malloc(sizeof(DB_HEADER)))) {
        return;
    }
    memset(header, 0, sizeof(DB_HEADER));
    
    if (NULL == (fields = (DB_FIELD *)malloc(columns * SIZE_OF_DB_FIELD))) {
        return;
    }
    fieldptr = (char *) fields;
}

DbfWriter::~DbfWriter(){
    ofs_.close();
    free(header);
    free(fields);
}

int DbfWriter::SetHeaderField(const char type, const char name[11], int len, int decimals)
{
    DB_FIELD *field = (DB_FIELD *) fieldptr;
    memset(field, 0, sizeof(DB_FIELD));
    field->field_type = type;
    strncpy(field->field_name, name, 11);
    field->field_length = len;
    field->field_decimals = decimals;

    fieldptr += SIZE_OF_DB_FIELD;

    reclen += len;
    printf("reclen = %d \n", reclen);
    return 0;
}

int DbfWriter::WriteHeaderInfo(/*DB_HEADER *header*/){

    time_t ps_calendar_time = time(NULL);
    if (ps_calendar_time != (time_t) (-1)) {
        struct tm *ps_local_tm = localtime(&ps_calendar_time);
        header->last_update[0] = ps_local_tm->tm_year;
        header->last_update[1] = ps_local_tm->tm_mon + 1;
        header->last_update[2] = ps_local_tm->tm_mday;
    }

    reclen = 0;
    for (int i = 0; i < columns; i++) {
        reclen += fields[i].field_length;
    }

    header->version = FoxBasePlus;
    /* Add 1 to record length for deletion flog */
    header->record_length = rotate2b(reclen + 1);
    header->header_length = rotate2b(sizeof(DB_HEADER) + columns * sizeof(DB_FIELD) + 2);
    header->records_cnt = rotate4b(header->records_cnt);

    printf(" dbf version:%d\n record_length:%d\n header_length:%d\n records_cnt:%d\n",
           header->version, header->record_length, header->header_length, header->records_cnt);
    /* Make sure the header is written at the beginning of the file
	 * because this function is also called after each record has
	 * been written.
	 */
    ofs_.seekp(ios::beg);
    ofs_.write((char*)header, sizeof(DB_HEADER));

    return 0;
}

int DbfWriter::WriteFieldInfo(/*DB_FIELD *fields, int numfields*/){
    ofs_.seekp(sizeof(DB_HEADER));
    ofs_.write((char*)fields, columns * sizeof(DB_FIELD));
//    perror("In function dbf_WriteFieldInfo(): ");
    ofs_.write("\r\0", 2);
    return 0;
}

int DbfWriter::WriteRecord(char *record, int len) {
    if (len != header->record_length - 1) {
        fprintf(stderr, "Length of record mismatches expected length (%d != %d).", len, header->record_length - 1);
        fprintf(stderr, "\n");
        return -1;
    }
    ofs_.seekp(ios::end); //lseek(dbf_fh, 0, SEEK_END);
    ofs_.write(" ", 1);
    ofs_.write(record, header->record_length - 1);
    header->records_cnt++;
    printf("record-%d:%s\n", header->records_cnt, record);
    WriteHeaderInfo();

    cur_record++;
    return header->records_cnt;
}

int DbfWriter::SaveFile(){
    cur_record = 0;
    return 0;
}

int DbfWriter::RecordLength(){
    return reclen;
}

/* rotate2b() {{{
 * swap 4 byte integers
 */
unsigned short rotate2b(unsigned short var) {
    u_int16_t tmp;
    unsigned char *ptmp;
    tmp = var;
    ptmp = (unsigned char *) &tmp;
    return (((u_int16_t) ptmp[1] << 8) + (u_int16_t) ptmp[0]);
}
/* }}} */

/* rotate4b() {{{
 * swap 4 byte integers
 */
unsigned int rotate4b(unsigned int var) {
    u_int32_t tmp;
    unsigned char *ptmp;
    tmp = var;
    ptmp = (unsigned char *) &tmp;
    return (((u_int32_t) ptmp[3] << 24) + ((u_int32_t) ptmp[2] << 16) + ((u_int32_t) ptmp[1] << 8) +
            (u_int32_t) ptmp[0]);
}
/* }}} */

char *stroffset(char *dst, const char *src, size_t offset) {
//  ASSERT();
    if (strlen(dst) >= offset + strlen(src)) {
        for (int i = 0; i < strlen(src); ++i) {
            dst[offset + i] = src[i];
        }
    }
    return dst;
}