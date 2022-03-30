/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __IOT_LIST_H__
#define __IOT_LIST_H__

typedef void* IOT_LIST;

IOT_LIST IoT_ListCreate(void *data, int size);

int IoT_ListAppend(IOT_LIST mHandle, void *data, int size);

int IoT_ListUpdate(IOT_LIST mHandle, int idx, void *data, int size);

int IoT_ListDelete(IOT_LIST mHandle, void *data);

int IoT_ListGet(IOT_LIST mHandle, int idx, void *data, int size);

int IoT_ListGetSize(IOT_LIST mHandle);

void IoT_ListClear(IOT_LIST mHandle);

void IoT_ListDestroy(IOT_LIST mHandle);

#endif  /* __IOT_LIST_H__ */
