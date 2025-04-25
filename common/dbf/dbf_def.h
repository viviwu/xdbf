// dbf_def.h
#ifndef DBF_DEF_H
#define DBF_DEF_H

#include <stdint.h>
#include <string.h>
#include <time.h>

#pragma pack(push, 1)  // 确保结构体按1字节对齐

// DBF版本类型枚举 (版本号第一个字节)
typedef enum {
    DBF_VERSION_FOXBASE_1 = 0x02,            // FoxBASE
    DBF_VERSION_FOXBASE_DBASE3 = 0x03,       // dBASE III+/FoxBASE+/dBase III PLUS
    DBF_VERSION_VISUAL_FOXPRO = 0x30,        // Visual FoxPro
    DBF_VERSION_FOXPRO_2 = 0x31,             // FoxPro 2.x
    DBF_VERSION_DBASE4 = 0x43,               // dBASE IV SQL table files, no memo
    DBF_VERSION_DBASE4_MEMO = 0x63,          // dBASE IV SQL system files, with memo
    DBF_VERSION_DBASE5 = 0x83,               // dBASE V, no memo
    DBF_VERSION_DBASE5_MEMO = 0x8B,          // dBASE V, with memo
    DBF_VERSION_DBASE7 = 0x87,               // dBASE VII, no memo
    DBF_VERSION_DBASE7_MEMO = 0x8C,          // dBASE VII, with memo
    DBF_VERSION_FOXPRO_2_MEMO = 0xF5,        // FoxPro 2.x with memo
    DBF_VERSION_FOXBASE_DBASE3_MEMO = 0xFB,  // FoxBASE+/dBASE III PLUS with memo
} DBF_Version;

// DBF文件头结构 (32 bytes)
typedef struct {
    /* 字节 0: 版本号 (参见DBF_Version枚举) */
    uint8_t version;

    /* 字节 1-3: 最后更新日期 (YYMMDD)
       YY: 年份-1900 (如2021-1900=121)
       MM: 1-12
       DD: 1-31 */
    uint8_t last_update[3];

    /* 字节 4-7: 文件中的记录数 (32位小端) */
    uint32_t num_records;

    /* 字节 8-9: 文件头大小(字节) (16位小端) 包括头+字段描述+终止符
     * Position of first data record */
    uint16_t header_size;

    /* 字节 10-11: 每条记录大小(字节) (16位小端) */
    uint16_t record_size;

    /* 字节 12-13: 保留 (用于早期版本兼容性) */
    uint16_t reserved1;

    /* 字节 14: 事务标志 (0x00=无, 0x01=有未完成事务) */
    uint8_t incomplete_transaction;

    /* 字节 15: 加密标志 (0x00=未加密, 0x01=加密) */
    uint8_t encryption_flag;

    /* 字节 16-27: 多用户环境保留 */
    uint8_t multi_user_reserved[12];

    /* 字节 28: MDX标志 (0x00=无MDX, 0x01=有MDX) */
    uint8_t mdx_flag;

    /* 字节 29: 语言驱动程序ID (参见DBF_LanguageDriver枚举) */
    uint8_t language_driver;

    /* 字节 30-31: 保留 */
    uint16_t reserved3;
} DBF_Header, DB_HEADER;

// 字段描述结构 (32 bytes each)
typedef struct {
    /* 字节 0-10: 字段名(ASCIIZ, 以0x00结尾) */
    char field_name[11];

    /* 字节 11: 字段类型 (参见DBF_FieldType枚举) */
    uint8_t field_type;

    /* 字节 12-15: Displacement of field in record */
    uint32_t field_address;

    /* 字节 16: 字段长度 (最大254) */
    uint8_t field_length;

    /* 字节 17: 小数位数(数值字段) */
    uint8_t decimal_count;

    /* 字节 18-19: 保留 (用于早期版本兼容性) */
    uint16_t reserved1;

    /* 字节 20: 工作区ID */
    uint8_t work_area_id;

    /* 字节 21-22: 保留 */
    uint16_t reserved2;

    /* 字节 23: SET FIELDS标志 (0x00=未设置, 0x01=已设置) */
    uint8_t set_fields_flag;

    /* 字节 24-30: 保留 */
    uint8_t reserved3[7];

    /* 字节 31: MDX字段标志 (0x00=不在MDX中, 0x01=在MDX中) */
    uint8_t mdx_flag;
} DBF_FieldDescriptor, DB_FIELD;

// 文件终止符 (1 byte)
typedef struct {
    /* 字节 0: 0x0D表示字段描述结束 */
    uint8_t terminator;
} DBF_HeaderTerminator;

#define DBF_RECORD_DELETED 0x2A
#define DBF_RECORD_VALID 0x20
// 记录删除标志和属性存储
typedef struct {
    /* 字节 0: 删除标志
       0x20(空格)=未删除, 0x2A(*)=已删除 */
    uint8_t deleted_flag;

    /* 字节 1-n: 实际字段数据(变长) */
    uint8_t data[];
} DBF_Record;

// 文件结束标志 (1 byte)
typedef struct {
    /* 字节 0: 0x1A表示文件结束 */
    uint8_t eof_marker;
} DBF_EOFMarker;

// 日期类型转换辅助结构
typedef struct {
    uint8_t year;   // 年份-1900
    uint8_t month;  // 1-12
    uint8_t day;    // 1-31
} DBF_Date;

// 语言驱动程序ID枚举
typedef enum {
    DBF_LANG_NONE = 0,
    DBF_LANG_US = 0x01,            // USA MS-DOS (OEM)
    DBF_LANG_INTL = 0x02,          // International MS-DOS (OEM)
    DBF_LANG_WIN = 0x03,           // Windows ANSI
    DBF_LANG_MAC = 0x04,           // Macintosh
    DBF_LANG_EEUROPE = 0x64,       // Eastern European MS-DOS (OEM)
    DBF_LANG_RUSSIAN = 0x65,       // Russian MS-DOS (OEM)
    DBF_LANG_NORDIC = 0x66,        // Nordic MS-DOS (OEM)
    DBF_LANG_ICELAND = 0x67,       // Icelandic MS-DOS (OEM)
    DBF_LANG_CZECH = 0x68,         // Czech MS-DOS (OEM)
    DBF_LANG_POLISH = 0x69,        // Polish MS-DOS (OEM)
    DBF_LANG_GREEK = 0x6A,         // Greek MS-DOS (OEM)
    DBF_LANG_TURKISH = 0x6B,       // Turkish MS-DOS (OEM)
    DBF_LANG_CYRILLIC = 0x6C,      // Russian Windows (ANSI)
    DBF_LANG_EEUROPE_WIN = 0x6D,   // Eastern European Windows (ANSI)
    DBF_LANG_GREEK_WIN = 0x6E,     // Greek Windows (ANSI)
    DBF_LANG_CHINESE_GB = 0x78,    // Simplified Chinese Windows (ANSI)
    DBF_LANG_KOREAN = 0x79,        // Korean Windows (ANSI)
    DBF_LANG_JAPANESE = 0x7A,      // Japanese Windows (ANSI)
    DBF_LANG_CHINESE_BIG5 = 0x7B,  // Traditional Chinese Windows (ANSI)
    DBF_LANG_THAI = 0x7C,          // Thai Windows (ANSI)
    DBF_LANG_HEBREW = 0x7D,        // Hebrew Windows (ANSI)
    DBF_LANG_ARABIC = 0x7E,        // Arabic Windows (ANSI)
} DBF_LanguageDriver;

// 字段类型枚举
typedef enum {
    DBF_FIELD_CHAR = 'C',       // 字符型
    DBF_FIELD_NUMERIC = 'N',    // 数值型
    DBF_FIELD_FLOAT = 'F',      // 浮点型
    DBF_FIELD_DATE = 'D',       // 日期型 (YYYYMMDD)
    DBF_FIELD_LOGICAL = 'L',    // 逻辑型 (T/F/Y/N)
    DBF_FIELD_MEMO = 'M',       // 备注型
    DBF_FIELD_BINARY = 'B',     // 二进制型
    DBF_FIELD_GENERAL = 'G',    // 通用型(OLE对象)
    DBF_FIELD_PICTURE = 'P',    // 图片型
    DBF_FIELD_CURRENCY = 'Y',   // 货币型
    DBF_FIELD_DATETIME = 'T',   // 日期时间型
    DBF_FIELD_INTEGER = 'I',    // 整型
    DBF_FIELD_VARCHAR = 'V',    // 变长字符型
    DBF_FIELD_TIMESTAMP = '@',  // 时间戳
    DBF_FIELD_DOUBLE = 'O',     // 双精度浮点
    DBF_FIELD_AUTOINC = '+',    // 自增型
} DBF_FieldType;

#pragma pack(pop)  // 恢复默认对齐方式

// ==================== 辅助函数实现 ====================

/**
 * 解析DBF日期(YYMMDD)为time_t
 * @param date DBF格式的3字节日期数据
 * @return 对应的time_t值
 */
static inline time_t dbf_parse_date(const uint8_t date[3]) {
    struct tm tm = {0};
    tm.tm_year = date[0] + 100;  // DBF年份=实际-1900, tm_year=实际-1900
    tm.tm_mon = date[1] - 1;     // 月份0-11
    tm.tm_mday = date[2];
    return mktime(&tm);
}

/**
 * 格式化time_t为DBF日期(YYMMDD)
 * @param time 时间值
 * @param out_date 输出的3字节DBF日期
 */
static inline void dbf_format_date(time_t time, uint8_t out_date[3]) {
    struct tm* tm = localtime(&time);
    out_date[0] = tm->tm_year % 100;  // 取后两位年份
    out_date[1] = tm->tm_mon + 1;
    out_date[2] = tm->tm_mday;
}

/**
 * 字段类型转可读字符串
 * @param type 字段类型字符
 * @return 类型描述字符串
 */
static inline const char* dbf_field_type_to_str(uint8_t type) {
    switch (type) {
        case DBF_FIELD_CHAR:
            return "Character";
        case DBF_FIELD_NUMERIC:
            return "Numeric";
        case DBF_FIELD_FLOAT:
            return "Float";
        case DBF_FIELD_DATE:
            return "Date";
        case DBF_FIELD_LOGICAL:
            return "Logical";
        case DBF_FIELD_MEMO:
            return "Memo";
        case DBF_FIELD_BINARY:
            return "Binary";
        case DBF_FIELD_GENERAL:
            return "General(OLE)";
        case DBF_FIELD_PICTURE:
            return "Picture";
        case DBF_FIELD_CURRENCY:
            return "Currency";
        case DBF_FIELD_DATETIME:
            return "DateTime";
        case DBF_FIELD_INTEGER:
            return "Integer";
        case DBF_FIELD_VARCHAR:
            return "VarChar";
        case DBF_FIELD_TIMESTAMP:
            return "Timestamp";
        case DBF_FIELD_DOUBLE:
            return "Double";
        case DBF_FIELD_AUTOINC:
            return "AutoIncrement";
        default:
            return "Unknown";
    }
}

#endif  // DBF_DEF_H