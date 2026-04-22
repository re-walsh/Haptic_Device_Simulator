# Multi-stage Dockerfile for C++ development environment
# Targets Linux x86 architecture with build tools for the HapticDevice library

FROM ubuntu:22.04

# Prevent interactive prompts during build
ENV DEBIAN_FRONTEND=noninteractive
ENV VCPKG_ROOT=/vcpkg
ENV VCPKG_DISABLE_METRICS=1

# Use CMake's kitware to get the latest CMake
RUN apt-get update && apt-get install -y --no-install-recommends \
ca-certificates \
curl \
gpg \
&& curl https://apt.kitware.com/keys/kitware-archive-latest.asc | gpg --dearmor - | tee /etc/apt/trusted.gpg.d/kitware.gpg >/dev/null \
&& echo "deb https://apt.kitware.com/ubuntu/ jammy main" | tee /etc/apt/sources.list.d/kitware.list >/dev/null \
&& apt-get update

# Install system dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    sudo \
    build-essential \
    cmake \
    ninja-build \
    git \
    curl \
    wget \
    pkg-config \
    clang \
    lld \
    ca-certificates \
    python3 \
    python3-pip \
    gdb \
    zip \
    unzip \
    tar \
    libgl1-mesa-dev \
    libglx-dev \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxi-dev \
    libx11-dev \
    libglu1-mesa-dev \
    mesa-common-dev \
    libxxf86vm-dev \
    libboost-dev \
    tree \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# --- ROS 2 INSTALL ---
# make sure UTF-8 is supported
RUN apt update && apt install -y locales \ 
    && locale-gen en_US en_US.UTF-8 \
    && update-locale LC_ALL=en_US.UTF-8 LANG=en_US.UTF-8 \
    && export LANG=en_US.UTF-8

# ensure ubuntu universe is installed
RUN apt-get install -y software-properties-common \  
    && add-apt-repository universe

# get the ros 2 apt packages
RUN sudo apt update && sudo apt install curl -y \
    && export ROS_APT_SOURCE_VERSION=$(curl -s https://api.github.com/repos/ros-infrastructure/ros-apt-source/releases/latest | grep -F "tag_name" | awk -F'"' '{print $4}') \
    && curl -L -o /tmp/ros2-apt-source.deb "https://github.com/ros-infrastructure/ros-apt-source/releases/download/${ROS_APT_SOURCE_VERSION}/ros2-apt-source_${ROS_APT_SOURCE_VERSION}.$(. /etc/os-release && echo ${UBUNTU_CODENAME:-${VERSION_CODENAME}})_all.deb" \
    && sudo dpkg -i /tmp/ros2-apt-source.deb

# install ros 2
RUN apt update && apt install -y --no-install-recommends \
    ros-dev-tools \
    ros-humble-desktop \
    ros-humble-ros-gz-sim \ 
    ros-humble-ros2-control \ 
    ros-humble-ros2-controllers \ 
    ros-humble-gz-ros2-control \
    ros-humble-joint-state-publisher-gui

# Install vcpkg
RUN git clone https://github.com/Microsoft/vcpkg.git ${VCPKG_ROOT} \
    && cd ${VCPKG_ROOT} \
    && ./bootstrap-vcpkg.sh -disableMetrics

# Create a vcpkg overlay ports directory for custom ports if needed
RUN mkdir -p ${VCPKG_ROOT}/overlays/ports

# Set working directory
WORKDIR /workspace

# Create mount points for source code
VOLUME ["/workspace/src"]

# Set environment for CMake
ENV CMAKE_BUILD_TYPE=Release
ENV PATH="${VCPKG_ROOT}:${PATH}"

# Default command: bash shell for interactive development
CMD ["/bin/bash"]
