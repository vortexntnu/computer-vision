The code consists of three files; vision.h, main.cpp and vision.cpp.

Vision.h is the header file for the other two files.
main.cpp contains the while loop which call camera.activate() and camera.analyse() from vision.cpp.
The last part of the while loop contains an key listener which breaks the loop if the escape key is pressed.

vision.cpp contains the all the parameters, the activate and analyse function.
