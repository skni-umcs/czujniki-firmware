# README

My future bachelor's degree that is C++ firmware for a network of climate sensors.

UML diagrams of the firmware:

![UML 1](https://raw.githubusercontent.com/skni-umcs/firmware/main/uml1.png)
![UML 2](https://raw.githubusercontent.com/skni-umcs/firmware/main/uml2.png)

The design patterns used in the design are: 
- Facade (to reduce the code needed to manage all the sensors at once)
- Bridge (to separate the logic of sending data from collecting it)
- Observer (to receive)
- Singleton (to notify timers of time updates on the device)

Technical information:
- The firmware is designed for esp32 s2 mini
- Due to the reset functionality of esp32 s2 mini it is advised to run unit tests on other esp32 board (for me it is esp32 firebeetle)