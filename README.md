# ESP32 Settings WebServer

This is a ESP32 Firmware developed in [PlatformIO](https://platformio.org/) which consists in a HTTP server for serve static files and provides an API for wifi settings. 

The static page is place under the `./data` folder and is a [Preact](https://preactjs.com/) page. This framework is used because of it's minimal size. The page is uploaded by generating a SPIFFS binary with PIO.

The settings routes are: 
 - `/wifis` - for getting a list of near networks. (Not implemented on the webpage yet)
 - `/local` - to set the wifi network which the ESP will connect
 - `/hub` - to set the wifi network which the ESP will generate
 - `/save` - to restart and apply the settings

To access the webpage you can access the following URL ``http://{esp's ip}/index.html`.

The webpage code is in this [repo](https://github.com/mateusm09/settings-page)

