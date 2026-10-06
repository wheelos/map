/* Copyright 2017 The Apollo Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
=========================================================================*/

#pragma once

#include <memory>
#include <string>

#include "wheelos_msgs/map_msgs/map_id.pb.h"
#include "wheelos_msgs/planning_msgs/navigation.pb.h"
#include "modules/common/configs/config_gflags.h"
#include "modules/map/hdmap/hdmap.h"

/**
 * @namespace apollo::hdmap
 * @brief apollo::hdmap
 */
namespace apollo {
namespace hdmap {

/**
 * @brief get base map file path from the selected map bundle.
 * @return base map path
 */
std::string BaseMapFile();

/**
 * @brief get simulation map file path from the selected map bundle.
 * @return simulation map path
 */
std::string SimMapFile();

/**
 * @brief get routing map file path from the selected map bundle.
 * @return routing map path
 */
std::string RoutingMapFile();

/**
 * @brief get end way point file path from the selected map bundle.
 * @return end way point file path
 */
std::string EndWayPointFile();

/**
 * @brief get default routing file path associated with the selected map.
 * @return default routing points file path
 */
std::string DefaultRoutingFile();

/**
 * @brief get park and go routing file path associated with the selected map.
 * @return park and routng routings file path
 */
std::string ParkGoRoutingFile();

/**
 * @brief create a Map ID given a string.
 * @param id a string id
 * @return a Map ID instance
 */
inline apollo::hdmap::Id MakeMapId(const std::string& id) {
  apollo::hdmap::Id map_id;
  map_id.set_id(id);
  return map_id;
}

std::unique_ptr<HDMap> CreateMap(const std::string& map_file_path);

class HDMapUtil {
 public:
  // Get the selected base map bundle.
  // Return nullptr if failed to load.
  static const HDMap* BaseMapPtr();
  static const HDMap* BaseMapPtr(const relative_map::MapMsg& map_msg);
  // Guarantee to return a valid base_map, or else raise fatal error.
  static const HDMap& BaseMap();

  // Get the selected sim_map bundle.
  // Return nullptr if failed to load.
  static const HDMap* SimMapPtr();

  // Guarantee to return a valid sim_map, or else raise fatal error.
  static const HDMap& SimMap();

  // Reload maps from the persisted selected map.
  static bool ReloadMaps();

 private:
  HDMapUtil() = delete;

  static std::unique_ptr<HDMap> base_map_;
  static uint64_t base_map_seq_;
  static std::mutex base_map_mutex_;

  static std::unique_ptr<HDMap> sim_map_;
  static std::mutex sim_map_mutex_;
};

}  // namespace hdmap
}  // namespace apollo
