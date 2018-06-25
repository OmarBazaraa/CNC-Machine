# Antar CNC Machine
A CNC machine powered by Arduino with the following features:

* Solving Flow game
* Drawing images
* Manual calibration
* Playing piano
* Sense 9 different issues that can damage the machine or lead to wrong results
* Fully expandable
* Contain power backup circuit
* Precise accuracy

![alt text](https://raw.githubusercontent.com/OmarBazaraa/CNC-Machine/master/picture.png)

### Trailer Video:
[![IMAGE ALT TEXT](http://img.youtube.com/vi/A8BskRCSG7A/0.jpg)](http://www.youtube.com/watch?v=A8BskRCSG7A "CNC Machine")

# Project Structure

## Controller
A console application written in Java that establish asynchronous communication with the Arduino to control the CNC-Machine from the computer.

It allows the user to select one of the following tasks:
* Solve flow game.
* Paint photo on paper.
* Play piano from the keyboard.
* Calibrate the CNC manually.

The controller is designed in a fully expandable way.
All the logic to communicate and configure the CNC is encapsulated in `CNCTask` class.
So we can simply add new tasks by just extending `CNCTask` class and implementing few methods
without the need to deal with the underlying logic for serial communications with the Arduino.

## Arduino
A C-program burnt on the Arduino that:
* Controls the motors and the sensors on the CNC machine and execute the instructions received from the controller program.
* Has a simple logic for protecting the CNC against some faults that might occur.

## Flow Solver
A C++ program that takes a screenshot for Flow game level, analyses it and perform one of the following tasks:
* Run an algorithm for solving the game level.
* Detect the "Next Level" button.

Finally, it returns an instruction-string that the CNC can perform.

## Painter
A C++ program that takes an image and convert it into an instruction-string that the CNC can perform.
