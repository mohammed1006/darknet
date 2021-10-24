#!/usr/bin/env bash

if [[ "$OSTYPE" == "darwin"* ]]; then
  echo "Unable to deploy CUDA on macOS, please wait for a future script update"
  exit 1
elif [[ $(cut -f2 <<< $(lsb_release -i)) == "Ubuntu" ]]; then
  if [[ $(cut -f2 <<< $(lsb_release -r)) == "18.04" ]]; then
    distr_name="ubuntu1804"
  elif [[ $(cut -f2 <<< $(lsb_release -r)) == "20.04" ]]; then
    distr_name="ubuntu2004"
  else
    echo "Unable to deploy CUDA on this Ubuntu version, please wait for a future script update"
    exit 2
  fi
else
  echo "Unable to deploy CUDA on this OS, please wait for a future script update"
  exit 3
fi

sudo apt-get update
sudo apt-get install build-essential g++
sudo apt-get install apt-transport-https ca-certificates gnupg software-properties-common wget
sudo wget -O /etc/apt/preferences.d/cuda-repository-pin-600 https://developer.download.nvidia.com/compute/cuda/repos/$distr_name/x86_64/cuda-$distr_name.pin
sudo apt-key adv --fetch-keys https://developer.download.nvidia.com/compute/cuda/repos/$distr_name/x86_64/7fa2af80.pub
sudo add-apt-repository "deb http://developer.download.nvidia.com/compute/cuda/repos/$distr_name/x86_64/ /"
sudo add-apt-repository "deb http://developer.download.nvidia.com/compute/machine-learning/repos/$distr_name/x86_64/ /"
sudo apt-get update
sudo apt-get dist-upgrade -y
sudo apt-get install -y --no-install-recommends cuda-compiler-11-5 cuda-libraries-dev-11-5 cuda-driver-dev-11-5 cuda-cudart-dev-11-5
sudo apt-get install -y --no-install-recommends libcudnn8-dev
sudo rm -rf /usr/local/cuda
sudo ln -s /usr/local/cuda-11.5 /usr/local/cuda
