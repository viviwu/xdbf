// dbf_def.h
#ifndef DBF_DEF_H
#define DBF_DEF_H

#include <stdint.h>
#include <time.h>
#include <string.h>

#pragma pack(push, 1) // ȷ���ṹ�尴1�ֽڶ���

// DBF�汾����ö�� (�汾�ŵ�һ���ֽ�)
typedef enum {
    DBF_VERSION_FOXBASE_1          = 0x02, // FoxBASE
    DBF_VERSION_FOXBASE_DBASE3     = 0x03, // dBASE III+/FoxBASE+/dBase III PLUS
    DBF_VERSION_VISUAL_FOXPRO      = 0x30, // Visual FoxPro
    DBF_VERSION_FOXPRO_2           = 0x31, // FoxPro 2.x
    DBF_VERSION_DBASE4             = 0x43, // dBASE IV SQL table files, no memo
    DBF_VERSION_DBASE4_MEMO        = 0x63, // dBASE IV SQL system files, with memo
    DBF_VERSION_DBASE5             = 0x83, // dBASE V, no memo
    DBF_VERSION_DBASE5_MEMO        = 0x8B, // dBASE V, with memo
    DBF_VERSION_DBASE7             = 0x87, // dBASE VII, no memo
    DBF_VERSION_DBASE7_MEMO        = 0x8C, // dBASE VII, with memo
    DBF_VERSION_FOXPRO_2_MEMO      = 0xF5, // FoxPro 2.x with memo
    DBF_VERSION_FOXBASE_DBASE3_MEMO= 0xFB, // FoxBASE+/dBASE III PLUS with memo
} DBF_Version;

// DBF�ļ�ͷ�ṹ (32 bytes)
typedef struct {
    /* �ֽ� 0: �汾�� (�μ�DBF_Versionö��) */
    uint8_t version;

    /* �ֽ� 1-3: ���������� (YYMMDD)
       YY: ���-1900 (��2021-1900=121)
       MM: 1-12
       DD: 1-31 */
    uint8_t last_update[3];

    /* �ֽ� 4-7: �ļ��еļ�¼�� (32λС��) */
    uint32_t num_records;

    /* �ֽ� 8-9: �ļ�ͷ��С(�ֽ�) (16λС��) ����ͷ+�ֶ�����+��ֹ��
     * Position of first data record */
    uint16_t header_size;

    /* �ֽ� 10-11: ÿ����¼��С(�ֽ�) (16λС��) */
    uint16_t record_size;

    /* �ֽ� 12-13: ���� (�������ڰ汾������) */
    uint16_t reserved1;

    /* �ֽ� 14: �����־ (0x00=��, 0x01=��δ�������) */
    uint8_t incomplete_transaction;

    /* �ֽ� 15: ���ܱ�־ (0x00=δ����, 0x01=����) */
    uint8_t encryption_flag;

    /* �ֽ� 16-27: ���û��������� */
    uint8_t multi_user_reserved[12];

    /* �ֽ� 28: MDX��־ (0x00=��MDX, 0x01=��MDX) */
    uint8_t mdx_flag;

    /* �ֽ� 29: ������������ID (�μ�DBF_LanguageDriverö��) */
    uint8_t language_driver;

    /* �ֽ� 30-31: ���� */
    uint16_t reserved3;
} DBF_Header, DB_HEADER;

// �ֶ������ṹ (32 bytes each)
typedef struct {
    /* �ֽ� 0-10: �ֶ���(ASCIIZ, ��0x00��β) */
    char field_name[11];

    /* �ֽ� 11: �ֶ����� (�μ�DBF_FieldTypeö��) */
    uint8_t field_type;

    /* �ֽ� 12-15: �ֶ����ݵ�ַ(�ڴ��ַ���ļ��в�ʹ�ã�ͨ��Ϊ0) */
    uint32_t field_data_address;

    /* �ֽ� 16: �ֶγ��� (���254) */
    uint8_t field_length;

    /* �ֽ� 17: С��λ��(��ֵ�ֶ�) */
    uint8_t decimal_count;

    /* �ֽ� 18-19: ���� (�������ڰ汾������) */
    uint16_t reserved1;

    /* �ֽ� 20: ������ID */
    uint8_t work_area_id;

    /* �ֽ� 21-22: ���� */
    uint16_t reserved2;

    /* �ֽ� 23: SET FIELDS��־ (0x00=δ����, 0x01=������) */
    uint8_t set_fields_flag;

    /* �ֽ� 24-30: ���� */
    uint8_t reserved3[7];

    /* �ֽ� 31: MDX�ֶα�־ (0x00=����MDX��, 0x01=��MDX��) */
    uint8_t mdx_flag;
} DBF_FieldDescriptor, DB_FIELD;

// �ļ���ֹ�� (1 byte)
typedef struct {
    /* �ֽ� 0: 0x0D��ʾ�ֶ��������� */
    uint8_t terminator;
} DBF_HeaderTerminator;

#define DBF_RECORD_DELETED  0x2A
#define DBF_RECORD_VALID  0x20
// ��¼ɾ����־�����Դ洢
typedef struct {
    /* �ֽ� 0: ɾ����־
       0x20(�ո�)=δɾ��, 0x2A(*)=��ɾ�� */
    uint8_t deleted_flag;

    /* �ֽ� 1-n: ʵ���ֶ�����(�䳤) */
    uint8_t data[];
} DBF_Record;

// �ļ�������־ (1 byte)
typedef struct {
    /* �ֽ� 0: 0x1A��ʾ�ļ����� */
    uint8_t eof_marker;
} DBF_EOFMarker;

// ��������ת�������ṹ
typedef struct {
    uint8_t year;   // ���-1900
    uint8_t month;  // 1-12
    uint8_t day;    // 1-31
} DBF_Date;

// ������������IDö��
typedef enum {
    DBF_LANG_NONE = 0,
    DBF_LANG_US = 0x01,          // USA MS-DOS (OEM)
    DBF_LANG_INTL = 0x02,        // International MS-DOS (OEM)
    DBF_LANG_WIN = 0x03,         // Windows ANSI
    DBF_LANG_MAC = 0x04,         // Macintosh
    DBF_LANG_EEUROPE = 0x64,     // Eastern European MS-DOS (OEM)
    DBF_LANG_RUSSIAN = 0x65,     // Russian MS-DOS (OEM)
    DBF_LANG_NORDIC = 0x66,      // Nordic MS-DOS (OEM)
    DBF_LANG_ICELAND = 0x67,     // Icelandic MS-DOS (OEM)
    DBF_LANG_CZECH = 0x68,       // Czech MS-DOS (OEM)
    DBF_LANG_POLISH = 0x69,      // Polish MS-DOS (OEM)
    DBF_LANG_GREEK = 0x6A,       // Greek MS-DOS (OEM)
    DBF_LANG_TURKISH = 0x6B,     // Turkish MS-DOS (OEM)
    DBF_LANG_CYRILLIC = 0x6C,    // Russian Windows (ANSI)
    DBF_LANG_EEUROPE_WIN = 0x6D, // Eastern European Windows (ANSI)
    DBF_LANG_GREEK_WIN = 0x6E,   // Greek Windows (ANSI)
    DBF_LANG_CHINESE_GB = 0x78,  // Simplified Chinese Windows (ANSI)
    DBF_LANG_KOREAN = 0x79,      // Korean Windows (ANSI)
    DBF_LANG_JAPANESE = 0x7A,    // Japanese Windows (ANSI)
    DBF_LANG_CHINESE_BIG5 = 0x7B,// Traditional Chinese Windows (ANSI)
    DBF_LANG_THAI = 0x7C,        // Thai Windows (ANSI)
    DBF_LANG_HEBREW = 0x7D,      // Hebrew Windows (ANSI)
    DBF_LANG_ARABIC = 0x7E,      // Arabic Windows (ANSI)
} DBF_LanguageDriver;

// �ֶ�����ö��
typedef enum {
    DBF_FIELD_CHAR = 'C',     // �ַ���
    DBF_FIELD_NUMERIC = 'N',  // ��ֵ��
    DBF_FIELD_FLOAT = 'F',    // ������
    DBF_FIELD_DATE = 'D',     // ������ (YYYYMMDD)
    DBF_FIELD_LOGICAL = 'L',  // �߼��� (T/F/Y/N)
    DBF_FIELD_MEMO = 'M',     // ��ע��
    DBF_FIELD_BINARY = 'B',   // ��������
    DBF_FIELD_GENERAL = 'G',  // ͨ����(OLE����)
    DBF_FIELD_PICTURE = 'P',  // ͼƬ��
    DBF_FIELD_CURRENCY = 'Y', // ������
    DBF_FIELD_DATETIME = 'T', // ����ʱ����
    DBF_FIELD_INTEGER = 'I',  // ����
    DBF_FIELD_VARCHAR = 'V',  // �䳤�ַ���
    DBF_FIELD_TIMESTAMP = '@',// ʱ���
    DBF_FIELD_DOUBLE = 'O',   // ˫���ȸ���
    DBF_FIELD_AUTOINC = '+',  // ������
} DBF_FieldType;

#pragma pack(pop) // �ָ�Ĭ�϶��뷽ʽ

// ==================== ��������ʵ�� ====================

/**
 * ����DBF����(YYMMDD)Ϊtime_t
 * @param date DBF��ʽ��3�ֽ���������
 * @return ��Ӧ��time_tֵ
 */
static inline time_t dbf_parse_date(const uint8_t date[3]) {
    struct tm tm = {0};
    tm.tm_year = date[0] + 100; // DBF���=ʵ��-1900, tm_year=ʵ��-1900
    tm.tm_mon = date[1] - 1;     // �·�0-11
    tm.tm_mday = date[2];
    return mktime(&tm);
}

/**
 * ��ʽ��time_tΪDBF����(YYMMDD)
 * @param time ʱ��ֵ
 * @param out_date �����3�ֽ�DBF����
 */
static inline void dbf_format_date(time_t time, uint8_t out_date[3]) {
    struct tm* tm = localtime(&time);
    out_date[0] = tm->tm_year % 100; // ȡ����λ���
    out_date[1] = tm->tm_mon + 1;
    out_date[2] = tm->tm_mday;
}

/**
 * �ֶ�����ת�ɶ��ַ���
 * @param type �ֶ������ַ�
 * @return ���������ַ���
 */
static inline const char* dbf_field_type_to_str(uint8_t type) {
    switch(type) {
        case DBF_FIELD_CHAR: return "Character";
        case DBF_FIELD_NUMERIC: return "Numeric";
        case DBF_FIELD_FLOAT: return "Float";
        case DBF_FIELD_DATE: return "Date";
        case DBF_FIELD_LOGICAL: return "Logical";
        case DBF_FIELD_MEMO: return "Memo";
        case DBF_FIELD_BINARY: return "Binary";
        case DBF_FIELD_GENERAL: return "General(OLE)";
        case DBF_FIELD_PICTURE: return "Picture";
        case DBF_FIELD_CURRENCY: return "Currency";
        case DBF_FIELD_DATETIME: return "DateTime";
        case DBF_FIELD_INTEGER: return "Integer";
        case DBF_FIELD_VARCHAR: return "VarChar";
        case DBF_FIELD_TIMESTAMP: return "Timestamp";
        case DBF_FIELD_DOUBLE: return "Double";
        case DBF_FIELD_AUTOINC: return "AutoIncrement";
        default: return "Unknown";
    }
}

#endif // DBF_DEF_H