load(
    "//bazel:envoy_build_system.bzl",
    "envoy_cc_library",
    "envoy_package",
)

licenses(["notice"])  # Apache 2

envoy_package()

envoy_cc_library(
    name = "access_log_interface",
    hdrs = ["access_log.h"],
    deps = [
        "//envoy/config:typed_config_interface",
        "//envoy/filesystem:filesystem_interface",
        "//envoy/http:header_map_interface",
        "//envoy/stream_info:stream_info_interface",
        "//source/common/protobuf",
        "@envoy_api//envoy/data/accesslog/v3:pkg_cc_proto",
    ],
)
