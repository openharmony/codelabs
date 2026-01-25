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
#include "database_ops.h"
#include <memory>
#include <algorithm>
#include <sstream>
#include <iomanip>
#define SECUREC_ENABLE 1  // 启用安全函数
#define ZERO 0
#define THREE 2
// C接口实现
extern "C" {
time_t ParseDate(const char *dateStr)
{
    struct tm tm = {0};
    if (strptime(dateStr, "%Y-%m-%d", &tm) != nullptr) {
        return mktime(&tm);
    }
    return 0;
}

DataCell CreateDataCell(DataType type, const char *value)
{
    DataCell cell;
    cell.type = type;

    if (value == nullptr || strcasecmp(value, "NULL") == 0) {
        cell.isNull = true;
        return cell;
    }

    cell.isNull = false;

    switch (type) {
        case TYPE_INT:
            cell.value.intVal = std::stoi(value);
            break;
        case TYPE_FLOAT:
            cell.value.floatVal = std::stod(value);
            break;
        case TYPE_STRING:
            // 使用std::copy替代strncpy
            std::copy_n(value, MAX_STRING_LEN - 1, cell.value.stringVal);
            cell.value.stringVal[MAX_STRING_LEN - 1] = '\0';
            break;
        case TYPE_BOOL:
            cell.value.boolVal = (strcasecmp(value, "true") == 0 ||
                                   strcasecmp(value, "1") == 0);
            break;
        case TYPE_DATE:
            cell.value.dateVal = ParseDate(value);
            break;
        default:
            cell.isNull = true;
            break;
    }

    return cell;
}

} // extern "C"

/* ========== 辅助函数 ========== */
const char* DataBaseOps::DataTypeToString(DataType type)
{
    switch (type) {
        case TYPE_INT:
            return "INT";
        case TYPE_FLOAT:
            return "FLOAT";
        case TYPE_STRING:
            return "STRING";
        case TYPE_BOOL:
            return "BOOL";
        case TYPE_DATE:
            return "DATE";
        default:
            return "UNKNOWN";
    }
}

DataType DataBaseOps::StringToDataType(const char *str)
{
    if (strcasecmp(str, "int") == 0) {
        return TYPE_INT;
    }
    if (strcasecmp(str, "float") == 0) {
        return TYPE_FLOAT;
    }
    if (strcasecmp(str, "string") == 0) {
        return TYPE_STRING;
    }
    if (strcasecmp(str, "bool") == 0) {
        return TYPE_BOOL;
    }
    if (strcasecmp(str, "date") == 0) {
        return TYPE_DATE;
    }
    return TYPE_NULL;
}

char* DataBaseOps::FormatDate(time_t timestamp)
{
    static const int dateBufferSize = 20;
    static char buffer[dateBufferSize];
    struct tm *tmInfo = localtime(&timestamp);
    size_t result = strftime(buffer, dateBufferSize, "%Y-%m-%d %H:%M:%S", tmInfo);
    if (result == 0) {
        // 处理返回值为0的情况，例如设置默认值或返回错误信息
        buffer[0] = '\0';
        return buffer;
    }
    return buffer;
}

void DataBaseOps::FreeDataCell(DataCell *cell)
{
    if (cell) {
        if (cell->type == TYPE_STRING && !cell->isNull) {
            // 字符串在栈上，无需释放
        }
    }
}

int DataBaseOps::CompareDataCells(const DataCell *a, const DataCell *b)
{
    if (a->isNull && b->isNull) {
        return 0;
    }
    if (a->isNull) {
        return -1;
    }
    if (b->isNull) {
        return 1;
    }

    if (a->type != b->type) {
        return static_cast<int>(a->type) - static_cast<int>(b->type);
    }

    switch (a->type) {
        case TYPE_INT:
            return a->value.intVal - b->value.intVal;
        case TYPE_FLOAT:
            if (a->value.floatVal < b->value.floatVal) {
                return -1;
            }
            if (a->value.floatVal > b->value.floatVal) {
                return 1;
            }
            return 0;
        case TYPE_STRING:
            return strcmp(a->value.stringVal, b->value.stringVal);
        case TYPE_BOOL:
            return a->value.boolVal - b->value.boolVal;
        case TYPE_DATE:
            if (a->value.dateVal < b->value.dateVal) {
                return -1;
            }
            if (a->value.dateVal > b->value.dateVal) {
                return 1;
            }
            return 0;
        default:
            return 0;
    }
}

SmartBPlusTreeNode* DataBaseOps::CreateBplusTreeNode(bool isLeaf, int order)
{
    std::shared_ptr<SmartBPlusTreeNode> sNode = std::make_shared<SmartBPlusTreeNode>(order, isLeaf);
    if (!sNode) {
        return nullptr;
    }

    return sNode.get();
}

TableIndex* DataBaseOps::CreateTableIndex(const char *columnName)
{
    const int bplusTreeOrder = 4;
    if (table->indexCount >= MAX_INDEXES) {
        return nullptr;
    }

    // 查找列
    int colIndex = -1;
    for (int i = 0; i < table->columnCount; i++) {
        if (strcmp(table->columns[i].name, columnName) == 0) {
            colIndex = i;
            break;
        }
    }

    if (colIndex == -1) {
        return nullptr;
    }

    TableIndex *index = &table->indexes[table->indexCount];
    // 使用std::copy替代strncpy
    std::copy_n(columnName, MAX_COLUMN_NAME - 1, index->name);
    index->columnIndex = colIndex;
    index->root = CreateBplusTreeNode(true, bplusTreeOrder);
    index->height = 1;
    index->keyCount = 0;

    table->columns[colIndex].hasIndex = true;
    table->indexCount++;

    return index;
}

void DataBaseOps::DeleteFromIndex(TableIndex *index, int key)
{
    const int formatBufferSize = 32;
    char buffer[formatBufferSize];
    // 使用std::ostringstream替代snprintf
    std::ostringstream oss;
    oss << "Index delete: key=" << key << "\n";
    std::string message = oss.str();
    std::copy(message.begin(), message.end(), buffer);
    buffer[message.size()] = '\0';
    printf("%s", buffer);
    index->keyCount--;
}

DataBaseOps::DataBaseOps(const char *tableName, ColumnDef *columns, int columnCount)
{
    const int initialRowId = 1;
    table = std::make_unique<DatabaseTable>();
    if (!table) {
        printf("no memory for DatabaseTable pointer");
    }

    // 使用value initialization替代memset
    *table.get() = DatabaseTable{};
    // 使用std::copy替代strncpy
    std::copy_n(tableName, MAX_TABLE_NAME - 1, table->name);
    table->createdAt = time(nullptr);
    table->nextRowId = initialRowId;

    // 复制列定义
    for (int i = 0; i < columnCount; i++) {
        table->columns[i] = columns[i];
    }
    table->columnCount = columnCount;

    // 为每一行分配单元格内存
    for (int i = 0; i < MAX_ROWS; i++) {
        SmartTableRow tableRow(columnCount);
        table->rows[i] = tableRow;
    }
}

DataBaseOps::~DataBaseOps() {}

SmartTableRow* DataBaseOps::FindTableRow(int rowId)
{
    for (int i = 0; i < table->rowCount; i++) {
        if (!table->rows[i].deleted && table->rows[i].rowId == rowId) {
            return &table->rows[i];
        }
    }
    return nullptr;
}

int DataBaseOps::InsertTableRow(DataCell *cells)
{
    if (table->rowCount >= MAX_ROWS) {
        return -1;
    }

    for (int i = 0; i < MAX_ROWS; i++) {
        if (table->rows[i].deleted || table->rows[i].rowId == 0) {
            SmartTableRow *row = &table->rows[i];

            // 复制数据
            for (int j = 0; j < table->columnCount; j++) {
                row->cells[j] = cells[j];
            }

            row->rowId = table->nextRowId++;
            row->deleted = false;
            row->createdAt = time(nullptr);
            row->updatedAt = row->createdAt;
            table->rowCount++;
            return row->rowId;
        }
    }

    return -1;
}

bool DataBaseOps::UpdateTableRow(int rowId, DataCell *newCells)
{
    SmartTableRow *row = FindTableRow(rowId);
    if (!row) {
        return false;
    }

    // 保存旧数据（用于事务回滚）
    DataCell *oldCells = static_cast<DataCell*>(malloc(sizeof(DataCell) * table->columnCount));
    if (!oldCells) {
        return false;
    }

    // 使用std::copy替代memcpy
    std::copy(row->cells.get(), row->cells.get() + table->columnCount, oldCells);

    // 更新数据
    for (int i = 0; i < table->columnCount; i++) {
        if (!newCells[i].isNull) {
            row->cells[i] = newCells[i];
        }
    }

    row->updatedAt = time(nullptr);

    free(oldCells);
    return true;
}

bool DataBaseOps::DeleteTableRow(int rowId)
{
    SmartTableRow *row = FindTableRow(rowId);
    if (!row) {
        return false;
    }

    // 更新索引
    for (int i = 0; i < table->indexCount; i++) {
        TableIndex *index = &table->indexes[i];
        DeleteFromIndex(index, rowId);
    }

    row->deleted = true;
    table->rowCount--;

    return true;
}

bool DataBaseOps::BeginTransaction()
{
    if (table->inTransaction) {
        return false;
    }

    table->inTransaction = true;
    return true;
}

bool DataBaseOps::CommitTransaction()
{
    if (!table->inTransaction) {
        return false;
    }

    table->inTransaction = false;
    return true;
}

bool DataBaseOps::RollbackTransaction()
{
    if (!table->inTransaction) {
        return false;
    }

    // 在实际系统中需要回滚所有更改
    table->inTransaction = false;
    return true;
}

void DataBaseOps::PrintTableSchema()
{
    int schemaColumnWidth = 20;
    int schemaTypeWidth = 10;
    int schemaFlagWidth = 8;

    printf("\n=== 表结构: %s ===\n", table->name);
    printf("创建时间: %s\n", FormatDate(table->createdAt));
    printf("行数: %d\n", table->rowCount);
    printf("列数: %d\n\n", table->columnCount);

    printf("%-*s %-*s %-*s %-*s %-*s\n",
        schemaColumnWidth, "列名",
        schemaTypeWidth, "类型",
        schemaFlagWidth, "非空",
        schemaFlagWidth, "主键",
        schemaFlagWidth, "索引");
    printf("%s\n", "------------------------------------------------------------");

    for (int i = 0; i < table->columnCount; i++) {
        ColumnDef *col = &table->columns[i];
        printf("%-*s %-*s %-*s %-*s %-*s\n",
            schemaColumnWidth, col->name,
            schemaTypeWidth, DataTypeToString(col->type),
            schemaFlagWidth, col->notNull ? "YES" : "NO",
            schemaFlagWidth, col->isPrimary ? "YES" : "NO",
            schemaFlagWidth, col->hasIndex ? "YES" : "NO");
    }
}

void DataBaseOps::PrintQueryResult(SmartQueryResult *result)
{
    if (!result || result->error != ERR_NONE) {
        printf("查询错误: %s\n", result->errorMsg);
        return;
    }
    printf("\n查询结果 (%d 行):\n", result->rowCount);
}

SmartQueryResult* DataBaseOps::ExecuteSelectQuery(const char *whereClause)
{
    const int maxQueryResults = 100;
    std::vector<std::string> colNames;
    for (int i = 0; i < table->columnCount; i++) {
        std::string strTemp(table->columns[i].name);
        colNames.push_back(strTemp);
    }
    // 收集符合条件的行
    std::vector<std::vector<DataCell>> data;
    int rowCount = 0;
    for (int i = 0; i < maxQueryResults; i++) {
        if (!table->rows[i].deleted && table->rows[i].rowId > 0) {
            // 简化：暂时不考虑WHERE条件
            DataCell *pCell = table->rows[i].cells.get();
            std::vector<DataCell> dataCellVec;
            for (int j = 0; j < table->columnCount; j++) {
                DataCell item = pCell[j];
                dataCellVec.push_back(item);
            }
            data.push_back(dataCellVec);
            rowCount++;
        }
    }
    queryResult = std::make_unique<SmartQueryResult>(table->columnCount, colNames, data);
    return queryResult.get();
}
// C接口实现
extern "C" {
// 辅助函数: 打印系统信息
static void PrintSystemInfo(int maxTableCount)
{
    printf("=== 简易关系型数据库系统启动 ===\n");
    printf("版本: 1.0.0\n");
    printf("最大表数: %d\n", maxTableCount);
    printf("最大行数: %d\n", MAX_ROWS);
    printf("最大列数: %d\n", MAX_COLUMNS);
    printf("页面大小: %d bytes\n\n", PAGE_SIZE);
}
// 辅助函数: 打印总结信息
static void PrintSummary(int operationCount)
{
    printf("\n数据库系统正常关闭。\n");
    printf("总计执行操作: %d\n", operationCount);
}

// 函数1: 创建用户表结构
static std::unique_ptr<DataBaseOps> CreateUserTable(int& operationCount)
{
    const int userColumnCount = 8;

    printf("创建示例表: users\n");

    ColumnDef userColumns[] = {
        {"id", TYPE_INT, 0, true, true, false, "0"},
        {"username", TYPE_STRING, 50, true, false, true, ""},
        {"email", TYPE_STRING, 100, true, false, true, ""},
        {"age", TYPE_INT, 0, false, false, false, "0"},
        {"salary", TYPE_FLOAT, 0, false, false, false, "0.0"},
        {"is_active", TYPE_BOOL, 0, false, false, false, "true"},
        {"created_at", TYPE_DATE, 0, false, false, false, ""},
        {"updated_at", TYPE_DATE, 0, false, false, false, ""}
    };

    std::unique_ptr<DataBaseOps> ops = std::make_unique<DataBaseOps>("users", userColumns, userColumnCount);
    if (!ops) {
        printf("创建表失败！\n");
        return nullptr;
    }

    // 创建索引
    ops->CreateTableIndex("username");
    ops->CreateTableIndex("email");
    operationCount += THREE; // 两次创建索引操作

    return ops;
}

// 函数2: 插入示例数据并执行事务
static int InsertUserSampleData(DataBaseOps* ops, int& operationCount)
{
    if (!ops) {
        return -1;
    }

    // 开始事务
    printf("\n开始事务...\n");
    ops->BeginTransaction();
    operationCount++;

    // 插入示例数据
    printf("\n插入示例数据...\n");

    const int userColumnCount = 8;
    DataCell user1[userColumnCount] = {
        CreateDataCell(TYPE_INT, "1"),
        CreateDataCell(TYPE_STRING, "alice"),
        CreateDataCell(TYPE_STRING, "alice@example.com"),
        CreateDataCell(TYPE_INT, "25"),
        CreateDataCell(TYPE_FLOAT, "50000.0"),
        CreateDataCell(TYPE_BOOL, "true"),
        CreateDataCell(TYPE_DATE, "2023-10-20"),
        CreateDataCell(TYPE_DATE, "2023-10-20")
    };

    DataCell user2[userColumnCount] = {
        CreateDataCell(TYPE_INT, "2"),
        CreateDataCell(TYPE_STRING, "bob"),
        CreateDataCell(TYPE_STRING, "bob@example.com"),
        CreateDataCell(TYPE_INT, "30"),
        CreateDataCell(TYPE_FLOAT, "60000.0"),
        CreateDataCell(TYPE_BOOL, "true"),
        CreateDataCell(TYPE_DATE, "2023-10-21"),
        CreateDataCell(TYPE_DATE, "2023-10-21")
    };

    int id1 = ops->InsertTableRow(user1);
    int id2 = ops->InsertTableRow(user2);
    operationCount += 3; // 假设THREE是3

    printf("插入完成: id1=%d, id2=%d\n", id1, id2);

    // 提交事务
    printf("\n提交事务...\n");
    ops->CommitTransaction();
    operationCount++;
    return (id1 > 0 && id2 > 0) ? 0 : -1;
}

// 函数3: 主函数 - 调用其他函数并控制流程
int DatabaseOpsDemo()
{
    const int maxTableCount = 50;
    const int formatBufferSize = 32;
    int operationCount = 0;

    // 打印系统信息
    PrintSystemInfo(maxTableCount);
    // 创建用户表
    auto ops = CreateUserTable(operationCount);
    if (!ops) {
        return -1;
    }

    // 插入示例数据
    if (InsertUserSampleData(ops.get(), operationCount) < 0) {
        printf("插入示例数据失败！\n");
        return -1;
    }

    // 显示表结构
    ops->PrintTableSchema();

    // 查询所有数据
    printf("\n执行查询: SELECT * FROM users\n");
    SmartQueryResult *result = ops->ExecuteSelectQuery("");
    if (result) {
        ops->PrintQueryResult(result);
        // 假设需要释放result
        // delete result; 或使用适当的释放方法
    }
    operationCount++;
    // 打印总结信息
    PrintSummary(operationCount);
    return 0; // 假设ZERO是0
}

} // extern "C"