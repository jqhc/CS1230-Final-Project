# Itsy Bitsy Inverse Kinematic Spider

## About
Thanks for checking out our project! Itsy Bitsy is a dynamic, user-controlled spider built using C++ and its OpenGL API. Itsy's legs are managed using an inverse kinematics algorithm.

## Group Members
- Justin Cheng (jcheng59)
- Austin Funk (afunk3)

## Repository:
https://github.com/jqhc/CS1230-Final-Project

## How to run:
1. Open `CMakeLists.txt` in Qt Creator.
2. Press the "Build" button at the bottom left to build the project.
3. Press "Run" at the bottom left to run the project!

## Controls:
The camera can be controlled with WASD (for forward and side-to-side movement) and the Ctrl/Cmd and Space keys (for world up and down movement).

Itsy Bitsy can be controlled using the arrow keys (up and down to move forwards and backwards, and left and right to rotate).

## Known Bugs
Itsy will not walk off the edge! He will simply keep walking as though the ground was there. As such, try not to leave the area.

## Acknowledgements
The following resources were used as references when implementing the inverse kinematics algorithm:
- https://iopscience.iop.org/article/10.1088/1742-6596/1314/1/012109/pdf
- https://www.alanzucconi.com/2020/09/14/inverse-kinematics-in-3d/