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
#include "modules/map/hdmap/hdmap_util.h"

#include <string>
#include <vector>

#include "absl/strings/str_split.h"
#include "cyber/common/file.h"
#include "modules/common/map/map_selection.h"

namespace apollo {
namespace hdmap {

using apollo::relative_map::MapMsg;

namespace {

// Find the first existing file from a list of candidates: "file_a|file_b|...".
bool GetSelectedMapDirectory(std::string* map_directory) {
  apollo::common::SelectedMap selected_map;
  if (!apollo::common::MapSelection::GetSelectedMap(&selected_map)) {
    return false;
  }
  *map_directory = selected_map.directory;
  return true;
}

std::string FindFirstExist(const std::string& map_directory,
                           const std::string& files) {
  const std::vector<std::string> candidates = absl::StrSplit(files, '|');
  for (const auto& filename : candidates) {
    if (filename.empty()) {
      continue;
    }
    const std::string file_path =
        absl::StrCat(map_directory, "/", filename);
    if (cyber::common::PathExists(file_path)) {
      return file_path;
    }
  }
  AERROR << "No existing map file found in " << map_directory << " for "
         << files;
  return "";
}

}  // namespace

std::string BaseMapFile() {
  std::string map_directory;
  if (!GetSelectedMapDirectory(&map_directory)) {
    return "";
  }
  if (FLAGS_use_navigation_mode) {
    AWARN << "base_map file is not used when FLAGS_use_navigation_mode is true";
  }
  return FLAGS_test_base_map_filename.empty()
             ? FindFirstExist(map_directory, FLAGS_base_map_filename)
             : FindFirstExist(map_directory, FLAGS_test_base_map_filename);
}

std::string SimMapFile() {
  std::string map_directory;
  if (!GetSelectedMapDirectory(&map_directory)) {
    return "";
  }
  if (FLAGS_use_navigation_mode) {
    AWARN << "sim_map file is not used when FLAGS_use_navigation_mode is true";
  }
  return FindFirstExist(map_directory, FLAGS_sim_map_filename);
}

std::string RoutingMapFile() {
  std::string map_directory;
  if (!GetSelectedMapDirectory(&map_directory)) {
    return "";
  }
  if (FLAGS_use_navigation_mode) {
    AWARN << "routing_map file is not used when FLAGS_use_navigation_mode is "
             "true";
  }
  return FindFirstExist(map_directory, FLAGS_routing_map_filename);
}

std::string EndWayPointFile() {
  if (FLAGS_use_navigation_mode) {
    return FLAGS_navigation_mode_end_way_point_file;
  }
  std::string map_directory;
  if (!GetSelectedMapDirectory(&map_directory)) {
    return "";
  }
  return absl::StrCat(map_directory, "/", FLAGS_end_way_point_filename);
}

std::string DefaultRoutingFile() {
  std::string map_directory;
  if (!GetSelectedMapDirectory(&map_directory)) {
    return "";
  }
  // TODO(map assets): Move auxiliary routing files into the selected bundle.
  return absl::StrCat(map_directory, "_", FLAGS_default_routing_filename);
}

std::string ParkGoRoutingFile() {
  std::string map_directory;
  if (!GetSelectedMapDirectory(&map_directory)) {
    return "";
  }
  // TODO(map assets): Move auxiliary routing files into the selected bundle.
  return absl::StrCat(map_directory, "_", FLAGS_park_go_routing_filename);
}

std::unique_ptr<HDMap> CreateMap(const std::string& map_file_path) {
  std::unique_ptr<HDMap> hdmap(new HDMap());
  if (hdmap->LoadMapFromFile(map_file_path) != 0) {
    AERROR << "Failed to load HDMap " << map_file_path;
    return nullptr;
  }
  AINFO << "Load HDMap success: " << map_file_path;
  return hdmap;
}

std::unique_ptr<HDMap> CreateMap(const MapMsg& map_msg) {
  std::unique_ptr<HDMap> hdmap(new HDMap());
  if (hdmap->LoadMapFromProto(map_msg.hdmap()) != 0) {
    AERROR << "Failed to load RelativeMap: "
           << map_msg.header().ShortDebugString();
    return nullptr;
  }
  return hdmap;
}

std::unique_ptr<HDMap> HDMapUtil::base_map_ = nullptr;
uint64_t HDMapUtil::base_map_seq_ = 0;
std::mutex HDMapUtil::base_map_mutex_;

std::unique_ptr<HDMap> HDMapUtil::sim_map_ = nullptr;
std::mutex HDMapUtil::sim_map_mutex_;

const HDMap* HDMapUtil::BaseMapPtr(const MapMsg& map_msg) {
  std::lock_guard<std::mutex> lock(base_map_mutex_);
  if (base_map_ != nullptr &&
      base_map_seq_ == map_msg.header().sequence_num()) {
    // avoid re-create map in the same cycle.
    return base_map_.get();
  } else {
    base_map_ = CreateMap(map_msg);
    base_map_seq_ = map_msg.header().sequence_num();
  }
  return base_map_.get();
}

const HDMap* HDMapUtil::BaseMapPtr() {
  // TODO(all) Those logics should be removed to planning
  /*if (FLAGS_use_navigation_mode) {
    std::lock_guard<std::mutex> lock(base_map_mutex_);
    auto* relative_map = AdapterManager::GetRelativeMap();
    if (!relative_map) {
      AERROR << "RelativeMap adapter is not registered";
      return nullptr;
    }
    if (relative_map->Empty()) {
      AERROR << "RelativeMap is empty";
      return nullptr;
    }
    const auto& latest = relative_map->GetLatestObserved();
    if (base_map_ != nullptr &&
        base_map_seq_ == latest.header().sequence_num()) {
      // avoid re-create map in the same cycle.
      return base_map_.get();
    } else {
      base_map_ = CreateMap(latest);
      base_map_seq_ = latest.header().sequence_num();
    }
  } else*/
  if (base_map_ == nullptr) {
    std::lock_guard<std::mutex> lock(base_map_mutex_);
    if (base_map_ == nullptr) {  // Double check.
      base_map_ = CreateMap(BaseMapFile());
    }
  }
  return base_map_.get();
}

const HDMap& HDMapUtil::BaseMap() { return *CHECK_NOTNULL(BaseMapPtr()); }

const HDMap* HDMapUtil::SimMapPtr() {
  if (FLAGS_use_navigation_mode) {
    return BaseMapPtr();
  } else if (sim_map_ == nullptr) {
    std::lock_guard<std::mutex> lock(sim_map_mutex_);
    if (sim_map_ == nullptr) {  // Double check.
      sim_map_ = CreateMap(SimMapFile());
    }
  }
  return sim_map_.get();
}

const HDMap& HDMapUtil::SimMap() { return *CHECK_NOTNULL(SimMapPtr()); }

bool HDMapUtil::ReloadMaps() {
  {
    std::lock_guard<std::mutex> lock(base_map_mutex_);
    base_map_ = CreateMap(BaseMapFile());
  }
  {
    std::lock_guard<std::mutex> lock(sim_map_mutex_);
    sim_map_ = CreateMap(SimMapFile());
  }
  return base_map_ != nullptr && sim_map_ != nullptr;
}

}  // namespace hdmap
}  // namespace apollo
