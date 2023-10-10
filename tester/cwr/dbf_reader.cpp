/**
* @program: libdbf
* @description: [...]
* @author: viviwu [Administrator]
* @create: 2022-05-23 16:29
**/
// // created by Administrator on 2022/5/23.

#include "dbf_reader.h"

DbfReader::DbfReader(const std::string &filename, const char delims[]) :
        delims_(delims),
        columns_cnt_(0),
        cur_record_(0),
        columns_(0),
        offset_(0) {

    if ((fdbf_ = fopen(filename.c_str(), "rb")) == NULL) {
        fprintf(stderr, "can not fopen file [%s]! \n", filename.c_str());
        return;
    }
    fseek(fdbf_, 0L, SEEK_SET);
    fread(&header_, SIZE_OF_DB_HEAD, 1, fdbf_);
    columns_cnt_ = (header_.header_length - SIZE_OF_DB_HEAD - SIZE_OF_RECORD_TERMINATOR) / SIZE_OF_DB_FIELD;
    printf(" dbf version:%d\n record_length:%d\n records_cnt:%d\n header_length:%d\n columns_cnt_:%d\n",
           header_.version,
           header_.record_length,
           header_.records_cnt,
           header_.header_length,
           columns_cnt_
           );

    columns_.resize(columns_cnt_);
    offset_.resize(columns_cnt_);

    fseek(fdbf_, SIZE_OF_DB_HEAD, SEEK_SET);

    int n = 0;
    for (int i = 0; i < columns_cnt_; i++) {
        offset_[i] = n;
        fread(&columns_[i], SIZE_OF_DB_FIELD, 1, fdbf_);
        n = n + columns_[i].field_length;
        printf("%d name:%s type:%c offset:%d width:%d decimal:%d\n",
               i, columns_[i].field_name, columns_[i].field_type, columns_[i].field_address,
               columns_[i].field_length, columns_[i].field_decimals);
    }

}


int DbfReader::NextRecord(void *record) {
    if (cur_record_ >= header_.records_cnt)
        return -1;
    /*off_t offset_ = */
    fseek(fdbf_, header_.header_length + cur_record_ * (header_.record_length) + SZIE_OF_DELETE_FLAG, SEEK_SET);
    if (fread(record, header_.record_length - SZIE_OF_DELETE_FLAG, 1, fdbf_) == -1) {
        return -1;
    }
    cur_record_++;
    return cur_record_;
}

int DbfReader::NextLine(char *line) {
    if (cur_record_ >= header_.records_cnt)
        return -1;

    for (int j = 0; j < columns_cnt_; j++) {
        char *buf = (char *) malloc(columns_[j].field_length + 1);
        memset(buf, '0', columns_[j].field_length + 1);
        fseek(fdbf_, (long) (header_.header_length + SZIE_OF_DELETE_FLAG + cur_record_ * (long) header_.record_length + offset_[j]), SEEK_SET);
        fread(buf, columns_[j].field_length, 1, fdbf_);
        buf[columns_[j].field_length] = '\0';
        strcat(line, buf);
        if (j != columns_cnt_ - 1)
            strcat(line, delims_.c_str());
        free(buf);
    }
    cur_record_++;
    return cur_record_;
}

int DbfReader::RecordLength() {
    return header_.record_length;
}

int DbfReader::ColumnsCount() {
    return columns_cnt_;
}

DbfReader::~DbfReader() {
    if (fdbf_ != NULL) {
        columns_.clear();
        offset_.clear();
        fclose(fdbf_);
    }
}
