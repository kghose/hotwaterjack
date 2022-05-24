# Hot Water Jack*

Use ESP32 board to record temperatures and events associated with
hot water boiler operation.

1. Record temperatures and events and keep in memory
2. Deliver data via WiFi link when client conencts
3. Overwrite data when buffer full or when client has downloaded data

*With apologies to Carole King


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
