#!/bin/bash -e

DOCKER_CACHE_PATH="$1"
NO_MOUNT_TMPFS="${2:-}"
DOCKER_CACHE_OWNERSHIP="vsts:vsts"
TMPFS_SIZE=5G

if [[ -z "$DOCKER_CACHE_PATH" ]]; then
    echo "prepare_docker_cache called without path arg" >&2
    exit 1
fi

if ! id -u vsts &> /dev/null; then
    DOCKER_CACHE_OWNERSHIP=azure-pipelines
fi

echo "Creating cache directory (${DOCKER_CACHE_PATH}) ..."
mkdir -p "${DOCKER_CACHE_PATH}"
if [[ -z "$NO_MOUNT_TMPFS" ]]; then
    echo "Mount tmpfs directory: ${DOCKER_CACHE_PATH}"
    mount -o size="$TMPFS_SIZE" -t tmpfs none "$DOCKER_CACHE_PATH"
fi
mkdir -p "${DOCKER_CACHE_PATH}/docker"
mkdir -p "${DOCKER_CACHE_PATH}/bazel"
chown -R "$DOCKER_CACHE_OWNERSHIP" "${DOCKER_CACHE_PATH}"
