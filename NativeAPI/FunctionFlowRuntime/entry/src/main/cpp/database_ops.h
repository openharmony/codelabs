/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.All Rights Reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FUNCTIONFLOWRUNTIME_DATABASE_OPS_H
#define FUNCTIONFLOWRUNTIME_DATABASE_OPS_H

#include <cstdio>
#include <cstdlib>
#include <cstdbool>
#include <ctime>
#include <string>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

/* ========== 常量定义 ========== */
#define MAX_TABLE_NAME 50
#define MAX_COLUMN_NAME 30
#define MAX_COLUMNS 20
#define MAX_ROWS 1000
#define MAX_STRING_LEN 256
#define MAX_INDEXES 10
#define MAX_QUERY_LEN 1024
#define MAX_TRANSACTION_DEPTH 5
#define MAX_ERROR_MSG 200
#define PAGE_SIZE 4096

/* ========== 数据类型枚举 ========== */
typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_BOOL,
    TYPE_DATE,
    TYPE_NULL
} DataType;

/* ========== 错误码枚举 ========== */
typedef enum {
    ERR_NONE,
    ERR_MEMORY,
    ERR_SYNTAX,
    ERR_TYPE_MISMATCH,
    ERR_TABLE_EXISTS,
    ERR_TABLE_NOT_FOUND,
    ERR_COLUMN_NOT_FOUND,
    ERR_DUPLICATE_ENTRY,
    ERR_INDEX_EXISTS,
    ERR_INDEX_NOT_FOUND,
    ERR_TRANSACTION_ACTIVE,
    ERR_TRANSACTION_NESTED,
    ERR_CONSTRAINT_VIOLATION,
    ERR_IO_ERROR,
    ERR_INVALID_VALUE,
    ERR_MAX_ROWS_EXCEEDED
} ErrorCode;

/* ========== 列定义结构 ========== */
typedef struct {
    char name[MAX_COLUMN_NAME];
    DataType type;
    int length;          // 对于字符串类型
    bool notNull;
    bool isPrimary;
    bool hasIndex;
    char defaultValue[MAX_STRING_LEN];
} ColumnDef;

/* ========== 数据单元结构 ========== */
typedef struct {
    DataType type;
    union {
        int intVal;
        float floatVal;
        char stringVal[MAX_STRING_LEN];
        bool boolVal;
        time_t dateVal;
    } value;
    bool isNull;
} DataCell;

#ifdef __cplusplus
}
#endif

/* ========== 表行结构 ========== */
class SmartTableRow {
public:
    std::unique_ptr<DataCell[]> cells;  // 使用unique_ptr管理数组
    size_t cellCount;                  // 数组大小
    int rowId;
    bool deleted;
    time_t createdAt;
    time_t updatedAt;

    SmartTableRow()
    {
        cells = nullptr;
        cellCount = 0;
        rowId = 0;
        deleted = false;
    }

    explicit SmartTableRow(int columnCnt)
    {
        cellCount = columnCnt;
        cells = std::make_unique<DataCell[]>(columnCnt);
        rowId = 0;
        deleted = false;
    }

    // 构造函数1：从vector初始化
    SmartTableRow(int id, const std::vector<DataCell>& cellData)
        : rowId(id), deleted(false), cellCount(cellData.size())
    {
        time_t now = time(nullptr);
        if (now == static_cast<time_t>(-1)) {
            throw std::runtime_error("Failed to get current time");
        }
        createdAt = now;
        updatedAt = now;
        cells = std::make_unique<DataCell[]>(cellCount);
        for (size_t i = 0; i < cellCount; ++i) {
            cells[i] = cellData[i];  // 调用拷贝构造函数
        }
    }

    // 构造函数2：从初始化列表
    SmartTableRow(int id, std::initializer_list<DataCell> initList)
        : SmartTableRow(id, std::vector<DataCell>(initList)) {}

    // 深拷贝构造函数
    SmartTableRow(const SmartTableRow& other)
        : cellCount(other.cellCount),
          rowId(other.rowId),
          deleted(other.deleted),
          createdAt(other.createdAt),
          updatedAt(other.updatedAt)
    {
        if (other.cells) {
            cells = std::make_unique<DataCell[]>(cellCount);
            for (size_t i = 0; i < cellCount; ++i) {
                cells[i] = other.cells[i];
            }
        }
    }
    // 深拷贝赋值运算符
    SmartTableRow& operator=(const SmartTableRow& other)
    {
        if (this != &other) {
            cells.reset();  // 释放原有内存

            cellCount = other.cellCount;
            rowId = other.rowId;
            deleted = other.deleted;
            createdAt = other.createdAt;
            updatedAt = other.updatedAt;

            if (other.cells) {
                cells = std::make_unique<DataCell[]>(cellCount);
                for (size_t i = 0; i < cellCount; ++i) {
                    cells[i] = other.cells[i];
                }
            }
        }
        return *this;
    }
};

/* ========== B+树索引节点结构 ========== */
class SmartBPlusTreeNode {
public:
    // 使用unique_ptr管理动态数组
    std::unique_ptr<int[]> keys;
    std::unique_ptr<std::shared_ptr<void>[]> children;  // void*的智能指针版本

    int numKeys;
    bool isLeaf;

    // 使用weak_ptr避免循环引用
    std::weak_ptr<SmartBPlusTreeNode> nextPtr;
    std::weak_ptr<SmartBPlusTreeNode> parentPtr;

    int maxDegree;

    // 构造函数
    SmartBPlusTreeNode(int degree, bool leaf)
        : maxDegree(degree), isLeaf(leaf), numKeys(0)
    {
        // 分配keys数组（B+树最多有maxDegree-1个key）
        int maxKeys = maxDegree - 1;
        keys = std::make_unique<int[]>(maxKeys);
        // 分配children数组（B+树最多有maxDegree个孩子）
        int maxChildren = isLeaf ? 0 : maxDegree;
        children = std::make_unique<std::shared_ptr<void>[]>(maxChildren);
        for (size_t i = 0; i < maxChildren; ++i) {
            children[i] = nullptr;
            // 或等价写法：children[i].reset();
        }
    }
};

#ifdef __cplusplus
extern "C" {
#endif

/* ========== 索引结构 ========== */
typedef struct {
    char name[MAX_COLUMN_NAME];
    int columnIndex;
    SmartBPlusTreeNode *root;
    int height;
    int keyCount;
} TableIndex;

/* ========== 表结构 ========== */
typedef struct {
    char name[MAX_TABLE_NAME];
    ColumnDef columns[MAX_COLUMNS];
    SmartTableRow rows[MAX_ROWS];
    TableIndex indexes[MAX_INDEXES];
    int columnCount;
    int rowCount;
    int nextRowId;
    int indexCount;
    time_t createdAt;
    bool inTransaction;
    char dataFile[256];
} DatabaseTable;

/* ========== 事务日志条目 ========== */
typedef struct {
    enum { OP_INSERT, OP_UPDATE, OP_DELETE } operation;
    int rowId;
    DataCell *oldData;
    DataCell *newData;
    int columnCount;
    time_t timestamp;
} TransactionLog;

/* ========== 事务结构 ========== */
typedef struct {
    TransactionLog logs[1000];
    int logCount;
    DatabaseTable *table;
    bool active;
    int savepointCount;
    int savepoints[10];
} Transaction;

/* ========== 查询结果结构 ========== */
typedef struct {
    DataCell **rows;
    int rowCount;
    int columnCount;
    char **columnNames;
    ErrorCode error;
    char errorMsg[MAX_ERROR_MSG];
} QueryResult;

#ifdef __cplusplus
}
#endif
class SmartQueryResult {
public:
    // 使用unique_ptr管理二维数组
    std::unique_ptr<std::unique_ptr<DataCell[]>[]> rows;
    int rowCount;
    int columnCount;

    // 列名使用unique_ptr管理
    std::unique_ptr<std::unique_ptr<char[]>[]> columnNames;
    ErrorCode error;
    char errorMsg[MAX_ERROR_MSG];

    SmartQueryResult(int cols,
                     const std::vector<std::string>& colNames,
                     const std::vector<std::vector<DataCell>>& data) : columnCount(cols)
    {
        columnNames = std::make_unique<std::unique_ptr<char[]>[]>(columnCount);
        for (int i = 0; i < columnCount; i++) {
            if (i < static_cast<int>(colNames.size()) && !colNames[i].empty()) {
                columnNames[i] = std::make_unique<char[]>(colNames[i].size() + 1);
            }
        }

        // 分配行数据
        rowCount = data.size();
        rows = std::make_unique<std::unique_ptr<DataCell[]>[]>(rowCount);
        for (int i = 0; i < rowCount; ++i) {
            rows[i] = std::make_unique<DataCell[]>(columnCount);
            for (int j = 0; j < columnCount; ++j) {
                rows[i][j] = data[i][j];
            }
        }
    }

    // 获取列名
    const char* GetColumnName(int col) const
    {
        if (col < 0 || col >= columnCount || !columnNames || !columnNames[col]) {
            return "";
        }
        return columnNames[col].get();
    }
};

#ifdef __cplusplus
extern "C" {
#endif

/* ========== 全局数据库状态 ========== */
typedef struct {
    DatabaseTable *tables[50];
    int tableCount;
    Transaction activeTransactions[MAX_TRANSACTION_DEPTH];
    int transactionDepth;
    char currentDatabase[256];
    bool autoCommit;
    FILE *logFile;
} DatabaseSystem;

time_t ParseDate(const char *dateStr);
DataCell CreateDataCell(DataType type, const char *value);

#ifdef __cplusplus
}
#endif

class DataBaseOps {
private:
    /* ========== 辅助函数 ========== */
    const char* DataTypeToString(DataType type);
    DataType StringToDataType(const char *str);

    char* FormatDate(time_t timestamp);
    void FreeDataCell(DataCell *cell);
    int CompareDataCells(const DataCell *a, const DataCell *b);
    SmartBPlusTreeNode* CreateBplusTreeNode(bool isLeaf, int order);
    void DeleteFromIndex(TableIndex *index, int key);
    SmartTableRow* FindTableRow(int rowId);

private:
    // DatabaseTable *table;
    std::unique_ptr<DatabaseTable> table;
    std::unique_ptr<SmartQueryResult> queryResult;

public:
    DataBaseOps(const char *tableName, ColumnDef *columns, int columnCount);
    ~DataBaseOps();
    TableIndex* CreateTableIndex(const char *columnName);
    bool DeleteTableRow(int rowId);
    SmartQueryResult* ExecuteSelectQuery(const char *whereClause);
    bool BeginTransaction();
    bool CommitTransaction();
    bool RollbackTransaction();
    void PrintTableSchema();
    void PrintQueryResult(SmartQueryResult *result);
    int InsertTableRow(DataCell *cells);
    bool UpdateTableRow(int rowId, DataCell *newCells);
};

#ifdef __cplusplus
extern "C" {
#endif

int DatabaseOpsDemo();

#ifdef __cplusplus
}
#endif

#endif //FUNCTIONFLOWRUNTIME_DATABASE_OPS_H
