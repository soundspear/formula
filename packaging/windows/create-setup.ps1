param (
	[Parameter(Mandatory=$true)][string]$AppName,
	[Parameter(Mandatory=$true)][string]$AppFolderName,
	[Parameter(Mandatory=$true)][string]$OutputFileName,
	[string]$AppVer = "1.0",
	[string]$WebsiteUrl = "https://soundspear.com/",
	
	[Parameter(Mandatory=$false)][string]$Vst2File64,
	[Parameter(Mandatory=$false)][string]$Vst3File64,
	
	[string]$InputScriptPath = "setup-script-windows.iss",
	[string]$OutputScriptPath = "setup-script-windows-out.iss"
)

function Prepend-Param {
	param (
        [Parameter(Mandatory=$true)][string]$Name,
        [Parameter(Mandatory=$true)][string]$Value
    )
	$paramStr = "#define $Name `"$Value`"`n"
	$paramStr + (Get-Content $OutputScriptPath -Raw) | Set-Content $OutputScriptPath
}

Copy-Item -Force $InputScriptPath -Destination $OutputScriptPath

Prepend-Param -Name AppName -Value $AppName
Prepend-Param -Name AppFolderName -Value $AppFolderName
Prepend-Param -Name OutputFileName -Value $OutputFileName
Prepend-Param -Name AppVer -Value $AppVer
Prepend-Param -Name WebsiteUrl -Value $WebsiteUrl
If ($PSBoundParameters.ContainsKey('Vst2File64')) {
	Prepend-Param -Name Vst2File64 -Value $Vst2File64
}
Else {
	Prepend-Param -Name NoVst2 -Value 1
}
Prepend-Param -Name Vst3File64 -Value $Vst3File64

$InnoSetupPath = "$Env:ProgramFiles (x86)\Inno Setup 6\ISCC.exe"
$InnoSetupPath
& "$InnoSetupPath" `"$OutputScriptPath`"
