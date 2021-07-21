param (
    [string]$version = $(throw "-version is required."), # dataset version 
    [string]$aug = 'noAHE', # augmentation: AHEtrain or noAHE at the moment    
    [float]$IoU = 0.25, # IoU threshold for MAP computation
	[string]$yolovers = 'yolov3-spp',
    [switch]$testAsValidation = $false # True 1 or False 0
)

# if version contains folders e.g. vL.1s/AP26to35 then the / or \ should be replaced by _ <underscore>
$v = $version -replace "\\","_"
$v = $v -replace "/","_"


if (-Not (Test-Path "./backup/NAOS/$version/" -PathType Container)){
	throw "$version is not a valid option for -version."
}
if (-Not (Test-Path "./backup/NAOS/$version/$yolovers/$aug/" -PathType Container)){
	throw "$aug is not a valid option for -aug the parameter."
}

# check if we are on windows or Linux. If Linux we assume a headless system!
if ($Env:OS -match "Windows"){
	$darknetExe = "./darknet.exe"
	$isWindows = 1
} elseif ($isLinux) {
	$darknetExe = "./darknet"
	# isWindows is already set!
} else { # assume it is Linux, but an old version of powershell!
	$darknetExe = "./darknet"
	$isWindows = 0
}

if ($yolovers -match "yolov3-spp"){
	$dataFile = "./data/NAOS/$version/NAOS-$v-$aug.data" # default
	$dFbackup = "backup/NAOS/$version/$aug/" #"backup  = backup/NAOS/v0.1/AHEtrain/"
} else {
	$dataFile = "./data/NAOS/$version/$($yolovers)-NAOS-$v-$aug.data" # new data file for a different yolo version
	$dFbackup = "backup/NAOS/$version/$yolovers/$aug/" # e.g. "backup  = backup/NAOS/v0.1/yolov3-tiny/AHEtrain/"
}

$cfgfile = "./cfg/$($yolovers)-NAOS.cfg"

# get all weights in folder and sort by time they have been changed (sorting by name does not work, e.g. 100 1000 200 ... )
$weights = Get-Item "$dFbackup/*.weights" | Sort-Object -Property LastWriteTime

if( $weights.Length -le 0 ){
	throw "no weights found in folder $dFbackup"
}


$dFtrain = "data/NAOS/$version/NAOS-$v-$($aug)_train.txt"#"train   = data/NAOS/v0.1/NAOS-v0.1-AHEtrain_train.txt"
if($testAsValidation){ #use the test set as validation set
	$dataFile = "./data/NAOS/$version/$($yolovers)-NAOS-$v-$aug-testAsValidation.data"
	$dFvalid = "data/NAOS/$version/NAOS-$v-$($aug)_test.txt "#"valid   = data/NAOS/v0.1/NAOS-v0.1-AHEtrain_valid.txt"
	$longMapFile = "$dFbackup/all_map_results_NAOS-$aug-testAsValidation.txt"
	$mapFile = "$dFbackup/map_results_NAOS-$aug-testAsValidation.txt"
} else {
	$dFvalid = "data/NAOS/$version/NAOS-$v-$($aug)_valid.txt "#"valid   = data/NAOS/v0.1/NAOS-v0.1-AHEtrain_valid.txt"
	$longMapFile = "$dFbackup/all_map_results_NAOS-$aug.txt"
	$mapFile = "$dFbackup/map_results_NAOS-$aug.txt"
}
$dFbackup = "backup/NAOS/$version/$aug/" #"backup  = backup/NAOS/v0.1/AHEtrain/"

if (-Not (Test-Path $dataFile -PathType leaf)){

	# create a (new) data file 
	Set-Content -Path $dataFile -Value "classes = 1"
	Add-Content -Path $dataFile -Value "names   = data/thermal-persons.names"
	Add-Content -Path $dataFile -Value "saveweights = 200"
	Add-Content -Path $dataFile -Value "savelast 	= 100"
	Add-Content -Path $dataFile -Value "train   = $dFtrain"
	Add-Content -Path $dataFile -Value "valid   = $dFvalid"
	Add-Content -Path $dataFile -Value "backup  = $dFbackup"

	Write-Host "Created data file $dataFile."
}


if (Test-Path $longMapFile -PathType leaf){
	# make sure the file does not exist prior to writing
	Remove-Item $longMapFile
}


$i = 0
# compute mAP values with YOLO
foreach ($w in $weights){ 
	$per = ([math]::Round( $i/$weights.Length*100 ))
	Write-Progress -Activity "Compute MAP for backup weights" -Status "working on $($w.Name); $($per)%:" -PercentComplete $per 

	if ($w.Name -match "_200.weights"){ continue }# ignore this weight as it contains garbage and takes very long to evaluate!
	
	
	
	$res = iex "$darknetExe detector map $dataFile $cfgfile $($w.FullName) -iou_thresh $IoU" 
	
	Add-Content -Path $longMapFile -Value $w.FullName -Force
	Add-Content -Path $longMapFile -Value $res -Force
	
	$i ++

}

# write the values so that we can process them with Matlab
if( $isWindows ){
	Get-Content $longMapFile | select-string -pattern ".weights", "average IoU", "mAP\@$IoU\) \= " | Set-Content -Path $mapFile -Encoding utf8
} else {
	Get-Content $longMapFile | select-string -NoEmphasis -pattern ".weights", "average IoU", "mAP\@$IoU\) \= " | Set-Content -Path $mapFile
# use noEmphasis only on Linux machines!
}
