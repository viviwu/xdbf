/**
* @program: atgo_dbf
* @description: [...]
* @author: viviwu [Administrator]
* @create: 2022-05-23 16:31
**/

#include <sys/types.h>
#include <sys/stat.h>

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
  #include <unistd.h>
  // refers to the urtc/fcntl.h definition in Windows Kit:
  #define O_BINARY      0x8000  // file mode is binary (untranslated)

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
/******************************************************************/

#ifndef DBF_STRUCT_H
#define DBF_STRUCT_H

/*! \def FoxBase Code for FoxBase */
#define FoxBase 0x02
/*! \def FoxBasePlus Code for FoxBasePlus, same as for dBase III */
#define FoxBasePlus 0x03
/*! \def dBase3 Code for dBase III */
#define dBase3 0x03
/*! \def dBase3WM Code for dBase III with memo fields */
#define dBase3WM 0x83
/*! \def dBase4 Code for dBase IV */
#define dBase4 0x04
/*! \def dBase4WM Code for dBase IV with memo fields */
#define dBase4WM 0x8B
/*! \def dBase4SQL Code for dBase IV with SQL table */
#define dBase4SQL 0x8E
/*! \def dBase5 Code for dBase 5.0 */
#define dBase5 0x05
/*! \def FoxPro2WM Code for FoxPro 2.0 (or earlier) with memo fields */
#define FoxPro2WM 0xF5
/*! \def VisualFoxPro Code for Visual FoxPro without memo fields */
#define VisualFoxPro 0x30


#pragma warning(disable:4018)
#pragma warning( disable : 4996)

#pragma pack(push, 1)


/*! \struct DB_HEADER
	\brief table file header
 	 Standard dBASE Header
	 Offsets of this header are the same in all versions of
	 dBASE except dBASE 7.0
	\warning It is recommend not to access DB_HEADER directly.
 */
typedef struct InfoOfDbfHead {
/*! Byte: 0; dBase version */
    unsigned char version;
/*! Byte: 1-3; date of last update */
    unsigned char last_update[3];
/*! Byte: 4-7; number of records in table */
    u_int32_t records;
/*! Byte: 8-9; number of bytes in the header */
    u_int16_t header_length;
/*! Byte: 10-11; number of bytes in the record */
    u_int16_t record_length;
/*! Byte: 12-13; reserved, see specification of dBase databases */
    unsigned char reserved01[2];
/*! Byte: 14; Flag indicating incomplete transaction */
    unsigned char transaction;
/*! Byte: 15; Encryption Flag */
    unsigned char encryption;
/*! Byte: 16-27; reserved for dBASE in a multiuser environment*/
    unsigned char reserved02[12];
/*! Byte: 28; Production MDX file flag */
    unsigned char mdx;
/*! Byte: 29; Language driver ID, for Visual FoxPro */
    unsigned char language;
/*! Byte: 30-31; reserved, filled with zero */
    unsigned char reserved03[2];
} DB_HEADER;

/*! \struct DB_FIELD
	\brief The field descriptor array
	Offsets of this header are the same in all versions of dBASE
	\warning It is recommend not to access DB_FIELD directly.
 */
typedef struct InfoOfColumn {
/*! Byte: 0-10; fieldname in ASCII */
    char field_name[11];
/*! Byte: 11; field type in ASCII (C, D, L, M or N) */
    unsigned char field_type;
/*! Byte: 12-15; field data adress */
    u_int32_t field_address;
/*! Byte: 16; field length in binary */
    unsigned char field_length;
/*! Byte: 17; field decimal count in binary */
    unsigned char field_decimals;
/*! Byte: 18-30; reserved */
    unsigned char reserved1[2];
    u_int32_t field_offset;
    unsigned char reserved2[7];
/*! Byte: 31; Production MDX field flag */
    unsigned char mdx;
} DB_FIELD;


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

/* [1] Integers stored with the most
significant byte first. See: endian.h   */

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


#define UN_DELETE_FLAG " "
#define DELETED_FLAG "*"

// sizeof(DB_FIELD)
#define SIZE_OF_DB_FIELD 32

// sizeof(DB_HEADER)
#define SIZE_OF_DB_HEAD 32

#define RECORD_TERMINATOR 0x0D
#define SIZE_OF_RECORD_TERMINATOR 1

#define SZIE_OF_DELETE_FLAG 1

u_int16_t rotate2b(u_int16_t var);

u_int32_t rotate4b(u_int32_t var);

#endif //DBF_STRUCT_H
