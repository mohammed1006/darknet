param (
    [string]$version = $(throw "-version is required."), # dataset version 
    [string]$aug = 'noAHE', # augmentation: AHEtrain or noAHE at the moment    
	[string]$yolovers = 'yolov3-spp',
	[int[]]$gpus = 0,
	[switch]$onlyPrint = $False
)

# if version contains folders e.g. vL.1s/AP26to35 then the / or \ should be replaced by _ <underscore>
$v = $version -replace "\\","_"
$v = $v -replace "/","_"


if (-Not (Test-Path "./data/NAOS/$version/" -PathType Container)){
	throw "$version is not a valid option for -version."
}
if (-Not (Test-Path "./backup/NAOS/$version/$aug/" -PathType Container)){
	throw "$aug is not a valid option for -aug the parameter."
}

# select the start weigth based on the yolo version
if ( ($yolovers -eq "yolov3-spp") -or ($yolovers -eq "yolov3") ){
	$startweight = "darknet53.conv.74"
} elseif ($yolovers -eq "yolov3-tiny"){
	$startweight = "yolov3-tiny.conv.11"
} elseif ($yolovers -eq "yolov4"){
	$startweight = "yolov4.conv.137"
} elseif ($yolovers -eq "yolov4-tiny"){
	$startweight = "yolov4-tiny.conv.29"
}

# check if we are on windows or Linux. If Linux we assume a headless system!
if ($Env:OS -match "Windows"){
	$darknetExe = ".\darknet.exe"
	if(-Not $isWindows){ $isWindows = 1 } # older versions of powershell do not have this variable!
} elseif ($isLinux) {
	$darknetExe = "darknet"
	# isWindows is already set!
} else { # assume it is Linux, but an old version of powershell!
	$darknetExe = "darknet"
	$isWindows = 0
}

if ($yolovers -match "yolov3-spp"){
	$dataFile = "./data/NAOS/$version/NAOS-$v-$aug.data" # default
	$dFbackup = "backup/NAOS/$version/$aug/" #"backup  = backup/NAOS/v0.1/AHEtrain/"
} else {
	$dataFile = "./data/NAOS/$version/$($yolovers)-NAOS-$v-$aug.data" # new data file for a different yolo version
	$dFbackup = "backup/NAOS/$version/$yolovers/$aug/" # e.g. "backup  = backup/NAOS/v0.1/yolov3-tiny/AHEtrain/"
}

$dFtrain = "data/NAOS/$version/NAOS-$v-$($aug)_train.txt"#"train   = data/NAOS/v0.1/NAOS-v0.1-AHEtrain_train.txt"
$dFvalid = "data/NAOS/$version/NAOS-$v-$($aug)_valid.txt "#"valid   = data/NAOS/v0.1/NAOS-v0.1-AHEtrain_valid.txt"
$cfgfile = "./cfg/$($yolovers)-NAOS.cfg"

# create the .data file if it does not exist!
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

# create backup directory if it does not exist yet!
if (-Not (Test-Path $dFbackup -PathType Container)){
	mkdir "backup/NAOS/$version/$yolovers/" # create root first
	mkdir $dFbackup # then subdirectory (noAHE or AHEtrain)
}


$gpuStr = ''
$gpus | ForEach-Object { $gpuStr = "$gpuStr$_," }
$gpuStr = $gpuStr.Substring(0,$gpuStr.Length-1) # remove last ','

if( -Not $isWindows ){ # Linux no-display system 
	$darknetargs = " detector train {0} {1} {2} -map -dont_show -mjpeg_port 809{3} -gpus {4}" -f $datafile, $cfgfile, $startweight, $gpus[0], $gpuStr
} else { # windows
	$darknetargs = " detector train {0} {1} {2} -map -gpus {3}" -f $datafile, $cfgfile, $startweight, $gpuStr
}

#Write-Host $darknetargs 

Write-Host $darknetExe "$darknetargs"

if( -Not $onlyPrint){
	# run the command:
	iex "$darknetExe $darknetargs "
}
