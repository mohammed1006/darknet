#!/usr/bin/env bash

install_tools=false

POSITIONAL=()
while [[ $# -gt 0 ]]
do
key="$1"

case $key in
    -InstallCUDA|--InstallCUDA)
    install_tools=true
    shift
    ;;
    *)    # unknown option
    POSITIONAL+=("$1") # save it in an array for later
    shift # past argument
    ;;
esac
done
set -- "${POSITIONAL[@]}" # restore positional parameters

script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
cd $script_dir/..
temp_folder="./temp"
mkdir -p $temp_folder
cd $temp_folder

if [ "$install_tools" = true ] ; then
  if [[ "$OSTYPE" == "darwin"* ]]; then
    echo "Unable to provide tools on macOS, please wait for a future script update or do not put -InstallCUDA command line flag to continue"
  else
    if [[ $(cut -f2 <<< $(lsb_release -r)) == "18.04" ]]; then
      sudo apt-get update
      sudo apt-get install git ninja-build build-essential g++
      sudo apt-get install apt-transport-https ca-certificates gnupg software-properties-common wget
      wget http://developer.download.nvidia.com/compute/cuda/repos/ubuntu1804/x86_64/cuda-repo-ubuntu1804_10.2.89-1_amd64.deb
      sudo apt-key adv --fetch-keys http://developer.download.nvidia.com/compute/cuda/repos/ubuntu1804/x86_64/7fa2af80.pub
      sudo dpkg -i cuda-repo-ubuntu1804_10.2.89-1_amd64.deb
      wget http://developer.download.nvidia.com/compute/machine-learning/repos/ubuntu1804/x86_64/nvidia-machine-learning-repo-ubuntu1804_1.0.0-1_amd64.deb
      sudo dpkg -i nvidia-machine-learning-repo-ubuntu1804_1.0.0-1_amd64.deb
      wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | sudo tee /etc/apt/trusted.gpg.d/kitware.gpg >/dev/null
      sudo apt-add-repository 'deb https://apt.kitware.com/ubuntu/ bionic main'
      wget -q https://packages.microsoft.com/config/ubuntu/18.04/packages-microsoft-prod.deb
      sudo dpkg -i packages-microsoft-prod.deb
      sudo add-apt-repository universe
      sudo apt-get update
      sudo apt-get dist-upgrade -y
      sudo apt-get install -y --no-install-recommends cuda-compiler-10-2 cuda-libraries-dev-10-2 cuda-driver-dev-10-2 cuda-cudart-dev-10-2 cuda-curand-dev-10-2
      sudo apt-get install -y --no-install-recommends libcudnn7-dev
      sudo apt-get install -y cmake
      sudo apt-get install -y powershell
      sudo rm -rf /usr/local/cuda
      sudo ln -s /usr/local/cuda-10.2 /usr/local/cuda
      export PATH=/usr/local/cuda/bin:$PATH
      export LD_LIBRARY_PATH=/usr/local/cuda/lib64:$LD_LIBRARY_PATH
      export CUDACXX=/usr/local/cuda/bin/nvcc
      export CUDA_PATH=/usr/local/cuda
      export CUDA_TOOLKIT_ROOT_DIR=/usr/local/cuda
      cuda_is_available=true
    elif [[ $(cut -f2 <<< $(lsb_release -r)) == "20.04" ]]; then
      sudo apt-get update
      sudo apt-get install git ninja-build build-essential g++
      sudo apt-get install apt-transport-https ca-certificates gnupg software-properties-common wget
      sudo wget -O /etc/apt/preferences.d/cuda-repository-pin-600 https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2004/x86_64/cuda-ubuntu2004.pin
      sudo apt-key adv --fetch-keys https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2004/x86_64/7fa2af80.pub
      wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | sudo tee /etc/apt/trusted.gpg.d/kitware.gpg >/dev/null
      sudo apt-add-repository 'deb https://apt.kitware.com/ubuntu/ focal main'
      sudo add-apt-repository "deb http://developer.download.nvidia.com/compute/cuda/repos/ubuntu2004/x86_64/ /"
      sudo add-apt-repository "deb http://developer.download.nvidia.com/compute/machine-learning/repos/ubuntu2004/x86_64/ /"
      wget -q https://packages.microsoft.com/config/ubuntu/20.04/packages-microsoft-prod.deb
      sudo dpkg -i packages-microsoft-prod.deb
      sudo add-apt-repository universe
      sudo apt-get update
      sudo apt-get dist-upgrade -y
      sudo apt-get install -y --no-install-recommends cuda-compiler-11-2 cuda-libraries-dev-11-2 cuda-driver-dev-11-2 cuda-cudart-dev-11-2
      sudo apt-get install -y --no-install-recommends libcudnn8-dev
      sudo apt-get install -y cmake
      sudo apt-get install -y powershell
      sudo rm -rf /usr/local/cuda
      sudo ln -s /usr/local/cuda-11.2 /usr/local/cuda
      export PATH=/usr/local/cuda/bin:$PATH
      export LD_LIBRARY_PATH=/usr/local/cuda/lib64:$LD_LIBRARY_PATH
      export CUDACXX=/usr/local/cuda/bin/nvcc
      export CUDA_PATH=/usr/local/cuda
      export CUDA_TOOLKIT_ROOT_DIR=/usr/local/cuda
      cuda_is_available=true
    else
      echo "Unable to provide tools on macOS, please wait for a future script update or do not put -InstallCUDA command line flag to continue"
    fi
  fi
fi

cd ..
rm -rf "$temp_folder"

if [[ -v CUDA_PATH ]]; then
  ./build.ps1 -UseVCPKG -EnableOPENCV -EnableCUDA -EnableCUDNN -DisableInteractive -DoNotUpdateDARKNET
  #./build.ps1 -UseVCPKG -EnableOPENCV -EnableCUDA -EnableCUDNN -EnableOPENCV_CUDA -DisableInteractive -DoNotUpdateDARKNET
else
  ./build.ps1 -UseVCPKG -EnableOPENCV -DisableInteractive -DoNotUpdateDARKNET
fi
