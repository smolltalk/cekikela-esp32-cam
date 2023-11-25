# esp32-cam-sd-web-pir

## Getting started

- Copy config-example.cpp to config.cpp
- Rename setupAppConfigExample to setupAppConfig

## Settings

|Name|Category|Description|Type|Range|config.cpp Example|config.txt Example|
|----|--------|-----------|----|-----|----------|----------|
|appConfig_t.savePictureOnSdCard||When enabled, picture will be saved on the SD card|bool|true, false|`appConfig->savePictureOnSdCard = true;`|savePictureOnSdCard=true|
|appConfig_t.awakeDuration||It defines a time delay in ms before sleep mode.<br/>This prevents picture bursts when the board is awakened by an untimely signal|uint16_t|[0, 65535]|`appConfig->awakeDuration=5000;`|awakeDurationMs=5000|
|appConfig_t.awakePeriodSec||It defines the sleep duration in seconds before the board will be waken up.<br/>A 0 value disables the feature.|uint16_t|[0, 65535]|`appConfig->awakePeriodSec=300;`|awakePeriodSec=300|
|wifiSettings_t.enabled|WiFi|It enables WiFi connections.<br/>WiFi is required to update time by NTP and to upload pictures.|bool|true, false|`appConfig->wifi.enabled = true;`|wifi.enabled=true|
|wifiSettings_t.ssid|WiFi|WiFi SSID, i.e. the name of your WiFi network|char *|N/A|`appConfig->wifi.ssid = "MyWiFiSSID";`|wifi.ssid=MyWiFiSSID|
|wifiSettings_t.password|WiFi|WiFi network password|char *|N/A|`appConfig->wifi.password = "MyWiFiPassword";`|wifi.ssid=MyWiFiPassword|
|wifiSettings_t.connectAttemptMax|WiFi|The max count of attempts to be connected|uint8_t|[0, 255]|`appConfig->wifi.connectAttemptMax = 30;`|wifi.connectAttemptMax=30|
|uploadSettings_t.enabled|Upload|When set to true, pictures will be uploaded to a remote server according to the following parameters.|bool|true, false|`appConfig->upload.enabled=true;`|upload.enabled=true|
|uploadSettings_t.serverName|Upload|Address of the server receiving pictures.|char *|N/A|`appConfig->upload.serverName="myserver.picture.com";`|upload.serverName=myserver.picture.com|
|uploadSettings_t.serverPort|Upload|Listening TCP port of the server receiving pictures.|uint16_t|[1, 65535]|`appConfig->upload.serverPort=8080;`|upload.serverPort=8080|
|uploadSettings_t.uploadPath|Upload|Upload path of the service receiving data.|char *|N/A|`appConfig->upload.uploadPath=/upload.php;`|upload.serverName=/upload.php|
|uploadSettings_t.auth|Upload|Address of the server receiving pictures.|char *|N/A|`appConfig->upload.auth="MyUploadPassword";`|upload.auth=MyUploadPassword|
|uploadSettings_t.bunchSize|Upload|Upload in packs of `bunchSize` when pictures are stored on SD card.|uint8_t|[0, 255]|`appConfig->upload.bunchSize=10;`|upload.bunchSize=10|
|uploadSettings_t.fileNameRandSize|Upload|When the picture is not stored on the SD card,<br/>a random file name is computed.<br/>Its format is `pic-random.jpg` where `random` is randomly composed of numbers and letters.<br/>`fileNameRandSize` defines the length of the random part.|uint8_t|[1, 8]|`appConfig->upload.fileNameRandSize=5;`|upload.fileNameRandSize=5|
|cameraSettings_t.getReadyDelayMs|Camera|Time required to let the sensor be ready. A delay of 1500ms prevents 'green' pictures.|uint16_t|[0, 65535]|`appConfig->camera.getReadyDelayMs=1500`|camera.getReadyDelayMs=1500|
|sensorSettings_t.contrast|Camera Sensor|Set contrast.|int|[-2, 2]|`setSensorSetting(&(appConfig->camera.sensor.contrast), 0)`|sensor.contrast=|
|sensorSettings_t.brightness|Camera Sensor|Set brightness.|int|[-2, 2]|`setSensorSetting(&(appConfig->camera.sensor.brightness), 0)`|sensor.brightness=|
|sensorSettings_t.saturation|Camera Sensor|Set saturation.|int|[-2, 2]|`setSensorSetting(&(appConfig->camera.sensor.saturation), 0)`|sensor.saturation=|
|sensorSettings_t.special_effect|Camera Sensor|Set a special effect: 0 - No Effect, 1 - Negative, 2 - Grayscale, 3 - Red Tint, 4 - Green Tint, 5 - Blue Tint, 6 - Sepia.|int|[0, 6]|`setSensorSetting(&(appConfig->camera.sensor.special_effect), 2)`|sensor.special_effect=2|
|sensorSettings_t.gainceiling|Camera Sensor|0 = 2X, 1 = 4X, 2 = 8X, 3 = 16X, 4 = 32X, 5 = 64X, 6 = 128X.|int|[0-6]|`setSensorSetting(&(appConfig->camera.sensor.gainceiling), 0)`|sensor.gainceiling=0|
|sensorSettings_t.framesize|Camera Sensor|0 = 96X96, 1 = QQVGA (160x120), 2 = QCIF (176x144), 3 = HQVGA (240x176), 4 = 240X240 (240x240), 5 = QVGA (320x240), 6 = CIF (400x296), 7 = HVGA (480x320), 8 = VGA (640x480), 9 = SVGA (800x600), 10 = XGA (1024x768), 11 = HD (1280x720), 12 = SXGA (1280x1024), 13 = UXGA (1600x1200), [3MP Sensors] 14 = FHD (1920x1080), 15 = P_HD (720x1280), 16 = P_3MP (864x1536), 17 = QXGA (2048x1536), [5MP Sensors] 18 = QHD (2560x1440), 19 = WQXGA (2560x1600), 20 = P_FHD (1080x1920), 21 = QSXGA (2560x1920)|int|[0-21]|`setSensorSetting(&(appConfig->camera.sensor.framesize), 11)`|sensor.framesize=11|
|sensorSettings_t.pixformat|Camera Sensor|0 = 2BPP/RGB565, 1 = 2BPP/YUV422, 2 = 1.5BPP/YUV420, 3 = 1BPP/GRAYSCALE, 4 = JPEG/COMPRESSED, 5 = 3BPP/RGB888, 6 = RAW, 7 = 3BP2P/RGB444, 8 = 3BP2P/RGB555|int|[0-8]|`setSensorSetting(&(appConfig->camera.sensor.pixformat), 4)`|sensor.pixformat=4|
|sensorSettings_t.whitebal|Camera Sensor|Set white balance: 0 = disable, 1 = enable.|int|[0, 1]|`setSensorSetting(&(appConfig->camera.sensor.whitebal), 0)`|sensor..whitebal=0|
|sensorSettings_t.awb_gain|Camera Sensor|Set white balance gain: 0 = disable, 1 = enable.|int|[0, 1]|`setSensorSetting(&(appConfig->camera.sensor.awb_gain), 0)`|sensor.awb_gain=0|
|sensorSettings_t.wb_mode|Camera Sensor|Set white balance mode: 0 to 4 - if awb_gain enabled (0 - Auto, 1 - Sunny, 2 - Cloudy, 3 - Office, 4 - Home).|int|[0, 4]|`setSensorSetting(&(appConfig->camera.sensor.wb_mode), 0)`|sensor.wb_mode=|
|sensorSettings_t.exposure_ctrl|Camera Sensor|Set exposure control: 0 = disable, 1 = enable.|int|[0, 1]|`setSensorSetting(&(appConfig->camera.sensor.exposure_ctrl), 0)`|sensor.exposure_ctrl=0|
|sensorSettings_t.aec2|Camera Sensor||int|[0, 1]|`setSensorSetting(&(appConfig->camera.sensor.aec2), 0)`|sensor.aec2=0|
|sensorSettings_t.ae_level|Camera Sensor||int|[-2, 2]|`setSensorSetting(&(appConfig->camera.sensor.ae_level), 0)`|sensor.ae_level=0|
|sensorSettings_t.aec_value|Camera Sensor||int|[0, 1200]|`setSensorSetting(&(appConfig->camera.sensor.aec_value), 0)`|sensor.aec), 0|
|sensorSettings_t.gain_ctrl|Camera Sensor|0 = disable, 1 = enable.|int|[0, 1]|`setSensorSetting(&(appConfig->camera.sensor.gain_ctrl), 0)`|sensor.gain_ctrl=0|
|sensorSettings_t.agc_gain|Camera Sensor||int|[0, 30]|`setSensorSetting(&(appConfig->camera.sensor.agc_gain), 0)`|sensor.agc_gain=0|
|sensorSettings_t.bpc|Camera Sensor|0 = disable, 1 = enable.|int|[0, 1]|`setSensorSetting(&(appConfig->camera.sensor.bpc), 0)`|sensor.bpc=0|
|sensorSettings_t.wpc|Camera Sensor|0 = disable, 1 = enable.|int|[0, 1]|`setSensorSetting(&(appConfig->camera.sensor.wpc), 0)`|sensor.wpc=0|
|sensorSettings_t.raw_gma|Camera Sensor|0 = disable, 1 = enable.|int|[0, 1]|`setSensorSetting(&(appConfig->camera.sensor.raw_gma), 0)`|sensor.raw_gma=0|
|sensorSettings_t.lenc|Camera Sensor|Set lens correction: 0 = disable, 1 = enable.|int|[0, 1]|`setSensorSetting(&(appConfig->camera.sensor.lenc), 0)`|sensor.lenc=0|
|sensorSettings_t.hmirror|Camera Sensor|Horizontal mirrot: 0 = disable, 1 = enable.|int|[0, 1]|`setSensorSetting(&(appConfig->camera.sensor.hmirror), 0)`|sensor.hmirror=0|
|sensorSettings_t.vflip|Camera Sensor|Vertical flip: 0 = disable, 1 = enable.|int|[0, 1]|`setSensorSetting(&(appConfig->camera.sensor.vflip), 0)`|sensor.vflip=0|
|sensorSettings_t.colorbar|Camera Sensor|0 = disable, 1 = enable.|int|[0, 1]|`setSensorSetting(&(appConfig->camera.sensor.colorbar), 0)`|sensor.colorbar=|
|sensorSettings_t.dcw|Camera Sensor||int||`setSensorSetting(&(appConfig->camera.sensor.dcw), 0)`|sensor.dcw=|
|sensorSettings_t.sharpness|Camera Sensor||int||`setSensorSetting(&(appConfig->camera.sensor.sharpness), 0)`|sensor.sharpness=|
|sensorSettings_t.denoise|Camera Sensor||int||`setSensorSetting(&(appConfig->camera.sensor.denoise), 0)`|sensor.denoise=0|
|sensorSettings_t.quality|Camera Sensor||int||`setSensorSetting(&(appConfig->camera.sensor.quality), 0)`|sensor.quality=0|

Create the file `config.cpp` from the model `config-example.cpp`.   
In the function `setupAppConfig`, you can set the following parameters:  

Further details are available in `sensor.h` and [here](https://randomnerdtutorials.com/esp32-cam-ov2640-camera-settings/)
## Examples  

### Example #1

`config.cpp`

```C
#include "cfgmgt.h"

void setupAppConfig(appConfig_t * appConfig) {
    // **** General ****
    
    // Save picture on SD card
    appConfig->savePictureOnSdCard = true;
    // Still awaken 5000ms before going in deep sleep mode
    appConfig->awakeDurationMs = 5000;
    // Do not periodically wake up the board
    appConfig->awakePeriodSec = 0;

    // **** WiFi ****
    
    // Enable WiFi connection
    appConfig->wifi.enabled = true;
    // WiFi SSID
    appConfig->wifi.ssid = "MyWifiSSID";
    // WiFi password
    appConfig->wifi.password = "MyWifiPassword";
    // 30 connection attempts max
    appConfig->wifi.connectAttemptMax = 30;
    
    // **** Upload ****
    
    // Enable pictures upload
    appConfig->upload.enabled = true;
    // Address of the server receiving pictures
    appConfig->upload.serverName = "myserver.picture.com";
    // Listening TCP port of the server receiving pictures
    appConfig->upload.serverPort = 80;
    // Upload path of the service receiving data
    appConfig->upload.uploadPath = "/upload.php";
    // Authentication to keep the upload private
    appConfig->upload.auth = "MyUploadPassword";
    // Upload in packs of 10 when pcitures are stored on SD card
    appConfig->upload.bunchSize = 10;
    // 
    appConfig->upload.fileNameRandSize = 5;
    
    // **** Camera ****
    
    appConfig->camera.getReadyDelayMs = 1500;
    
    // **** Camera sensor ****
    
    setSensorSetting(&(appConfig->camera.sensor.gain_ctrl), 1);      // Auto gain on
    setSensorSetting(&(appConfig->camera.sensor.exposure_ctrl), 1);  // Auto exposure on
    setSensorSetting(&(appConfig->camera.sensor.awb_gain), 1);       // Auto White Balance enable (0 or 1)
    setSensorSetting(&(appConfig->camera.sensor.brightness), 1);
    setSensorSetting(&(appConfig->camera.sensor.gainceiling), 1);
}
```

### Example #2

 `config.txt` on SD card:

```
savePictureOnSdCard=true
awakeDurationMs=5000
awakePeriodSec=0
wifi.enabled=true
wifi.ssid=MyWifiSSID
wifi.password=MyWifiPassword
wifi.connectAttemptMax=30
upload.enabled=true
upload.serverName=myserver.picture.com
upload.serverPort=80
upload.uploadPath=/upload.php
upload.auth=MyUploadPassword
upload.bunchSize=2
upload.fileNameRandSize=5
camera.getReadyDelayMs=1500
sensor.gain_ctrl=1
sensor.exposure_ctrl=1
sensor.awb_gain=1
sensor.brightness=1
sensor.gainceiling=1
```

## TODO
- Complete Readme.md
- OTA
- use begin instead of initXYZ
- Rename file in C or use extern C { } as in sensor.h
- get default sensor values
- test with framesize and pixformat
- file header

