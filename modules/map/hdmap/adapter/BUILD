load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "opendrive_adapter",
    srcs = [
        "opendrive_adapter.cc",
    ],
    hdrs = [
        "opendrive_adapter.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":proto_organizer",
        "@wheelos_common//modules/common/math",
        "@wheelos_common//modules/common/status",
        "@wheelos_common//modules/common/util",
        "//modules/map/hdmap/adapter/xml_parser",
        "@com_google_absl//absl/strings",
        "@core//cyber/common:log",
        "@proj",
        "@tinyxml2",
        "@wheelos_msgs//wheelos_msgs/map_msgs:map_cc_proto",
    ],
)

cc_library(
    name = "proto_organizer",
    srcs = ["proto_organizer.cc"],
    hdrs = ["proto_organizer.h"],
    linkstatic = True,
    visibility = ["//visibility:private"],
    deps = [
        "@wheelos_common//modules/common/math",
        "@wheelos_common//modules/common/util",
        "//modules/map/hdmap/adapter/xml_parser",
        "@com_google_absl//absl/strings",
        "@core//cyber/common:log",
        "@wheelos_msgs//wheelos_msgs/map_msgs:map_cc_proto",
    ],
    alwayslink = True,
)
