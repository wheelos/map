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

#include <chrono>
#include <filesystem>
#include <fstream>

#include "gtest/gtest.h"
#include "modules/common/map/map_selection.h"

namespace apollo {
namespace hdmap {

class HDMapUtilTestSuite : public ::testing::Test {
 protected:
  void SetUp() override {
    original_base_map_filename_ = FLAGS_base_map_filename;
    original_test_base_map_filename_ = FLAGS_test_base_map_filename;
    map_directory_ =
        std::filesystem::temp_directory_path() /
        ("hdmap_util_test_" +
         std::to_string(std::chrono::steady_clock::now()
                            .time_since_epoch()
                            .count()));
    std::filesystem::create_directories(map_directory_);
    apollo::common::MapSelection::SetTestMapDirectory(
        map_directory_.string());
    FLAGS_base_map_filename = "missing.bin|base_map.bin";
    FLAGS_test_base_map_filename.clear();
  }

  void TearDown() override {
    apollo::common::MapSelection::ClearTestMapDirectory();
    FLAGS_base_map_filename = original_base_map_filename_;
    FLAGS_test_base_map_filename = original_test_base_map_filename_;
    std::error_code error;
    std::filesystem::remove_all(map_directory_, error);
  }

  void InitMapProto(Map* map_proto);

  std::filesystem::path map_directory_;
  std::string original_base_map_filename_;
  std::string original_test_base_map_filename_;
};

void HDMapUtilTestSuite::InitMapProto(Map* map_proto) {
  auto* lane = map_proto->add_lane();
  lane->mutable_id()->set_id("lane_1");
  CurveSegment* curve_segment = lane->mutable_central_curve()->add_segment();
  LineSegment* line_segment = curve_segment->mutable_line_segment();
  double delta_s = 0.2;
  double heading = M_PI / 2.0;
  double x0 = 0.0;
  double y0 = 0.0;
  for (double s = 0; s < 200; s += delta_s) {
    auto* pt = line_segment->add_point();
    pt->set_x(x0 + s * cos(heading));
    pt->set_y(y0 + s * sin(heading));
    auto* left_sample = lane->add_left_sample();
    left_sample->set_s(s);
    left_sample->set_width(1.5);
    auto* right_sample = lane->add_right_sample();
    right_sample->set_s(s);
    right_sample->set_width(1.5);
  }
  lane->set_type(Lane::CITY_DRIVING);
}

TEST_F(HDMapUtilTestSuite, BaseMapFileReturnsExistingCandidate) {
  const std::filesystem::path expected = map_directory_ / "base_map.bin";
  std::ofstream(expected).put('\0');

  EXPECT_EQ(expected.string(), BaseMapFile());
}

TEST_F(HDMapUtilTestSuite, BaseMapFileReturnsEmptyWhenCandidatesAreMissing) {
  EXPECT_TRUE(BaseMapFile().empty());
}

}  // namespace hdmap
}  // namespace apollo
