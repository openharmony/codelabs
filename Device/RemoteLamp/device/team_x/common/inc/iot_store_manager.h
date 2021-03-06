
/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef __IOT_STORE_MANAGER_H__
#define __IOT_STORE_MANAGER_H__

#define USER_KV_STORE   1
#if (USER_KV_STORE)
int StoreManagerAddData(const char *data, int length);

int StoreManagerUpdateData(int idx, const char *data, int length);

int StoreManagerDeleteDataWithId(int idx);

int StoreManagerDeleteData(const char *data);

int StoreManagerGetTotal(void);

int StoreManagerGetData(char *data, int size, int idx);

void StoreManagerDelete(void);

#endif

#endif  /* __IOT_STORE_MANAGER_H__ */
