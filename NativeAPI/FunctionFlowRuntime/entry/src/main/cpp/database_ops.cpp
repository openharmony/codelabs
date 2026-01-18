//
// Created on 2026/1/12.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "database_ops.h"
#include <memory>

// C接口实现
extern "C" {

time_t parse_date(const char *date_str) {
    struct tm tm = {0};
    if (strptime(date_str, "%Y-%m-%d", &tm) != NULL) {
        return mktime(&tm);
    }
    return 0;
}

DataCell create_data_cell(DataType type, const char *value) {
    DataCell cell;
    memset((void *)&cell, 0, sizeof(cell));
    cell.type = type;
    
    if (value == NULL || strcasecmp(value, "NULL") == 0) {
        cell.is_null = true;
        return cell;
    }
    
    cell.is_null = false;
    
    switch(type) {
        case TYPE_INT:
            cell.value.int_val = atoi(value);
            break;
        case TYPE_FLOAT:
            cell.value.float_val = atof(value);
            break;
        case TYPE_STRING:
            strncpy(cell.value.string_val, value, MAX_STRING_LEN - 1);
            cell.value.string_val[MAX_STRING_LEN - 1] = '\0';
            break;
        case TYPE_BOOL:
            cell.value.bool_val = (strcasecmp(value, "true") == 0 || 
                                   strcasecmp(value, "1") == 0);
            break;
        case TYPE_DATE:
            cell.value.date_val = parse_date(value);
            break;
        default:
            cell.is_null = true;
            break;
    }
    
    return cell;
}

} // extern "C"

/* ========== 辅助函数 ========== */
const char* DataBaseOps::data_type_to_string(DataType type) {
    switch(type) {
        case TYPE_INT: return "INT";
        case TYPE_FLOAT: return "FLOAT";
        case TYPE_STRING: return "STRING";
        case TYPE_BOOL: return "BOOL";
        case TYPE_DATE: return "DATE";
        default: return "UNKNOWN";
    }
}

DataType DataBaseOps::string_to_data_type(const char *str) {
    if (strcasecmp(str, "int") == 0) return TYPE_INT;
    if (strcasecmp(str, "float") == 0) return TYPE_FLOAT;
    if (strcasecmp(str, "string") == 0) return TYPE_STRING;
    if (strcasecmp(str, "bool") == 0) return TYPE_BOOL;
    if (strcasecmp(str, "date") == 0) return TYPE_DATE;
    return TYPE_NULL;
}

char* DataBaseOps::format_date(time_t timestamp) {
    static char buffer[20];
    struct tm *tm_info = localtime(&timestamp);
    strftime(buffer, 20, "%Y-%m-%d %H:%M:%S", tm_info);
    return buffer;
}

void DataBaseOps::free_data_cell(DataCell *cell) {
    if (cell) {
        if (cell->type == TYPE_STRING && !cell->is_null) {
            // 字符串在栈上，无需释放
        }
    }
}

int DataBaseOps::compare_data_cells(const DataCell *a, const DataCell *b) {
    if (a->is_null && b->is_null) return 0;
    if (a->is_null) return -1;
    if (b->is_null) return 1;
    
    if (a->type != b->type) {
        return (int)a->type - (int)b->type;
    }
    
    switch(a->type) {
        case TYPE_INT:
            return a->value.int_val - b->value.int_val;
        case TYPE_FLOAT:
            if (a->value.float_val < b->value.float_val) return -1;
            if (a->value.float_val > b->value.float_val) return 1;
            return 0;
        case TYPE_STRING:
            return strcmp(a->value.string_val, b->value.string_val);
        case TYPE_BOOL:
            return a->value.bool_val - b->value.bool_val;
        case TYPE_DATE:
            if (a->value.date_val < b->value.date_val) return -1;
            if (a->value.date_val > b->value.date_val) return 1;
            return 0;
        default:
            return 0;
    }
}

SmartBPlusTreeNode* DataBaseOps::create_bplus_tree_node(bool is_leaf, int order) {
    std::shared_ptr<SmartBPlusTreeNode> s_node = std::make_shared<SmartBPlusTreeNode>(order, is_leaf);
    if (!s_node) return NULL;
 
    return s_node.get();
}

TableIndex* DataBaseOps::create_table_index(const char *column_name) {
    if (table->index_count >= MAX_INDEXES) {
        return NULL;
    }

    // 查找列
    int col_index = -1;
    for (int i = 0; i < table->column_count; i++) {
        if (strcmp(table->columns[i].name, column_name) == 0) {
            col_index = i;
            break;
        }
    }
    
    if (col_index == -1) {
        return NULL;
    }
    
    TableIndex *index = &table->indexes[table->index_count];
    strncpy(index->name, column_name, MAX_COLUMN_NAME - 1);
    index->column_index = col_index;
    index->root = create_bplus_tree_node(true, 4); // 使用4阶B+树
    index->height = 1;
    index->key_count = 0;
    
    table->columns[col_index].has_index = true;
    table->index_count++;
    
    return index;
}

void DataBaseOps::insert_into_index(TableIndex *index, int key) {
    // 简化实现：在实际系统中需要实现完整的B+树插入算法
    // 这里仅做演示
    printf("Index insert: key=%d\n", key);
    index->key_count++;
}

void DataBaseOps::delete_from_index(TableIndex *index, int key) {
    printf("Index delete: key=%d\n", key);
    index->key_count--;
}

DataBaseOps::DataBaseOps(const char *table_name, ColumnDef *columns, int column_count) {
    table = std::make_unique<DatabaseTable>();
    if (!table) {
        printf("no memory for DatabaseTable pointer");
    }

    memset((void *)table.get(), 0, sizeof(DatabaseTable));
    strncpy(table->name, table_name, MAX_TABLE_NAME - 1);
    table->created_at = time(NULL);
    table->next_row_id = 1;
    
    // 复制列定义
    for (int i = 0; i < column_count; i++) {
        table->columns[i] = columns[i];
    }
    table->column_count = column_count;
    
    // 为每一行分配单元格内存
    for (int i = 0; i < MAX_ROWS; i++) {
        SmartTableRow table_row(column_count);
        table->rows[i] = table_row;
    }
}

DataBaseOps::~DataBaseOps() {}

SmartTableRow* DataBaseOps::find_table_row(int row_id) {
    for (int i = 0; i < table->row_count; i++) {
        if (!table->rows[i].deleted && table->rows[i].row_id == row_id) {
            return &table->rows[i];
        }
    }
    return NULL;
}

int DataBaseOps::insert_table_row(DataCell *cells) {
    if (table->row_count >= MAX_ROWS) {
        return -1;
    }
    
    for (int i = 0; i < MAX_ROWS; i++) {
        if (table->rows[i].deleted || table->rows[i].row_id == 0) {
            SmartTableRow *row = &table->rows[i];
            
            // 复制数据
            for (int j = 0; j < table->column_count; j++) {
                row->cells[j] = cells[j];
            }
            
            row->row_id = table->next_row_id++;
            row->deleted = false;
            row->created_at = time(NULL);
            row->updated_at = row->created_at;
            
            table->row_count++;
            
            // 更新索引
            for (int j = 0; j < table->index_count; j++) {
                TableIndex *index = &table->indexes[j];
                if (index->column_index < table->column_count) {
                    // 简化：使用行ID作为索引键
                    insert_into_index(index, row->row_id);
                }
            }
            
            return row->row_id;
        }
    }
    
    return -1;
}

bool DataBaseOps::update_table_row(int row_id, DataCell *new_cells) {
    SmartTableRow *row = find_table_row(row_id);
    if (!row) return false;
    
    // 保存旧数据（用于事务回滚）
    DataCell *old_cells = (DataCell*)malloc(sizeof(DataCell) * table->column_count);
    if (!old_cells) return false;
    
    memcpy(old_cells, row->cells.get(), sizeof(DataCell) * table->column_count);
    
    // 更新数据
    for (int i = 0; i < table->column_count; i++) {
        if (!new_cells[i].is_null) {
            row->cells[i] = new_cells[i];
        }
    }
    
    row->updated_at = time(NULL);

    free(old_cells);
    return true;
}

bool DataBaseOps::delete_table_row(int row_id) {
    SmartTableRow *row = find_table_row(row_id);
    if (!row) return false;
    
    // 更新索引
    for (int i = 0; i < table->index_count; i++) {
        TableIndex *index = &table->indexes[i];
        delete_from_index(index, row_id);
    }
    
    row->deleted = true;
    table->row_count--;
    
    return true;
}

bool DataBaseOps::begin_transaction() {
    if (table->in_transaction) {
        return false;
    }
    
    table->in_transaction = true;
    return true;
}

bool DataBaseOps::commit_transaction() {
    if (!table->in_transaction) {
        return false;
    }
    
    table->in_transaction = false;
    return true;
}

bool DataBaseOps::rollback_transaction() {
    if (!table->in_transaction) {
        return false;
    }
    
    // 在实际系统中需要回滚所有更改
    table->in_transaction = false;
    return true;
}

void DataBaseOps::print_table_schema() {
    printf("\n=== 表结构: %s ===\n", table->name);
    printf("创建时间: %s\n", format_date(table->created_at));
    printf("行数: %d\n", table->row_count);
    printf("列数: %d\n\n", table->column_count);
    
    printf("%-20s %-10s %-8s %-8s %-8s\n", 
           "列名", "类型", "非空", "主键", "索引");
    printf("%s\n", "------------------------------------------------------------");
    
    for (int i = 0; i < table->column_count; i++) {
        ColumnDef *col = &table->columns[i];
        printf("%-20s %-10s %-8s %-8s %-8s\n",
               col->name,
               data_type_to_string(col->type),
               col->not_null ? "YES" : "NO",
               col->is_primary ? "YES" : "NO",
               col->has_index ? "YES" : "NO");
    }
}

void DataBaseOps::print_query_result(SmartQueryResult *result) {
    if (!result || result->error != ERR_NONE) {
        printf("查询错误: %s\n", result->error_msg);
        return;
    }
    
    printf("\n查询结果 (%d 行):\n", result->row_count);
    
    // 打印表头
    for (int i = 0; i < result->column_count; i++) {
        printf("%-20s", result->getColumnName(i));
        if (i < result->column_count - 1) printf(" | ");
    }
    printf("\n");
    
    // 打印分隔线
    for (int i = 0; i < result->column_count; i++) {
        for (int j = 0; j < 20; j++) printf("-");
        if (i < result->column_count - 1) printf("-+-");
    }
    printf("\n");
    
    // 打印数据
    for (int i = 0; i < result->row_count; i++) {
        for (int j = 0; j < result->column_count; j++) {
            DataCell *cell = &result->rows[i][j];
            
            if (cell->is_null) {
                printf("%-20s", "NULL");
            } else {
                char buffer[21] = {0};
                switch(cell->type) {
                    case TYPE_INT:
                        snprintf(buffer, 20, "%d", cell->value.int_val);
                        break;
                    case TYPE_FLOAT:
                        snprintf(buffer, 20, "%.2f", cell->value.float_val);
                        break;
                    case TYPE_STRING:
                        snprintf(buffer, 20, "%s", cell->value.string_val);
                        break;
                    case TYPE_BOOL:
                        snprintf(buffer, 20, "%s", cell->value.bool_val ? "true" : "false");
                        break;
                    case TYPE_DATE:
                        strncpy(buffer, format_date(cell->value.date_val), 20);
                        break;
                    default:
                        strncpy(buffer, "UNKNOWN", 20);
                }
                printf("%-20s", buffer);
            }
            
            if (j < result->column_count - 1) printf(" | ");
        }
        printf("\n");
    }
}

SmartQueryResult* DataBaseOps::execute_select_query(const char *where_clause) {
    std::vector<std::string> col_names;
    for (int i = 0; i < table->column_count; i++) {
        std::string str_temp(table->columns[i].name);
        col_names.push_back(str_temp);
    }

    // 收集符合条件的行
    int max_results = 100;
    std::vector<std::vector<DataCell>> data;
    int row_count = 0;
    
    for (int i = 0; i < max_results; i++) {
        if (!table->rows[i].deleted && table->rows[i].row_id > 0) {
            // 简化：暂时不考虑WHERE条件
            DataCell *p_cell = table->rows[i].cells.get();
            std::vector<DataCell> data_cell_vec;
            for (int j = 0; j < table->column_count; j++) {
                DataCell item = p_cell[j];
                data_cell_vec.push_back(item);
            }
            data.push_back(data_cell_vec);
            row_count++;
        }
    }
    
    query_result = std::make_unique<SmartQueryResult>(table->column_count, col_names, data);
    return query_result.get();
}

// C接口实现
extern "C" {

int database_ops_demo() {
    int operation_count = 0;
    printf("=== 简易关系型数据库系统启动 ===\n");
    printf("版本: 1.0.0\n");
    printf("最大表数: %d\n", 50);
    printf("最大行数: %d\n", MAX_ROWS);
    printf("最大列数: %d\n", MAX_COLUMNS);
    printf("页面大小: %d bytes\n\n", PAGE_SIZE);
    
    // 创建示例表：用户表
    printf("创建示例表: users\n");
    
    ColumnDef user_columns[] = {
        {"id", TYPE_INT, 0, true, true, false, "0"},
        {"username", TYPE_STRING, 50, true, false, true, ""},
        {"email", TYPE_STRING, 100, true, false, true, ""},
        {"age", TYPE_INT, 0, false, false, false, "0"},
        {"salary", TYPE_FLOAT, 0, false, false, false, "0.0"},
        {"is_active", TYPE_BOOL, 0, false, false, false, "true"},
        {"created_at", TYPE_DATE, 0, false, false, false, ""},
        {"updated_at", TYPE_DATE, 0, false, false, false, ""}
    };
    
    std::unique_ptr<DataBaseOps> ops = std::make_unique<DataBaseOps>("users", user_columns, 8);
    if (!ops) {
        printf("创建表失败！\n");
        return -1;
    }
    
    // 创建索引
    ops->create_table_index("username");
    ops->create_table_index("email");
    
    // 开始事务
    printf("\n开始事务...\n");
    ops->begin_transaction();
    operation_count++;
    
    // 插入示例数据
    printf("\n插入示例数据...\n");
    
    DataCell user1[8] = {
        create_data_cell(TYPE_INT, "1"),
        create_data_cell(TYPE_STRING, "john_doe"),
        create_data_cell(TYPE_STRING, "john@example.com"),
        create_data_cell(TYPE_INT, "30"),
        create_data_cell(TYPE_FLOAT, "50000.50"),
        create_data_cell(TYPE_BOOL, "true"),
        create_data_cell(TYPE_DATE, "2023-01-15"),
        create_data_cell(TYPE_DATE, "2023-10-20")
    };
    
    DataCell user2[8] = {
        create_data_cell(TYPE_INT, "2"),
        create_data_cell(TYPE_STRING, "jane_smith"),
        create_data_cell(TYPE_STRING, "jane@example.com"),
        create_data_cell(TYPE_INT, "25"),
        create_data_cell(TYPE_FLOAT, "60000.75"),
        create_data_cell(TYPE_BOOL, "true"),
        create_data_cell(TYPE_DATE, "2023-02-20"),
        create_data_cell(TYPE_DATE, "2023-10-21")
    };
    
    DataCell user3[8] = {
        create_data_cell(TYPE_INT, "3"),
        create_data_cell(TYPE_STRING, "bob_johnson"),
        create_data_cell(TYPE_STRING, "bob@example.com"),
        create_data_cell(TYPE_INT, "35"),
        create_data_cell(TYPE_FLOAT, "75000.00"),
        create_data_cell(TYPE_BOOL, "false"),
        create_data_cell(TYPE_DATE, "2023-03-10"),
        create_data_cell(TYPE_DATE, "2023-09-15")
    };
    
    int id1 = ops->insert_table_row(user1);
    int id2 = ops->insert_table_row(user2);
    int id3 = ops->insert_table_row(user3);
    operation_count += 3;
    
    printf("插入完成: id1=%d, id2=%d, id3=%d\n", id1, id2, id3);
    
    // 提交事务
    printf("\n提交事务...\n");
    ops->commit_transaction();
    operation_count++;
    
    // 显示表结构
    ops->print_table_schema();
    
    // 查询所有数据
    printf("\n执行查询: SELECT * FROM users\n");
    SmartQueryResult *result = ops->execute_select_query("");
    if (result) {
        ops->print_query_result(result);
    }
    operation_count++;
    
    // 更新数据
    printf("\n更新数据: UPDATE users SET salary = 80000.00 WHERE id = 3\n");
    
    DataCell update_cells[8];
    memset((void *)update_cells, 0, sizeof(update_cells));

    for (int i = 0; i < 8; i++) {
        update_cells[i].is_null = true;
    }
    update_cells[4] = create_data_cell(TYPE_FLOAT, "80000.00");
    update_cells[7] = create_data_cell(TYPE_DATE, "2023-10-22");
    
    if (ops->update_table_row(3, update_cells)) {
        printf("更新成功！\n");
        operation_count++;
    }
    
    // 再次查询
    printf("\n再次查询更新后的数据:\n");
    result = ops->execute_select_query("");
    if (result) {
        ops->print_query_result(result);
    }
    operation_count++;
    
    // 开始新事务测试回滚
    printf("\n=== 测试事务回滚 ===\n");
    ops->begin_transaction();
    
    printf("插入测试数据...\n");
    DataCell test_user[8] = {
        create_data_cell(TYPE_INT, "4"),
        create_data_cell(TYPE_STRING, "test_user"),
        create_data_cell(TYPE_STRING, "test@example.com"),
        create_data_cell(TYPE_INT, "99"),
        create_data_cell(TYPE_FLOAT, "99999.99"),
        create_data_cell(TYPE_BOOL, "true"),
        create_data_cell(TYPE_DATE, "2023-10-22"),
        create_data_cell(TYPE_DATE, "2023-10-22")
    };
    
    int test_id = ops->insert_table_row(test_user);
    printf("插入测试用户，ID = %d\n", test_id);
    operation_count++;
    
    // 回滚事务
    printf("\n回滚事务...\n");
    ops->rollback_transaction();
    operation_count++;
    
    // 验证数据已回滚
    printf("\n验证回滚后数据（应无测试用户）:\n");
    result = ops->execute_select_query("");
    if (result) {
        printf("找到 %d 行数据\n", result->row_count);
    }
    operation_count++;
    
    // 创建第二个表：订单表
    printf("\n创建第二个表: orders\n");
    
    ColumnDef order_columns[] = {
        {"order_id", TYPE_INT, 0, true, true, false, "0"},
        {"user_id", TYPE_INT, 0, true, false, true, "0"},
        {"product_name", TYPE_STRING, 100, true, false, false, ""},
        {"quantity", TYPE_INT, 0, true, false, false, "1"},
        {"price", TYPE_FLOAT, 0, true, false, false, "0.0"},
        {"order_date", TYPE_DATE, 0, true, false, false, ""},
        {"status", TYPE_STRING, 20, true, false, false, "pending"}
    };
    
    std::unique_ptr<DataBaseOps> order_ops = std::make_unique<DataBaseOps>("orders", order_columns, 8);
    if (!order_ops) {
        printf("创建表失败！\n");
        return -1;
    }
    
    // 插入订单数据
    printf("插入订单数据...\n");
    
    DataCell order1[7] = {
        create_data_cell(TYPE_INT, "1001"),
        create_data_cell(TYPE_INT, "1"),
        create_data_cell(TYPE_STRING, "Laptop"),
        create_data_cell(TYPE_INT, "1"),
        create_data_cell(TYPE_FLOAT, "1200.00"),
        create_data_cell(TYPE_DATE, "2023-10-01"),
        create_data_cell(TYPE_STRING, "delivered")
    };
    
    DataCell order2[7] = {
        create_data_cell(TYPE_INT, "1002"),
        create_data_cell(TYPE_INT, "2"),
        create_data_cell(TYPE_STRING, "Mouse"),
        create_data_cell(TYPE_INT, "2"),
        create_data_cell(TYPE_FLOAT, "50.00"),
        create_data_cell(TYPE_DATE, "2023-10-05"),
        create_data_cell(TYPE_STRING, "shipped")
    };

    DataCell order3[7] = {
        create_data_cell(TYPE_INT, "1003"),
        create_data_cell(TYPE_INT, "1"),
        create_data_cell(TYPE_STRING, "Keyboard"),
        create_data_cell(TYPE_INT, "1"),
        create_data_cell(TYPE_FLOAT, "80.00"),
        create_data_cell(TYPE_DATE, "2023-10-10"),
        create_data_cell(TYPE_STRING, "pending")
    };
    
    order_ops->insert_table_row(order1);
    order_ops->insert_table_row(order2);
    order_ops->insert_table_row(order3);
    operation_count += 3;
    
    // 显示订单表
    order_ops->print_table_schema();

    printf("\n订单表数据:\n");
    result = order_ops->execute_select_query("");
    if (result) {
        order_ops->print_query_result(result);
    }
    operation_count++;
    
    printf("\n数据库系统正常关闭。\n");
    printf("总计执行操作: %d\n", operation_count);
    return 0;
}

} // extern "C"

