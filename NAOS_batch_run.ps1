param (
    [string[]]$versions = $(throw "-versions is required."), # dataset version. multiple as vL.0,vL.0z ...
	[string]$yolovers = 'yolov3-spp',
	[string[]]$augs = @('AHEtrain', 'noAHE'),
	[int[]]$gpus = 0,
	[switch]$onlyPrint = $False,
	[switch]$train = $False,
	[switch]$bmAP = $False # compute backup map scores
)

foreach ($version in $versions){
	foreach ($aug in $augs){
		if($train){
			./NAOS_start_training.ps1 -version $version -aug $aug -yolovers $yolovers -gpus $gpus -onlyPrint:$onlyPrint
		}
		if($bmAP){
			./NAOS_compute_backup_mAP.ps1 -version $version -aug $aug -yolovers $yolovers
		}
	}
}