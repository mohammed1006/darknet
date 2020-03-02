#!/bin/bash

sudo nvidia-docker run -it \
	-v /media/sambashare/Exchange/training_samples/synthetic:/darknet/data/synthetic \
	# docker image name
  12f9520d7112 \
