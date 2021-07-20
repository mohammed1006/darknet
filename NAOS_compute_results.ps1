param (
    [string]$version = $(throw "-version is required."), # dataset version 
    [string]$test = 'AHEtrain', # use AHEtrain the Matlab evaluate script takes care of the rest!
	[string]$yolovers = 'yolov3-spp',
	[switch]$useValidation = $false # True 1 or False 0
)

# SETTINGS
#$version = 'v0.3' #or simply use $version
#$test = 'AHEtrain' # use AHEtrain the Matlab evaluate script takes care of the rest!

if (-Not (Test-Path "./weights/NAOS/$version/" -PathType Container)){
	throw "$version is not a valid option for -version, as weights do not exist in the weights folder."
}

# if version contains folders e.g. vL.1s/AP26to35 then the / or \ should be replaced by _ <underscore>
$v = $version -replace "\\","_"
$v = $v -replace "/","_"


# check if we are on windows or Linux. If Linux we assume a headless system!
if ($Env:OS -match "Windows"){
	$darknetExe = "darknet.exe"
	$isWindows = 1
} elseif ($isLinux) {
	$darknetExe = "./darknet"
	# isWindows is already set!
} else { # assume it is Linux, but an old version of powershell!
	$darknetExe = "./darknet"
	$isWindows = 0
}


foreach ($aug in 'AHEtrain', 'noAHE') { # yolo version trained with ... 
	#Write-Host $aug
	#continue
	
	if ($useValidation){
		$testdata = "./data/NAOS/$version/NAOS-$v-$($test)_valid.txt"
		$resultfile = "./results/NAOS/$version/$yolovers/$aug/NAOS-$v-$($test)_valid.json"
	} else {
		$testdata = "./data/NAOS/$version/NAOS-$v-$($test)_test.txt"
		$resultfile = "./results/NAOS/$version/$yolovers/$aug/NAOS-$v-$($test)_test.json"
	}

	$datafile = "./data/NAOS/$version/$($yolovers)-NAOS-$v-$aug.data" # new data file for a different yolo version
	if (-Not (Test-Path $datafile -PathType leaf)){
		$datafile = "./data/NAOS/$version/NAOS-$v-$aug.data" # use old format
	}
	if (-Not (Test-Path $datafile -PathType leaf)){
		throw "datafile $datafile does not exist!"
	}
	$cfgfile = "./cfg/$($yolovers)-NAOS.cfg"
	$thresh = 0.0001 # note that the json export does not go below .5% or 0.005

	if ( Test-Path -Path "./results/NAOS/$version/$yolovers/$aug/" ){
	} else {
		New-Item -ItemType Directory -Path "./results/NAOS/$version/$yolovers/$aug/"
	}
	

	#Write-Host $testdata 

	# get weight in folder 
	$weight = Get-Item "./weights/NAOS/$version/$yolovers/$aug/*.weights" 
	# make sure there is only one weight
	if( $weight.Count -ne 1){
		throw "Multiple weights available"
	}

	$darknetargs = " detector test {0} {1} {2} -thresh {3} -dont_show -ext_output -out {4}" -f $datafile, $cfgfile, $weight.FullName, $thresh, $resultfile
	#$darknetargs = "detector  test  '{0}' '{1}' '{2}'" -f $datafile, $cfgfile, $weight.Name

	#Write-Host $darknetargs 

	Write-Host "cat $testdata |" $darknetExe "$darknetargs"

	# run the command:
	iex "cat $testdata | $darknetExe $darknetargs "

}