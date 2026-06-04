$msbuild = ""
if (Get-Command "vswhere.exe" -ErrorAction SilentlyContinue) {
    $msbuild = & vswhere.exe -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe
}

if (-not $msbuild -or -not (Test-Path $msbuild)) {
    $paths = @(
        "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe",
        "C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe",
        "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe"
    )
    foreach ($p in $paths) {
        if (Test-Path $p) {
            $msbuild = $p
            break
        }
    }
}

if ($msbuild -and (Test-Path $msbuild)) {
    Write-Host "Found MSBuild at: $msbuild"
    & $msbuild BlackOpsInternal.sln /p:Configuration=Release /p:Platform=x86
} else {
    Write-Error "MSBuild.exe was not found. Please make sure Visual Studio is installed with C++ development tools."
}
