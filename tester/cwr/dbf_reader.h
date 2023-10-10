/**
* @program: libdbf
* @description: [...]
* @author: viviwu [Administrator]
* @create: 2022-05-23 16:29
**/
// // created by Administrator on 2022/5/23.

#include <iostream>
#include <string>
#include <vector>

#include "../dbf_struct.h"

#ifndef LIBDBF_DBF_READER_H
#define LIBDBF_DBF_READER_H

typedef unsigned int            u_int32_t;

class DbfReader {
public:
    DbfReader(const std::string &filename, const char delims[] = ",");

    ~DbfReader();

    int NextRecord(void *record);

    int NextLine(char *line);

    int RecordLength();

    int ColumnsCount();


private:
    std::string delims_;
    FILE *fdbf_;
    std::vector<int> offset_;

    /*! header of .dbf file */
    DB_HEADER header_;
    /*! array of field specification */
    std::vector<DB_FIELD> columns_;
    /*! number of fields */
    u_int32_t columns_cnt_;
    /*! integrity could be: valid, invalid */
    unsigned char integrity[7];
    /*! record counter */
    int cur_record_;
    /*! errorhandler, maximum of 254 characters */
    char errmsg[254];

};


#endif //LIBDBF_DBF_READER_H
