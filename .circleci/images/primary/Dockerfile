FROM amd64/ubuntu:latest

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
        bison \
        llvm \
        gcc \
        g++ \
        gnupg \
        curl \
        cmake \
        ninja-build \
        clang \
        clang-format \
        clang-tidy \
        build-essential \
        xz-utils \
        git && \
    rm -rf /var/lib/apt/lists/*
