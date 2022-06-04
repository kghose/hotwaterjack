# Hotwater Jack*

Use ESP32 board to record temperatures and events associated with
hot water boiler operation.

1. Record temperatures and keep in rolling buffer
2. Deliver data via WiFi link when client conencts

*With apologies to Carole King

# Interface

Communication is via TCP. 

## Design 1: MVP

Store data in a rolling buffer. When asked, return the most recent N samples. 
Special function to print out latest data point in human readable format.

| Command  | Returns                                        |
|---       |---                                             |
| info     | Print stats and info about the logger and data |
| now      | Most recent data row, human readable |
| 1, 2..   | Most recent N data rows, binary |

Pros: 
1. Very simple storage and retrieval design

Cons: 
1. Makes it hard to stitch together multiple datasets because there are no timestamps
2. All data is in RAM, so a power outage will delete all data
3. Only 1 weeks worth of data is stored

Next design: "Page" the data by day and have a timestamp for the day.


## WiFi and other config options

Set using `idf.py menuconfig`. See [main/Kconfig.projbuild] for the list of options.

# Other software
1. [DS18b20 ESP32 library from FeelFreeLinux](https://github.com/feelfreelinux/ds18b20)
