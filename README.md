# esp32-wifi-PC-button
remotely power on/off PC

ESP32-->replay module---> SW on motherboard
visit http://esp32.local and control the PC powerbutton

ESP32 pin D14 ---> LED (debugging purpose)
      pin D12 ---> relaymodule
      VIN --->
      GND --->both relaymodule and LED

From VSCODE 
1.compile : idf.py build
2.upload ESP32: idf.py flash
3.monitor ESP32 running (also IP address allocation): idf.py -p COM3 monitor
