# ESP8266-alone
esp8266 single board with uart and tcp
**esp8266** as tcp client, **router** as tcp server, esp send command to slave board via uart and get SHT30 Temp&Humi via IIC.
**Router** will receive esp's Temp&Humi data then store in mysql database, and have a PHP server inside will take charge of the web display and data query.
