//
// Created by wuxw on 2025/4/24/024.
//

#include <libdbf/libdbf.h>
#include <stdio.h>

int main() {
    char file[] = "OrderAlgo_20250424.dbf";
    P_DBF* p_dbf = dbf_Open(file, true);
    printf("formatted date:%s\n", dbf_GetDate(p_dbf));
    printf("Number of rows:%d\n", dbf_NumRows(p_dbf));
    printf("Number of columns:%d\n", dbf_NumCols(p_dbf));
    printf("header length:%d\n", dbf_HeaderSize(p_dbf));
    printf("record length:%d\n", dbf_RecordLength(p_dbf));

    for (int column = 0; column < dbf_NumCols(p_dbf); ++column) {
        printf("column:%d name: %s; type:%c; length:%d; decimals:%d \n", column,
               dbf_ColumnName(p_dbf, column), dbf_ColumnType(p_dbf, column),
               dbf_ColumnSize(p_dbf, column), dbf_ColumnDecimals(p_dbf, column));
    }
    dbf_Close(p_dbf);
    return 0;
}
