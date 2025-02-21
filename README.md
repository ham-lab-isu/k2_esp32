# K2_ESP32

A ROS2 driver for communicating with ESP32 Dev boards. This package creates a ROS2 action server that communicates via ROS2 via Serial (USB-C).

The board of choice is an [ESP32-WROOM](https://www.amazon.com/AITRIP-ESP-WROOM-32-Development-Microcontroller-Integrated/dp/B0CR5Y2JVD?crid=3UCQCOIIDNSOZ&dib=eyJ2IjoiMSJ9.J8fl2PuZsBFTQRqqz9O9mL1Rpkhr_JZADgLi8zJ63PahbVJM_h8Jnr7apbTwY3g5fb_EH8pI-g6dXV5yTwNxnPzXEmCCKJkbHfoEy_QaIqR78cNqH69H0-KMZZEhFuJcPas8F5vD7wjJYq81OdESyY8H1WZL1V9pQ9xNWH0ezw3yyPjIgB2AFZdmNKNgOV_9kNuhNux1Xla1QEur9pz10AMH3TbMF10rLrtqHh24hns.RIaLbIh0lwKpRvStbqEWkckrmW2aszIeSKMqJYbZbTM&dib_tag=se&keywords=esp32%2Bc&qid=1739219767&sprefix=esp32%2Bc%2Caps%2C165&sr=8-3&th=1).

## Flashing the ESP32
### Arduino IDE
If you're working with the Arduino IDE, select the "ESP32 Dev Module" from "esp32".

### Platform.io
If you're working with Platform.io to flash software, use "espressif32" as the platform and "esp32dev" as the board name.
