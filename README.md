# esp32-wifi-PC-button
remotely power on/off PC

ESP32-->replay module---> SW on motherboard
visit http://esp32.local and control the PC powerbutton

ESP32 side connection:
pin D14 ---> LED (debugging purpose);
pin D12 ---> relaymodule Dx port ;
VIN --->both relaymodule and LED ;
GND --->both relaymodule and LED ;

replay module
NO --> motherboard SW ;
COM -> motherboard SW ;


From VSCODE 
1.compile : idf.py build
2.upload ESP32: idf.py flash
3.monitor ESP32 running (also IP address allocation): idf.py -p COM3 monitor

<img width="452" alt="image" src="https://github.com/user-attachments/assets/074dfb7a-671b-4cbf-a6e2-c2d9b7d58631" />


![image](https://github.com/user-attachments/assets/7d749c13-e88a-420d-8f83-865565a2016e)
