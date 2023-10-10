//
// Created by viviwu on 2023/5/9.
//

#ifndef DBF_TOOL_DBF_DOC_H
#define DBF_TOOL_DBF_DOC_H

#include "dbf_struct.h"

#pragma warning(disable:4018)
#pragma warning( disable : 4996)

class DbfDoc {
public:
  /*
 * Open the a dbf file and returns file handler
 */
  DbfDoc(const char* exist_file, bool read_only=true);

  DbfDoc(const char* create_file, DB_FIELD *fields, int numfields);

  ~DbfDoc();

/*
 *	FUNCTIONS
 */

/*! \fn GetVersion()
	\brief return the version of dbf file as a human readable string
	\param *p_dbf the object handle of the opened file
*/
  const char *GetDbfStringVersion();

/*! \fn GetVersion()
	\brief return the version of dbf file
	\param *p_dbf the object handle of the opened file

	\return version or -1 on error
*/
  int GetDbfVersion();

/*! \fn int DbfClose ()
	\brief DbfClose closes a dBASE file.
	\param *p_dbf the object handle of the opened file

	Closes a dBASE file and frees all memory.
	\return 0 if closing was successful and -1 if not.
*/
  int DbfClose();

// Functions to info about rows and columns
/*! \fn int DbfRowsNum()
	\brief dbf_NumRows returns the number of datasets/rows
	\param *p_dbf the object handle of the opened file

	Returns the number of datasets/rows.
	\return Number of rows or -1 in case of an error.
*/
  int DbfRowsNum();

/*! \fn int dbf_NumCols ()
	\brief dbf_NumCols returns the number of attributes/columns
	\param *p_dbf the object handle of the opened file

	Returns the number of fields/columns.
	\return Number of columns or -1 in case of an error.
*/
  int DbfColsNum();

/*! \fn const char *dbf_ColumnName(int column)
	\brief dbf_ColumnName returns the name of a selected \a column
	\param *p_dbf the object handle of the opened file
	\param column the number of the column

	Returns the name of a selected column.
	The first column has number 0. The maximum number of columns can
	be determined with \ref dbf_NumCols.
	\return Name of column or -1 on error
*/
  const char *DbfColumnName(int column);

/*! \fn int dbf_ColumnSize(int column);
	\brief dbf_ColumnSize returns the field length of a column
	\param *p_dbf the object handle of the opened file
	\param column the number of the column

	Returns the field length of a column.
	The first column has number 0. The maximum number of columns can
	be determined with \ref dbf_NumCols.
	\return field length of column or -1 on error
*/
  int DbfColumnSize(int column);

/*! \fn const char dbf_ColumnType(int column)
	\brief dbf_ColumnType returns the type of a field resp. column
	\param *p_dbf the object handle of the opened file
	\param column the number of the column

	Returns the type of a column. Type can be any
	of 'N' (number), 'C' (string), 'D' (data), 'M' (memo), 'L' (boolean)
	for dBASE III files.
	The first column has number 0. The maximum number of columns can
	be determined with \ref dbf_NumCols.
	\return field type of column or -1 on error
*/
  const char DbfColumnType(int column);

/*! \fn int dbf_ColumnDecimals(int column)
	\brief
	\param *p_dbf the object handle of the opened file
	\param column the number of the column

	Returns the number of digits right to the
	decimal point.
	The first column has number 0. The maximum number of columns can
	be determined with \ref dbf_NumCols.
	\return Number of decimals of column or -1 on error
*/
  int DbfColumnDecimals(int column);

/*! \fn u_int32_t DbfColumnAddress(int column)
	\brief DbfColumnAddress returns the address of a column
	\param *p_dbf the object handle of the opened file
	\param column the number of the column

	The first column has number 0. The maximum number of columns can
	be determined with \ref dbf_NumCols.
	\return Address of column or -1 on error
*/
  u_int32_t DbfColumnAddress(int column);

/*! \fn const char *GetDate()
	\brief GetDate returns a formatted date string
	\param *p_dbf the object handle of the opened file

	Returns a formatted date string of the type
	yyyy-mm-dd. The date indicates the time the file was last
	modified.

	\return formatted date or -1 on error
*/

  const char *GetDbfDate();


/*! \fn int DbfSetField(DB_FIELD *field, int type, const char *name, int len, int dec)
	\brief DbfSetField fills a field structure
	\param *field pointer to field which shall be set
	\param type type of field
	\param name name of field
	\param len length of field
	\param dec number of decimals

	Sets the field structure with the given values.

	\return 0 
*/
  int DbfSetField(DB_FIELD *field, int type, const char *name, int len, int dec);

/*! \fn int DbfRecordLength()
	\brief DbfRecordLength returns length of a dataset
	\param *p_dbf the object handle of the opened file

	Returns the length of a dataset. Do not confuse this
	with \ref DbfHeaderSize.

	\return record length or -1 on error
*/
  int DbfRecordLength();

/*! \fn int DbfHeaderSize()
	\brief DbfHeaderSize returns length of the header
	\param *p_dbf the object handle of the opened file

	Returns the length of the header.

	\return header length or -1 on error
*/
  int DbfHeaderSize();

/*! \fn int DbfSetRecordOffset(int offset)
	\brief DbfSetRecordOffset set the internal record counter
	\param *p_dbf the object handle of the opened file
	\param offset the new record number of the next record read

	Sets the internal record counter. The counter is an index counting
	through the records within a dBASE file. The first record has
	number 1. A negative offset will be counted from the end of the file.
	The last record has index -1.
	\ref ReadDbfRecordAtRow uses the counter when reading the
	next record. Setting a value outside the range -numrecord to numrecords
	or 0 will cause an error.

	\return the new internal record counter or -1 in case of an error
*/
  int DbfSetRecordOffset(int offset);

/*! \fn int ReadDbfRecordAtRow(char *record, int len)
	\brief ReadDbfRecordAtRow reads the current record
	\param *p_dbf the object handle of the opened file
	\param *record a memory block large enough to contain a record
	\param row the row number of the record block

	Returns a record as it stored in the dBASE file. The memory must
	be large enough and allocated by the calling application.

	\return 0 if successful, -1 on error
*/
  int ReadDbfRecordAtRow(char *record, int row);

  char *GetRecordData(char *record, int column);

  void GetRecordValue(char *record, int column, char *val);

/*! \fn int DbfWriteRecord(char *record, int len)
	\brief DbfWriteRecord writes a record
	\param *p_dbf the object handle of the opened file
	\param *record record data suitable for writing into the dBASE file
	\param len the length of the record block

	Writes a record into a dBASE file and returns the number of
	records. The record data must contain all field data but not
	the leading byte which indicates whether the record is deleted.
	Hence, len must be \ref DbfRecordLength() - 1.

	\return number of records written, -1 on error
*/
  int WriteDbfRecord(char *record, int len);

/*! \fn int DbfIsMemo()
	\brief DbfIsMemo tells if dbf provides also a memo file
	\param *p_dbf the object handle of the opened file

	DbfIsMemo indicates if dbf provides also a memo file

	\return 0 no memo, 1 memo, -1 on error
*/

  char * GetDbfRecordData(char *record, int column);

  void GetDbfRecordValue(char *record, int column, char *val);

  int DbfWriteRecordWithFlag(char *record, int len);

  int DbfIsMemo();

private:
/*
 * Reads header from file into struct
 */
  int ReadDbfHeaderInfo();

/*
 * Write header into file
 */
  int WriteDbfHeaderInfo(DB_HEADER *header);

/*
 * Sets p_dbf->fields to an array of DB_FIELD containing the specification
 * for all columns.
 */
  int ReadDbfFieldInfo();

/*
 * Writes the field specification into the output file
 */
  int WriteDbfFieldInfo(DB_FIELD *fields, int numfields);


private:
  /*! filehandler of *.dbf */
  int dbf_fh_;
  /*! header of .dbf file */
  DB_HEADER *header_;
  /*! array of field specification */
  DB_FIELD *fields_;
  /*! number of fields */
  u_int32_t columns_;
  /*! record counter */
  int cur_record_;

//  /*! filehandler of memo */
//  int dbt_fh_;
//  /*! the pysical size of the file, as stated from filesystem */
//  u_int32_t real_filesize_;
//  /*! the calculated filesize */
//  u_int32_t calc_filesize_;
//  /*! integrity could be: valid, invalid [7] */
//  char integrity_[7];
//  /*! errorhandler, maximum of 254 characters */
//  char  errmsg_[256];
};


#endif // DBF_TOOL_DBF_DOC_H
