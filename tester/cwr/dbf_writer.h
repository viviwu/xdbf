/**
* @program: libdbf
* @description: [...]
* @author: viviwu [Administrator]
* @create: 2022-05-24 11:32
**/
// // created by Administrator on 2022/5/24.

#include "../dbf_struct.h"
#include <fstream>
#include <string>
#include <vector>

#ifndef LIBDBF_DBF_WRITER_H
#define LIBDBF_DBF_WRITER_H


class DbfWriter {
public:
//    DbfWriter(const char *filename, DB_FIELD *fields, int numfields);
    DbfWriter(const char *filename, int numfields);
    ~DbfWriter();

    int SetHeaderField(const char type, const char name[11], int len, int decimals);

    int WriteHeaderInfo(/*DB_HEADER *header*/);

    int WriteFieldInfo(/*DB_FIELD *fields, int numfields*/);

    int WriteRecord(char *record, int len);

    int SaveFile();

    int RecordLength();

private:
    /*! filehandler of *.dbf */
    std::ofstream ofs_;       ///< Ofstream
    /*! header of .dbf file */
    DB_HEADER *header;
    /*! array of field specification */
    DB_FIELD *fields;
    char *fieldptr;
    /*! number of fields */
    u_int32_t columns;
    /*! record counter */
    int cur_record;
    int reclen;
};


/*
 * F U N C T I O N S
 */
unsigned short rotate2b(unsigned short var);

unsigned int rotate4b(unsigned int var);

char *stroffset(char *dst, const char *src, size_t offset);

#endif //LIBDBF_DBF_WRITER_H
