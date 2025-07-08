# Ideas
## station board
* put dmp4025 into a separate module, including the 5v level command, protections, current sensor and handle connector
* put power input close to handle connector
* one board per output, as the arduino clones are cheap so there is no real economy in controlling two handles per arduino. two arduinos share a power mosfet module
* increase FET S-D resistor to 4k7 or 5k6

* ## handle board
* put a reverse protection diode parallel to the heater
