SnakeOL
=======

The game with client part and server part
-----------------------------------------

BEFORE install the game, make sure OpenGL libary is pre-installed.<br />
If not, following link may be helpful.<br />
1. [http://www.opengl.org/wiki/Getting_Started](http://www.opengl.org/wiki/Getting_Started)<br />
2. [http://www.wikihow.com/Install-Mesa-(OpenGL)-on-Linux-Mint](http://www.wikihow.com/Install-Mesa-(OpenGL)-on-Linux-Mint)<br />
And Google protocol buffer:<br />
[https://developers.google.com/protocol-buffers/docs/cpptutorial](https://developers.google.com/protocol-buffers/docs/cpptutorial)<br />
To compile the file, use g++ 4.8 or later version.<br />

### Install:<br />
    make all
### Delete:<br />
    make clean
### Run:<br />
    1. First run snake_calculate
        ./snake_calculate \<PORT_NUMBER\>
    \<PORT_NUMBER\>: should be in the range of [10000,49999]
    2. Then run snake_display<br />
        ./snake_display \<IP_Address\> <PORT_NUMBER>
    \<IP_address\>: 127.0.0.1 OR ::1
    \<PORT_NUMBER\>: must be same as the PORT_NUMBER above.
### Control:<br />
    keyboard: left, right, up, down to change the direction of the snake.
    
Any advice would be appreciated! ^_^<br />
