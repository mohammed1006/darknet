param (
    [string[]]$versions = $(throw "-versions is required."), # dataset version. multiple as vL.0,vL.0z ...
	[string]$yolovers = 'yolov3-spp',
	[int[]]$gpus = 0,
	[switch]$onlyPrint = $False
)

foreach ($version in $versions){
	./NAOS_start_training.ps1 -version $version -aug AHEtrain -yolovers $yolovers -gpus $gpus -onlyPrint:$onlyPrint
	./NAOS_start_training.ps1 -version $version -aug noAHE -yolovers $yolovers -gpus $gpus -onlyPrint:$onlyPrint
}