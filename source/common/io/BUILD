load(
    "//bazel:envoy_build_system.bzl",
    "envoy_cc_library",
    "envoy_package",
)

licenses(["notice"])  # Apache 2

envoy_package()

envoy_cc_library(
    name = "io_uring_impl_lib",
    srcs = [
        "io_uring_impl.cc",
    ],
    hdrs = [
        "io_uring_impl.h",
    ],
    external_deps = ["uring"],
    tags = ["nocompdb"],
    deps = [
        "//envoy/common/io:io_uring_interface",
        "//envoy/thread_local:thread_local_interface",
    ],
)
