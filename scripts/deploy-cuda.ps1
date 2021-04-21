#!/usr/bin/env pwsh

$CudaUrl = 'https://developer.download.nvidia.com/compute/cuda/11.3.0/network_installers/cuda_11.3.0_win10_network.exe'

$CudaFeatures = 'nvcc_11.3 cuobjdump_11.3 nvprune_11.3 cupti_11.3 memcheck_11.3 nvdisasm_11.3 nvprof_11.3 ' + `
 'visual_studio_integration_11.3 visual_profiler_11.3 visual_profiler_11.3 cublas_11.3 cublas_dev_11.3 ' + `
 'cudart_11.3 cufft_11.3 cufft_dev_11.3 curand_11.3 curand_dev_11.3 cusolver_11.3 cusolver_dev_11.3 ' + `
 'cusparse_11.3 cusparse_dev_11.3 npp_11.3 npp_dev_11.3 nvrtc_11.3 nvrtc_dev_11.3 nvml_dev_11.3 ' + `
 'occupancy_calculator_11.3 '

try {
  Write-Host 'Downloading CUDA...'
  [string]$installerPath = Get-TempFilePath -Extension 'exe'
  curl.exe -L -o $installerPath -s -S $CudaUrl
  Write-Host 'Installing CUDA...'
  $proc = Start-Process -FilePath $installerPath -ArgumentList @('-s ' + $CudaFeatures) -Wait -PassThru
  $exitCode = $proc.ExitCode
  if ($exitCode -eq 0) {
    Write-Host 'Installation successful!'
  }
  else {
    Write-Error "Installation failed! Exited with $exitCode."
    throw
  }
}
catch {
  Write-Error "Failed to install CUDA! $($_.Exception.Message)"
  throw
}
