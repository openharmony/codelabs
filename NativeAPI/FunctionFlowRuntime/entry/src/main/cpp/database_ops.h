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
    bool not_null;
    bool is_primary;
    bool has_index;
    char default_value[MAX_STRING_LEN];
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
    size_t cell_count;                  // 数组大小
    int row_id;
    bool deleted;
    time_t created_at;
    time_t updated_at;

    SmartTableRow() {
        cells = nullptr;
        cell_count = 0;
        row_id = 0;
        deleted = false;
    }

    SmartTableRow(int column_cnt) {
        cell_count = column_cnt;
        cells = std::make_unique<DataCell[]>(column_cnt);
        row_id = 0;
        deleted = false;
    }

    // 构造函数1：从vector初始化
    SmartTableRow(int id, const std::vector<DataCell>& cell_data)
        : row_id(id), deleted(false), cell_count(cell_data.size()) {

        time_t now = time(nullptr);
        created_at = now;
        updated_at = now;

        cells = std::make_unique<DataCell[]>(cell_count);
        for (size_t i = 0; i < cell_count; ++i) {
            cells[i] = cell_data[i];  // 调用拷贝构造函数
        }
    }

    // 构造函数2：从初始化列表
    SmartTableRow(int id, std::initializer_list<DataCell> init_list)
        : SmartTableRow(id, std::vector<DataCell>(init_list)) {}

    // 深拷贝构造函数
    SmartTableRow(const SmartTableRow& other)
        : cell_count(other.cell_count),
          row_id(other.row_id),
          deleted(other.deleted),
          created_at(other.created_at),
          updated_at(other.updated_at) {

        if (other.cells) {
            cells = std::make_unique<DataCell[]>(cell_count);
            for (size_t i = 0; i < cell_count; ++i) {
                cells[i] = other.cells[i];
            }
        }
    }

    // 深拷贝赋值运算符
    SmartTableRow& operator=(const SmartTableRow& other) {
        if (this != &other) {
            cells.reset();  // 释放原有内存

            cell_count = other.cell_count;
            row_id = other.row_id;
            deleted = other.deleted;
            created_at = other.created_at;
            updated_at = other.updated_at;

            if (other.cells) {
                cells = std::make_unique<DataCell[]>(cell_count);
                for (size_t i = 0; i < cell_count; ++i) {
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

    int num_keys;
    bool is_leaf;

    // 使用weak_ptr避免循环引用
    std::weak_ptr<SmartBPlusTreeNode> next_ptr;
    std::weak_ptr<SmartBPlusTreeNode> parent_ptr;

    int max_degree;

    // 构造函数
    SmartBPlusTreeNode(int degree, bool leaf) 
        : max_degree(degree), is_leaf(leaf), num_keys(0) {

        // 分配keys数组（B+树最多有max_degree-1个key）
        int max_keys = max_degree - 1;
        keys = std::make_unique<int[]>(max_keys);

        // 分配children数组（B+树最多有max_degree个孩子）
        int max_children = is_leaf ? 0 : max_degree;
        children = std::make_unique<std::shared_ptr<void>[]>(max_children);

        for (size_t i = 0; i < max_children; ++i) {
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
    int column_index;
    SmartBPlusTreeNode *root;
    int height;
    int key_count;
} TableIndex;

/* ========== 表结构 ========== */
typedef struct {
    char name[MAX_TABLE_NAME];
    ColumnDef columns[MAX_COLUMNS];
    SmartTableRow rows[MAX_ROWS];
    TableIndex indexes[MAX_INDEXES];
    int column_count;
    int row_count;
    int next_row_id;
    int index_count;
    time_t created_at;
    bool in_transaction;
    char data_file[256];
} DatabaseTable;

/* ========== 事务日志条目 ========== */
typedef struct {
    enum { OP_INSERT, OP_UPDATE, OP_DELETE } operation;
    int row_id;
    DataCell *old_data;
    DataCell *new_data;
    int column_count;
    time_t timestamp;
} TransactionLog;

/* ========== 事务结构 ========== */
typedef struct {
    TransactionLog logs[1000];
    int log_count;
    DatabaseTable *table;
    bool active;
    int savepoint_count;
    int savepoints[10];
} Transaction;

/* ========== 查询结果结构 ========== */
typedef struct {
    DataCell **rows;
    int row_count;
    int column_count;
    char **column_names;
    ErrorCode error;
    char error_msg[MAX_ERROR_MSG];
} QueryResult;

#ifdef __cplusplus
}
#endif

class SmartQueryResult {
public:
    // 使用unique_ptr管理二维数组
    std::unique_ptr<std::unique_ptr<DataCell[]>[]> rows;
    int row_count;
    int column_count;

    // 列名使用unique_ptr管理
    std::unique_ptr<std::unique_ptr<char[]>[]> column_names;
    ErrorCode error;
    char error_msg[MAX_ERROR_MSG];

    SmartQueryResult(int cols,
                     const std::vector<std::string>& col_names,
                     const std::vector<std::vector<DataCell>>& data) : column_count(cols)
    {
        column_names = std::make_unique<std::unique_ptr<char[]>[]>(column_count);
        for (int i = 0; i < column_count; i++) {
            if (i < static_cast<int>(col_names.size()) && !col_names[i].empty()) {
                column_names[i] = std::make_unique<char[]>(col_names[i].size() + 1);
                    std::copy(col_names[i].c_str(), col_names[i].c_str() + col_names[i].size() + 1, column_names[i].get());
            }
        }

        // 分配行数据
        row_count = data.size();
        rows = std::make_unique<std::unique_ptr<DataCell[]>[]>(row_count);
        for (int i = 0; i < row_count; ++i) {
            rows[i] = std::make_unique<DataCell[]>(column_count);
            for (int j = 0; j < column_count; ++j) {
                rows[i][j] = data[i][j];
            }
        }
    }

    // 获取列名
    const char* getColumnName(int col) const
    {
        if (col < 0 || col >= column_count || !column_names || !column_names[col]) {
            return "";
        }
        return column_names[col].get();
    }
};

#ifdef __cplusplus
extern "C" {
#endif

/* ========== 全局数据库状态 ========== */
typedef struct {
    DatabaseTable *tables[50];
    int table_count;
    Transaction active_transactions[MAX_TRANSACTION_DEPTH];
    int transaction_depth;
    char current_database[256];
    bool auto_commit;
    FILE *log_file;
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
    SmartBPlusTreeNode* CreateBPlusTreeNode(bool isLeaf, int order);
    void InsertIntoIndex(TableIndex *index, int key);
    void DeleteFromIndex(TableIndex *index, int key);
    SmartTableRow* FindTableRow(int rowId);

private:
    // DatabaseTable *table;
    std::unique_ptr<DatabaseTable> table;
    std::unique_ptr<SmartQueryResult> query_result;

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
