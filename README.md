# Hotwater Jack*

Use ESP32 board to record temperatures and events associated with
hot water boiler operation.

1. Record temperatures and events and keep in memory
2. Deliver data via WiFi link when client conencts
3. Overwrite data when buffer full or when client has downloaded data

*With apologies to Carole King

# Interface

Communication is via TCP. 

## Design 1: MVP

Store data in a rolling buffer. When asked, return the most recent N samples. 
Special function to print out latest data point in human readable format.

| Command  | Returns                                        |
|---       |---                                             |
| now      | Most recent data row, human readable |
| 1, 2..   | Most recent N data rows, binary |

Pro: Very simple storage and retrieval design
Cons: 
1. Makes it hard to stitch together multiple datasets because there are no timestamps
2. All data is in RAM, so a power outage will delete all data
3. Only 1 weeks worth of data is stored

Next design: "Page" the data by day and have a timestamp for the day.

## Flowchart

1. Initialize WiFi connection
2. Initialize TCP server
3. Log temperature and events every 1 minute
4. When client connects, deliver data
5. If client instructs, reset buffer
6. If buffer is full, begin to overwrite it


## WiFi and other config options

Set using `idf.py menuconfig`. See [main/Kconfig.projbuild] for the list of options.


## Buffer design
