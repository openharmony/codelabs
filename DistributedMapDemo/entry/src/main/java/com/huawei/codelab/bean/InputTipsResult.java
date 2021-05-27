/*
 * Copyright (c) 2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.huawei.codelab.bean;

import java.util.List;

/**
 * InputTipsResult
 *
 * @since 2021-03-12
 */
public class InputTipsResult {
    private String count;

    private String infocode;

    private List<TipsEntity> tips;

    private String status;

    private String info;

    public void setCount(String count) {
        this.count = count;
    }

    public void setInfocode(String infocode) {
        this.infocode = infocode;
    }

    public void setTips(List<TipsEntity> tips) {
        this.tips = tips;
    }

    public void setStatus(String status) {
        this.status = status;
    }

    public void setInfo(String info) {
        this.info = info;
    }

    public String getCount() {
        return count;
    }

    public String getInfocode() {
        return infocode;
    }

    public List<TipsEntity> getTips() {
        return tips;
    }

    public String getStatus() {
        return status;
    }

    public String getInfo() {
        return info;
    }

    /**
     * TipsEntity
     *
     * @param <E> type
     * @since 2021-03-12
     */
    public class TipsEntity<E> {
        private String typecode;

        private E address;

        private E adcode;

        private List<?> city;

        private E district;

        private String name;

        private String location;

        private E id;

        public void setTypecode(String typecode) {
            this.typecode = typecode;
        }

        public void setAddress(E address) {
            this.address = address;
        }

        public void setAdcode(E adcode) {
            this.adcode = adcode;
        }

        public void setCity(List<?> city) {
            this.city = city;
        }

        public void setDistrict(E district) {
            this.district = district;
        }

        public void setName(String name) {
            this.name = name;
        }

        public void setLocation(String location) {
            this.location = location;
        }

        public void setId(E id) {
            this.id = id;
        }

        public String getTypecode() {
            return typecode;
        }

        public E getAddress() {
            return address;
        }

        public E getAdcode() {
            return adcode;
        }

        public List<?> getCity() {
            return city;
        }

        public E getDistrict() {
            return district;
        }

        public String getName() {
            return name;
        }

        public String getLocation() {
            return location;
        }

        public E getId() {
            return id;
        }
    }
}
