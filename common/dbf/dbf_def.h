/**
 * @program: atgo_dbf
 * @description: [...]
 * @author: wuxw
 * @create: 2022-05-23 16:31
 **/

/******************************************************************/

#ifndef DBF_STRUCT_H
#define DBF_STRUCT_H

#include <sys/stat.h>
#include <sys/types.h>

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
    #include <unistd.h>
    // refers to the urtc/fcntl.h definition in Windows Kit:
    #define O_BINARY 0x8000  // file mode is binary (untranslated)

#elif defined(_WIN32)
    #include <io.h>
    #include <windows.h>
    #ifndef __ANUBISNET_TYPES__
        #define __ANUBISNET_TYPES__
typedef UINT32 u_int32_t;
// Windows does not know UINT16 types
typedef unsigned short u_int16_t;
    #endif

#elif defined(__MSDOS__)
    #include <io.h>

#else
    #include <unistd.h>
#endif

#pragma warning(disable : 4018)
#pragma warning(disable : 4996)

/**
 * @brief Structure representing the fixed portion of the DBF file header.
 *
 * The DBF file header starts at byte 0 and contains metadata about the file,
 * such as the file type, number of records, and the position where data records begin.
 * All multi-byte integers are stored in little-endian format.
 */
#pragma pack(push, 1)
typedef struct {
    uint8_t file_type;  // Byte 0: DBF file type (e.g., 0x03 for FoxBASE+/dBASE III Plus, no memo)
    uint8_t last_update[3];   // Bytes 1-3: Last update in YYMMDD format
    uint32_t num_records;     // Bytes 4-7: Number of records in the file (little-endian)
    uint16_t pos_first_data;  // Bytes 8-9: Byte offset of the first data record (little-endian)
    uint16_t record_length;   // Bytes 10-11: Length of one data record, including delete flag
                              // (little-endian)
    uint8_t reserved1[16];    // Bytes 12-27: Reserved bytes
    uint8_t table_flags;  // Byte 28: Table flags (bitwise: 0x01 = has .cdx, 0x02 = has Memo, 0x04 =
                          // is .dbc)
    uint8_t code_page_mark;  // Byte 29: Code page mark
    uint8_t reserved2[2];    // Bytes 30-31: Reserved, contains 0x00
} DBFHeader;
#pragma pack(pop)

/**
 * @brief Structure representing a field subrecord in the DBF header.
 *
 * Field subrecords start at byte 32 of the header and define each field in the table.
 * There is one subrecord per field, each 32 bytes long, continuing until the header
 * terminator (0x0D). The number of fields can be determined by reading until the terminator.
 */
#pragma pack(push, 1)
typedef struct {
    char field_name[11];     // Bytes 0-10: Field name, up to 10 chars, padded with nulls (0x00)
    char field_type;         // Byte 11: Field type (e.g., 'C' for Character, 'N' for Numeric)
    uint32_t displacement;   // Bytes 12-15: Displacement of field in record (little-endian)
    uint8_t field_length;    // Byte 16: Length of the field in bytes
    uint8_t decimal_places;  // Byte 17: Number of decimal places (for numeric fields)
    uint8_t field_flags;    // Byte 18: Field flags (bitwise: 0x01 = system, 0x02 = nullable, 0x04 =
                            // binary)
    uint32_t autoinc_next;  // Bytes 19-22: Next autoincrement value (little-endian)
    uint8_t autoinc_step;   // Byte 23: Autoincrement step value
    uint8_t reserved[8];    // Bytes 24-31: Reserved bytes
} DBFField;
#pragma pack(pop)

// **File Type Constants**
#define DBF_FILE_TYPE_FOXBASE 0x02                 // FoxBASE
#define DBF_FILE_TYPE_FOXBASE_PLUS_DBASE_III 0x03  // FoxBASE+/dBASE III Plus, no memo
#define DBF_FILE_TYPE_VISUAL_FOXPRO 0x30           // Visual FoxPro
#define DBF_FILE_TYPE_VISUAL_FOXPRO_AUTOINC 0x31   // Visual FoxPro with autoincrement
#define DBF_FILE_TYPE_VISUAL_FOXPRO_VAR 0x32       // Visual FoxPro with Varchar/Varbinary
#define DBF_FILE_TYPE_DBASE_IV_SQL 0x43            // dBASE IV SQL table, no memo
#define DBF_FILE_TYPE_DBASE_IV_SQL_SYSTEM 0x63     // dBASE IV SQL system, no memo
#define DBF_FILE_TYPE_FOXBASE_PLUS_MEMO 0x83       // FoxBASE+/dBASE III Plus with memo
#define DBF_FILE_TYPE_DBASE_IV_MEMO 0x8B           // dBASE IV with memo
#define DBF_FILE_TYPE_DBASE_IV_SQL_MEMO 0xCB       // dBASE IV SQL table with memo
#define DBF_FILE_TYPE_FOXPRO_2X_MEMO 0xF5          // FoxPro 2.x (or earlier) with memo
#define DBF_FILE_TYPE_HIPER_SIX_MEMO 0xE5          // HiPer-Six with SMT memo
#define DBF_FILE_TYPE_FOXBASE_ALT 0xFB             // FoxBASE (alternative)

// **Table Flags (bitwise)**
#define DBF_TABLE_FLAG_HAS_CDX 0x01   // File has a structural .cdx
#define DBF_TABLE_FLAG_HAS_MEMO 0x02  // File has a Memo field
#define DBF_TABLE_FLAG_IS_DBC 0x04    // File is a database (.dbc)

// **Field Type Constants**
#define DBF_FIELD_TYPE_CHARACTER 'C'  // Character
#define DBF_FIELD_TYPE_CURRENCY 'Y'   // Currency (Visual FoxPro)
#define DBF_FIELD_TYPE_NUMERIC 'N'    // Numeric
#define DBF_FIELD_TYPE_FLOAT 'F'      // Float
#define DBF_FIELD_TYPE_DATE 'D'       // Date
#define DBF_FIELD_TYPE_DATETIME 'T'   // DateTime (Visual FoxPro)
#define DBF_FIELD_TYPE_DOUBLE 'B'     // Double (Visual FoxPro)
#define DBF_FIELD_TYPE_INTEGER 'I'    // Integer (Visual FoxPro)
#define DBF_FIELD_TYPE_LOGICAL 'L'    // Logical
#define DBF_FIELD_TYPE_MEMO 'M'       // Memo
#define DBF_FIELD_TYPE_GENERAL 'G'    // General
#define DBF_FIELD_TYPE_PICTURE 'P'    // Picture
#define DBF_FIELD_TYPE_VARCHAR 'V'    // Varchar (Visual FoxPro)
#define DBF_FIELD_TYPE_AUTOINC '+'    // Autoincrement (dBase Level 7)
#define DBF_FIELD_TYPE_DOUBLE_D7 'O'  // Double (dBase Level 7)
#define DBF_FIELD_TYPE_TIMESTAMP '@'  // Timestamp (dBase Level 7)

// **Field Flags (bitwise)**
#define DBF_FIELD_FLAG_SYSTEM 0x01       // System column (not visible to user)
#define DBF_FIELD_FLAG_CAN_NULL 0x02     // Column can store null values
#define DBF_FIELD_FLAG_BINARY 0x04       // Binary column (for CHAR and MEMO)
#define DBF_FIELD_FLAG_NULL_BINARY 0x06  // NULL and binary (0x02 + 0x04)
#define DBF_FIELD_FLAG_AUTOINC 0x0C      // Column is autoincrementing

// **Record Delete Flag Constants**
#define DBF_RECORD_NOT_DELETED 0x20  // Space: Record is not deleted
#define DBF_RECORD_DELETED 0x2A      // Asterisk: Record is deleted

#define SIZE_OF_DB_FIELD 32  // sizeof(DB_FIELD)
#define SIZE_OF_DB_HEAD 32   // sizeof(DB_HEADER)

#define RECORD_TERMINATOR 0x0D
#define SIZE_OF_RECORD_TERMINATOR 1

#define SIZE_OF_DELETE_FLAG 1

/* Memo File Structure (.FPT)
 * Memo files contain one header record and any number of block structures.
 * The header record contains a pointer to the next free block and the size
 * of the block in bytes. The size is determined by the SET BLOCKSIZE command
 * when the file is created. The header record starts at file position zero and
 * occupies 512 bytes. The SET BLOCKSIZE TO 0 command sets the block size width to 1.
 */
/* Following the header record are the blocks that contain a block header and
 * the text of the memo. The table file contains block numbers that are used to
 * reference the memo blocks. The position of the block in the memo file is
 * determined by multiplying the block number by the block size (found in the
 * memo file header record). All memo blocks start at even block boundary
 * addresses. A memo block can occupy more than one consecutive block.
 */

#pragma pack(push, 1)
/* Memo Header Record */
struct DB_MEMO_HEADER {
    /* 00  03	Location of next free block [1] */
    unsigned int block_adress;
    /* 04  05	Unused */
    unsigned char reserved[2];
    /* 06  07	Block size (bytes per block) [1] */
    unsigned short block_size;
    /* 08  511	Unused */
    unsigned char reserved2[504];
};

/* [1] Integers stored with the most significant byte first. See: endian.h   */

/* Memo Block Header and Memo Text */
struct DB_MEMO_BLOCK_TOP {
    /* 00  03			Block signature [1]  */
    /*	(indicates the type of data in the block)
     *	0  picture (picture field type)
     *	1  text (memo field type) */
    unsigned int signature;
    /* 04  07	Length [1] of memo (in bytes) */
    unsigned int block_length;
    /* 08 -  n	Memo text (n = length) */
};

/* [1] Integers stored with the most
significant byte first.    */

#pragma pack(pop)

// u_int16_t rotate2b(u_int16_t var);

// u_int32_t rotate4b(u_int32_t var);

#endif  // DBF_STRUCT_H
