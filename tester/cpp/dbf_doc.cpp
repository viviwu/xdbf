//
// Created by viviwu on 2023/5/9.
//

#include "dbf_doc.h"
/****************************************************/
#ifdef ENABLE_NLS
#include <libintl.h>
#define _(a) dgettext(GETTEXT_PACKAGE, a)
#else
#define _(a) a
#endif

#include <assert.h>
#include <cstdio>   /// #include <stdio.h>
#include <cstdlib>  /// #include <stdlib.h>
#include <cstring>  /// #include <string.h>
#include <climits>  /// #include <limits.h>
#include <ctime>    /// #include <time.h>
#include <fcntl.h>
/****************************************************/
using namespace std;
/****************************************************/

/*
 * Convert version field of header into human readable string.
 */
const char *get_db_version(int version) {
    static char name[31];

    switch (version) {
        case 0x02:
            // without memo fields
            return "FoxBase";
        case 0x03:
            // without memo fields
            return "FoxBase+/dBASE III+";
        case 0x04:
            // without memo fields
            return "dBASE IV";
        case 0x05:
            // without memo fields
            return "dBASE 5.0";
        case 0x83:
            return "FoxBase+/dBASE III+";
        case 0x8B:
            return "dBASE IV";
        case 0x30:
            // without memo fields
            return "Visual FoxPro";
        case 0xF5:
            // with memo fields
            return "FoxPro 2.0";
        default:
            sprintf(name, _("Unknown (code 0x%.2X)"), version);
            return name;
    }
}


/*
 * Open the a dbf file and returns file handler
 */
DbfDoc::DbfDoc(const char *file, bool read_only) :
    dbf_fh_(0), header_(0), fields_(0), columns_(0), cur_record_(0) {
    if (file[0] == '-' && file[1] == '\0') {
        dbf_fh_ = fileno(stdin);
    } else {
        if (read_only) {
            dbf_fh_ = open(file, O_RDONLY | O_BINARY);
        } else {
            dbf_fh_ = open(file, O_RDWR | O_BINARY); // O_RDWR|O_APPEND
        }
        if (-1 == dbf_fh_) {
            printf("dbf open  failed!");
            return;
        }
    }

    header_ = NULL;
    if (0 > ReadDbfHeaderInfo()) {
        return /* NULL */;
    }

    fields_ = NULL;
    if (0 > ReadDbfFieldInfo()) {
        free(header_);
        return /* NULL */;
    }

    cur_record_ = 0;
}


/*
 * Create a new dbf file and returns file handler
 */
DbfDoc::DbfDoc(const char *file, DB_FIELD *fields, int numfields) :
    dbf_fh_(0), header_(0), fields_(0), columns_(0), cur_record_(0) {
    int fh;

    if (file[0] == '-' && file[1] == '\0') {
        fh = fileno(stdout);
    } else if ((fh = open(file, O_CREAT | O_WRONLY | O_BINARY)) == -1) {
        return /* NULL */;
    }
    dbf_fh_ = fh;

    DB_HEADER *header;
    int reclen, i;

    if (NULL == (header = (DB_HEADER *) malloc(sizeof(DB_HEADER)))) {
        return /* NULL */;
    }
    reclen = 0;
    for (i = 0; i < numfields; i++) {
        reclen += fields[i].field_length;
    }
    memset(header, 0, sizeof(DB_HEADER));
    header->version = FoxBasePlus;
    /* Add 1 to record length for deletion flog */
    header->record_length = reclen + 1;
    header->header_length = sizeof(DB_HEADER) + numfields * sizeof(DB_FIELD) + 2;
    if (0 > WriteDbfHeaderInfo(header)) {

        return /* NULL */;
    }
    header_ = header;

    if (0 > WriteDbfFieldInfo(fields, numfields)) {
        // free(header_);

        return /* NULL */;
    }
    fields_ = fields;

    cur_record_ = 0;
}


DbfDoc::~DbfDoc() {
    DbfClose();
    printf("%s\n", __func__);
}

/*
 * Close the current open dbf file and free all memory
 */
int DbfDoc::DbfClose() {
    if (header_)
        free(header_);

    if (fields_)
        free(fields_);

    if (dbf_fh_ == fileno(stdin))
        return 0;

    if ((close(dbf_fh_)) == -1) {
        return -1;
    }

//  free(p_dbf_);

    return 0;
}


/*
 * Reads header from file into struct
 */
int DbfDoc::ReadDbfHeaderInfo() {
    DB_HEADER *header;
    if (NULL == (header = (DB_HEADER *) malloc(sizeof(DB_HEADER)))) {
        return -1;
    }

    if ((read(dbf_fh_, header, sizeof(DB_HEADER))) == -1) {
        printf("%s error!\n", __func__);
        return -1;
    }
    printf("version:%d \n", header->version);
    printf("header_length=%d, record_length=%d, records=%d \n", header->header_length, header->record_length,
           header->records);
    /* Endian Swapping */
    header->header_length = rotate2b(header->header_length);
    header->record_length = rotate2b(header->record_length);
    header->records = rotate4b(header->records);
    header_ = header;

    return 0;
}


/* 
 * Write header into file
 */
int DbfDoc::WriteDbfHeaderInfo(DB_HEADER *header) {
    time_t ps_calendar_time;
    struct tm *ps_local_tm;

    DB_HEADER *newheader = (DB_HEADER *) malloc(sizeof(DB_HEADER));
    if (NULL == newheader) {
        return -1;
    }
    memcpy(newheader, header, sizeof(DB_HEADER));

    ps_calendar_time = time(NULL);
    if (ps_calendar_time != (time_t) (-1)) {
        ps_local_tm = localtime(&ps_calendar_time);
        newheader->last_update[0] = ps_local_tm->tm_year;
        newheader->last_update[1] = ps_local_tm->tm_mon + 1;
        newheader->last_update[2] = ps_local_tm->tm_mday;
    }

    newheader->header_length = rotate2b(newheader->header_length);
    newheader->record_length = rotate2b(newheader->record_length);
    newheader->records = rotate4b(newheader->records);

    /* Make sure the header is written at the beginning of the file
     * because this function is also called after each record has
     * been written.
     */
    lseek(dbf_fh_, 0, SEEK_SET);
    if ((write(dbf_fh_, newheader, sizeof(DB_HEADER))) == -1) {
        free(newheader);
        return -1;
    }
    free(newheader);
    return 0;
}


/*  
 * Sets fields_ to an array of DB_FIELD containing the specification
 * for all columns.
 */
int DbfDoc::ReadDbfFieldInfo() {
    int columns, i, offset;
    DB_FIELD *fields;

    columns = DbfColsNum();

    if (NULL == (fields = (DB_FIELD *) malloc(columns * sizeof(DB_FIELD)))) {
        return -1;
    }

    lseek(dbf_fh_, sizeof(DB_HEADER), SEEK_SET);

    if ((read(dbf_fh_, fields, columns * sizeof(DB_FIELD))) == -1) {
        perror(_("In function ReadDbfFieldInfo(): "));
        return -1;
    }
    fields_ = fields;
    columns_ = columns;
    /* The first byte of a record indicates whether it is deleted or not. */
    offset = 1;
    for (i = 0; i < columns; i++) {
        fields[i].field_offset = offset;
        offset += fields[i].field_length;
    }

    return 0;
}


/*
 * Writes the field specification into the output file
 */
int DbfDoc::WriteDbfFieldInfo(DB_FIELD *fields, int numfields) {
    lseek(dbf_fh_, sizeof(DB_HEADER), SEEK_SET);

    if ((write(dbf_fh_, fields, numfields * sizeof(DB_FIELD))) == -1) {
        perror(_("In function WriteDbfFieldInfo(): "));
        return -1;
    }

    write(dbf_fh_, "\r\0", 2);

    return 0;
}

/******************************************************************************
	Block with functions to get information about the amount of
		- rows and
		- columns
 ******************************************************************************/

/*
 * Returns the number of records.
 */
int DbfDoc::DbfRowsNum() {
    if (header_->records >= 0) {
        return header_->records;
    } else {
        perror(_("In function dbf_NumRows(): "));
        return -1;
    }

    return 0;
}


/* DbfRowsNum() {{{
 * Returns the number of fields.
 */
int DbfDoc::DbfColsNum() {
    if (header_->header_length > 0) {
        // TODO: Backlink muss noch eingerechnet werden
        return ((header_->header_length - sizeof(DB_HEADER) - 1)
                / sizeof(DB_FIELD));
    } else {
        perror(_("In function dbf_NumCols(): "));
        return -1;
    }

    return 0;
}


/******************************************************************************
	Block with functions to get/set information about the columns
 ******************************************************************************/

/* dbf_ColumnName() {{{
 * Returns the name of a column. Column names cannot be longer than
 * 11 characters.
 */
const char *DbfDoc::DbfColumnName(int column) {
    if (column >= columns_) {
        return "invalid";
    }

    return fields_[column].field_name;
}


int DbfDoc::DbfColumnSize(int column) {
    if (column >= columns_) {
        return -1;
    }

    return (int) fields_[column].field_length;
}


const char DbfDoc::DbfColumnType(int column) {
    if (column >= columns_) {
        return -1;
    }

    return fields_[column].field_type;
}


int DbfDoc::DbfColumnDecimals(int column) {
    if (column >= columns_) {
        return -1;
    }

    return fields_[column].field_decimals;
}

u_int32_t DbfDoc::DbfColumnAddress(int column) {
    if (column >= columns_) {
        return -1;
    }

    return fields_[column].field_address;
}

int DbfSetField(DB_FIELD *field, int type, const char *name, int len, int dec) {
    memset(field, 0, sizeof(DB_FIELD));
    field->field_type = type;
    strncpy(field->field_name, name, 11);
    field->field_length = len;
    field->field_decimals = dec;
    return 0;
}


/******************************************************************************
	Block with functions to read out special dBASE information, like
		- date
		- usage of memo
 ******************************************************************************/

/*
 * Returns the date of last modification as a human readable string.
 */
const char *DbfDoc::GetDbfDate() {
    static char date[11];

    if (header_->last_update[0]) {
        sprintf(date, "%d-%02d-%02d",
                1900 + header_->last_update[0],
                header_->last_update[1],
                header_->last_update[2]);

        return date;
    } else {
        perror("In function GetDate(): ");
        return "";
    }

    return 0;
}


/* DbfHeaderSize() {{{
 */
int DbfDoc::DbfHeaderSize() {
    if (header_->header_length > 0) {
        return header_->header_length;
    } else {
        perror(_("In function DbfHeaderSize(): "));
        return -1;
    }

    return 0;
}


/*  
 * Returns the length of a record.
 */
int DbfDoc::DbfRecordLength() {
    if (header_->record_length > 0) {
        return header_->record_length;
    } else {
        perror(_("In function DbfRecordLength(): "));
        return -1;
    }

    return 0;
}

/*
 * Returns the verion of the dbase file as a human readable string.
 */
const char *DbfDoc::GetDbfStringVersion() {
    if (header_->version == 0) {
        perror(_("In function dbf_GetStringVersion(): "));
        return (char *) -1;
    }
    return get_db_version(header_->version);
}

/*  
 * Returns the verion field as it is storedi in the header.
 */
int DbfDoc::GetDbfVersion() {
    if (header_->version == 0) {
        perror(_("In function GetDbfVersion(): "));
        return -1;
    }

    return header_->version;
}

/* 
 */
int DbfDoc::DbfIsMemo() {
    int memo;

    if (header_->version == 0) {
        perror(_("In function DbfIsMemo(): "));
        return -1;
    }

    memo = (header_->version & 128) == 128 ? 1 : 0;

    return memo;
}


/******************************************************************************
	Block with functions to read records
 ******************************************************************************/

/* DbfSetRecordOffset() {{{
 */
int DbfDoc::DbfSetRecordOffset(int offset) {
    if (offset == 0)
        return -3;
    if (offset > (int) header_->records)
        return -1;
    if ((offset < 0) && (abs(offset) > header_->records))
        return -2;
    if (offset < 0)
        cur_record_ = (int) header_->records + offset;
    else
        cur_record_ = offset - 1;
    return cur_record_;
}


/* ReadDbfRecordAtRow() {{{
 */
int DbfDoc::ReadDbfRecordAtRow(char *record, int row) {
    if (row >= header_->records)
        return -1;
    cur_record_ = row;
    lseek(dbf_fh_, header_->header_length + cur_record_ * (header_->record_length), SEEK_SET);
    if (read(dbf_fh_, record, header_->record_length) == -1) {
        return -1;
    }
    cur_record_++;
    return cur_record_ - 1;
}


/* DbfWriteRecord() {{{
 */
int DbfDoc::WriteDbfRecord(char *record, int len) {

    if (len != header_->record_length - 1) {
        fprintf(stderr, _("Length of record mismatches expected length (%d != %d)."), len, header_->record_length - 1);
        fprintf(stderr, "\n");
        return -1;
    }
    lseek(dbf_fh_, 0, SEEK_END);
    if (write(dbf_fh_, UN_DELETE_FLAG, 1) == -1) {
        return -1;
    }
    if (write(dbf_fh_, record, header_->record_length - 1) == -1) {
        return -1;
    }

    header_->records++;
    if (0 > WriteDbfHeaderInfo(header_)) {
        return -1;
    }

    return header_->records;
}


int DbfDoc::DbfWriteRecordWithFlag(char *record, int len) {

    if (len != header_->record_length) {
        fprintf(stderr, _("Length of record mismatches expected length (%d != %d)."), len,
                header_->record_length);
        fprintf(stderr, "\n");
        return -1;
    }
    lseek(dbf_fh_, 0, SEEK_END);
    if (write(dbf_fh_, record, header_->record_length) == -1) {
        return -1;
    }
    header_->records++;
    if (0 > WriteDbfHeaderInfo(header_)) {
        return -1;
    }
    return header_->records;
}

/*
 * Useless function
 */
char *DbfDoc::GetDbfRecordData(char *record, int column) {
    return (record + fields_[column].field_offset);
}

void DbfDoc::GetDbfRecordValue(char *record, int column, char *val) {
    strncpy(val, record + fields_[column].field_offset, DbfColumnSize(column));
}

/***************************************/

/* rotate2b() {{{
 * swap 4 byte integers
 */
u_int16_t rotate2b(u_int16_t var) {
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
u_int32_t rotate4b(u_int32_t var) {
    u_int32_t tmp;
    unsigned char *ptmp;
    tmp = var;
    ptmp = (unsigned char *) &tmp;
    return (((u_int32_t) ptmp[3] << 24) + ((u_int32_t) ptmp[2] << 16) + ((u_int32_t) ptmp[1] << 8) +
            (u_int32_t) ptmp[0]);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
