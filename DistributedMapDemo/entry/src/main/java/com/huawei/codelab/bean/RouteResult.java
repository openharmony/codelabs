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
 * RouteResult
 *
 * @since 2021-03-12
 */
public class RouteResult {
    private RouteEntity route;

    private String count;

    private String infocode;

    private String status;

    private String info;

    public void setRoute(RouteEntity route) {
        this.route = route;
    }

    public void setCount(String count) {
        this.count = count;
    }

    public void setInfocode(String infocode) {
        this.infocode = infocode;
    }

    public void setStatus(String status) {
        this.status = status;
    }

    public void setInfo(String info) {
        this.info = info;
    }

    public RouteEntity getRoute() {
        return route;
    }

    public String getCount() {
        return count;
    }

    public String getInfocode() {
        return infocode;
    }

    public String getStatus() {
        return status;
    }

    public String getInfo() {
        return info;
    }

    /**
     * RouteEntity
     *
     * @since 2021-03-12
     */
    public class RouteEntity {
        private List<PathsEntity> paths;

        private String origin;

        private String destination;

        private String taxiCost;

        public void setPaths(List<PathsEntity> paths) {
            this.paths = paths;
        }

        public void setOrigin(String origin) {
            this.origin = origin;
        }

        public void setDestination(String destination) {
            this.destination = destination;
        }

        public void setTaxiCost(String taxiCost) {
            this.taxiCost = taxiCost;
        }

        public List<PathsEntity> getPaths() {
            return paths;
        }

        public String getOrigin() {
            return origin;
        }

        public String getDestination() {
            return destination;
        }

        public String getTaxiCost() {
            return taxiCost;
        }

        /**
         * PathsEntity
         *
         * @since 2021-03-12
         */
        public class PathsEntity {
            private String duration;

            private String distance;

            private String restriction;

            private String tollDistance;

            private String strategy;

            private String trafficLights;

            private List<StepsEntity> steps;

            private String tolls;

            public void setDuration(String duration) {
                this.duration = duration;
            }

            public void setDistance(String distance) {
                this.distance = distance;
            }

            public void setRestriction(String restriction) {
                this.restriction = restriction;
            }

            public void setTollDistance(String tollDistance) {
                this.tollDistance = tollDistance;
            }

            public void setStrategy(String strategy) {
                this.strategy = strategy;
            }

            public void setTrafficLights(String trafficLights) {
                this.trafficLights = trafficLights;
            }

            public void setSteps(List<StepsEntity> steps) {
                this.steps = steps;
            }

            public void setTolls(String tolls) {
                this.tolls = tolls;
            }

            public String getDuration() {
                return duration;
            }

            public String getDistance() {
                return distance;
            }

            public String getRestriction() {
                return restriction;
            }

            public String getTollDistance() {
                return tollDistance;
            }

            public String getStrategy() {
                return strategy;
            }

            public String getTrafficLights() {
                return trafficLights;
            }

            public List<StepsEntity> getSteps() {
                return steps;
            }

            public String getTolls() {
                return tolls;
            }

            /**
             * StepsEntity
             *
             * @param <E> type
             * @since 2021-03-12
             */
            public class StepsEntity<E> {
                private String orientation;

                private String distance;

                private List<CitiesEntity> cities;

                private E tollRoad;

                private String tollDistance;

                private String tolls;

                private String duration;

                private E assistantAction;

                private String road;

                private String instruction;

                private E action;

                private String polyline;

                private List<TmcsEntity> tmcs;

                public void setOrientation(String orientation) {
                    this.orientation = orientation;
                }

                public void setDistance(String distance) {
                    this.distance = distance;
                }

                public void setCities(List<CitiesEntity> cities) {
                    this.cities = cities;
                }

                public void setTollRoad(E tollRoad) {
                    this.tollRoad = tollRoad;
                }

                public void setTollDistance(String tollDistance) {
                    this.tollDistance = tollDistance;
                }

                public void setTolls(String tolls) {
                    this.tolls = tolls;
                }

                public void setDuration(String duration) {
                    this.duration = duration;
                }

                public void setAssistantAction(E assistantAction) {
                    this.assistantAction = assistantAction;
                }

                public void setRoad(String road) {
                    this.road = road;
                }

                public void setInstruction(String instruction) {
                    this.instruction = instruction;
                }

                public void setAction(E action) {
                    this.action = action;
                }

                public void setPolyline(String polyline) {
                    this.polyline = polyline;
                }

                public void setTmcs(List<TmcsEntity> tmcs) {
                    this.tmcs = tmcs;
                }

                public String getOrientation() {
                    return orientation;
                }

                public String getDistance() {
                    return distance;
                }

                public List<CitiesEntity> getCities() {
                    return cities;
                }

                public E getTollRoad() {
                    return tollRoad;
                }

                public String getTollDistance() {
                    return tollDistance;
                }

                public String getTolls() {
                    return tolls;
                }

                public String getDuration() {
                    return duration;
                }

                public E getAssistantAction() {
                    return assistantAction;
                }

                public String getRoad() {
                    return road;
                }

                public String getInstruction() {
                    return instruction;
                }

                public E getAction() {
                    return action;
                }

                public String getPolyline() {
                    return polyline;
                }

                public List<TmcsEntity> getTmcs() {
                    return tmcs;
                }

                /**
                 * CitiesEntity
                 *
                 * @since 2021-03-12
                 */
                public class CitiesEntity {
                    private String citycode;

                    private String adcode;

                    private String name;

                    private List<DistrictsEntity> districts;

                    public void setCitycode(String citycode) {
                        this.citycode = citycode;
                    }

                    public void setAdcode(String adcode) {
                        this.adcode = adcode;
                    }

                    public void setName(String name) {
                        this.name = name;
                    }

                    public void setDistricts(List<DistrictsEntity> districts) {
                        this.districts = districts;
                    }

                    public String getCitycode() {
                        return citycode;
                    }

                    public String getAdcode() {
                        return adcode;
                    }

                    public String getName() {
                        return name;
                    }

                    public List<DistrictsEntity> getDistricts() {
                        return districts;
                    }

                    /**
                     * DistrictsEntity
                     *
                     * @since 2021-03-12
                     */
                    public class DistrictsEntity {
                        private String adcode;

                        private String name;

                        public void setAdcode(String adcode) {
                            this.adcode = adcode;
                        }

                        public void setName(String name) {
                            this.name = name;
                        }

                        public String getAdcode() {
                            return adcode;
                        }

                        public String getName() {
                            return name;
                        }
                    }
                }

                /**
                 * TmcsEntity
                 *
                 * @since 2021-03-12
                 */
                public class TmcsEntity {
                    private String distance;

                    private E lcode;

                    private String polyline;

                    private String status;

                    public void setDistance(String distance) {
                        this.distance = distance;
                    }

                    public void setLcode(E lcode) {
                        this.lcode = lcode;
                    }

                    public void setPolyline(String polyline) {
                        this.polyline = polyline;
                    }

                    public void setStatus(String status) {
                        this.status = status;
                    }

                    public String getDistance() {
                        return distance;
                    }

                    public E getLcode() {
                        return lcode;
                    }

                    public String getPolyline() {
                        return polyline;
                    }

                    public String getStatus() {
                        return status;
                    }
                }
            }
        }
    }
}
