$configs = Get-ChildItem "boards/configs" -Filter *.h
$userSetup = ".pio/libdeps/4d_systems_esp32s3_gen4_r8n16/TFT_eSPI/User_Setup.h"
$buildDir = ".pio/build/4d_systems_esp32s3_gen4_r8n16"
$outputRoot = "E:\Project\chatbot_esp32\build"

foreach ($cfg in $configs) {
    $name = $cfg.BaseName
    Write-Host "`n---Build config: $name---`n"

    # Thay User_Setup.h
    Copy-Item $cfg.FullName -Destination $userSetup -Force

    # Build project
    pio run

    # Tạo thư mục output
    $outDir = "$outputRoot/$name"
    New-Item -ItemType Directory -Force -Path $outDir | Out-Null

    # Copy 3 file quan trọng
    foreach ($file in "firmware.bin","partitions.bin","bootloader.bin") {
        Copy-Item "$buildDir/$file" $outDir -Force
    }

    Write-Host "`n---Build successfully: $name---`n"
}

Set-Location -Path $outputRoot
git add .
git commit -m  "update fw"
git push origin main
cd ..
cd esp32_client
git add .
git commit -m  "update code"
git push origin main