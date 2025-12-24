#include "FunctionClass.h"
#include "BankQueueSystem.h"
#include "native_log_wrapper.h"
#include <string>
#include <unistd.h>

FunctionClass::FunctionClass(uint32_t version) {
    LOGI("FunctionClass::~FunctionClass %d",version);
}

FunctionClass::~FunctionClass() {
    LOGI("FunctionClass::~FunctionClass");
}

class Test {
public:
    int add(int a, int b) {
        return a + b; 
    }
};

void BankBusiness()
{
    usleep(100 * 1000);
    LOGI("saving or withdraw ordinary customer");
}

void BankBusinessVIP()
{
    usleep(100 * 1000);
    LOGI("saving or withdraw VIP");
}

void FunctionClass::FfrtSerialQueue(){
    // type传1，代表串行调度
    BankQueueSystem bankQueue(1, "Bank", 2);

    auto task1 = bankQueue.Enter(BankBusiness, "customer1", ffrt_queue_priority_low, 0);
    auto task2 = bankQueue.Enter(BankBusiness, "customer2", ffrt_queue_priority_low, 0);
    // VIP享受更优先的服务
    auto task3 = bankQueue.Enter(BankBusinessVIP, "customer3 vip", ffrt_queue_priority_high, 0);
    auto task4 = bankQueue.Enter(BankBusiness, "customer4", ffrt_queue_priority_low, 0);
    auto task5 = bankQueue.Enter(BankBusiness, "customer5", ffrt_queue_priority_low, 0);

    // 取消客户4的服务
    bankQueue.Exit(task4);

    // 等待所有的客户服务完成
    bankQueue.Wait(task5);
    LOGI("FfrtQueue results ");
}

void FunctionClass::FfrtConcurrentQueue(){
    // type传0，代表并发调度
    BankQueueSystem bankQueue(0, "Bank", 2);

    auto task1 = bankQueue.Enter(BankBusiness, "customer1", ffrt_queue_priority_low, 0);
    // VIP享受更优先的服务
    auto task2 = bankQueue.Enter(BankBusinessVIP, "customer3 vip", ffrt_queue_priority_high, 0);

    // 取消客户4的服务
    bankQueue.Exit(task1);

    // 等待所有的客户服务完成
    bankQueue.Wait(task2);
    LOGI("FfrtQueue results ");
}
