load("@rules_cc//cc:defs.bzl", "cc_binary")

package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "proto_map_generator",
    srcs = ["proto_map_generator.cc"],
    deps = [
        "//modules/map/hdmap:hdmap_util",
        "//modules/map/hdmap/adapter:opendrive_adapter",
        "@com_github_gflags_gflags//:gflags",
        "@core//cyber/common:file",
        "@core//cyber/common:log",
        "@wheelos_msgs//wheelos_msgs/map_msgs:map_cc_proto",
    ],
)

cc_binary(
    name = "sim_map_generator",
    srcs = ["sim_map_generator.cc"],
    deps = [
        "@wheelos_common//modules/common/configs:config_gflags",
        "@wheelos_common//modules/common/util",
        "//modules/map/hdmap:hdmap_util",
        "//modules/map/hdmap/adapter:opendrive_adapter",
        "@com_github_gflags_gflags//:gflags",
        "@com_google_absl//absl/strings",
        "@core//cyber/common:file",
        "@core//cyber/common:log",
        "@wheelos_msgs//wheelos_msgs/map_msgs:map_cc_proto",
    ],
)

cc_binary(
    name = "bin_map_generator",
    srcs = ["bin_map_generator.cc"],
    deps = [
        "//modules/map/hdmap:hdmap_util",
        "@com_github_gflags_gflags//:gflags",
        "@core//cyber/common:file",
        "@core//cyber/common:log",
        "@wheelos_msgs//wheelos_msgs/map_msgs:map_cc_proto",
    ],
)
