FROM nvidia/cuda:10.0-cudnn7-devel-ubuntu18.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y \
    libopencv-dev \
    python-dev \
    python3-pip \
    libgomp1 \
    wget \
    git cmake  && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

RUN mkdir darknet

RUN git clone https://github.com/fatal69100/darknet /darknet

RUN cd /darknet && cmake -L .