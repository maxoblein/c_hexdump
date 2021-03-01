Max Oblein
Silvia Yu
22 March 2018

*Description:
-Hexdump: Print a textual representation of the bytes from the file
-Version: Print the version (1.0) to stdout
-P2 to P5: Read a PNM file, the input is expected to be a P2 format image (ascii) and the output should be saved as P5 (binary). If the input is not a P2 format image, print the error message to stderr and exit with code 1
-P5 to P2: Read a PNM file, the input is expected to be a P5 format image (binary) and the output should be saved as P2 (ascii). If the input is not a P5 format image, print the error message to stderr and exit with code 1
-Rotate2: Read the rotate a p2 image, changes the image so that it has height W and width H, and the data that was at (row i column j) in the original image is now at (row j column i)
-Rotate5: Read and rotate a p5 image, changes the image so that it has height W and width H, and the data that was at (row i column j) in the original image is now at (row j column i)
-Rotate902: Read the rotate a P2 image, rotated CLOCKWISE by 90 degrees. Changing the image so that it has height W and width H, and the data that was at (row i column j) in the original image is now at (row j column i), then reflect the image about the central column
-Rotate905: Read the rotate a P5 image, rotated CLOCKWISE by 90 degrees. Changing the image so that it has height W and width H, and the data that was at (row i column j) in the original image is now at (row j column i), then reflect the image about the central column
-invert: this in our extension funtion it will invert the colour in the image file by taking each matrix entry and subtracting it from 255 the usual max value.





*To compile the code:
A Makefile is made in order to compile the code；

all: pnmdump.exe

pnmdump.exe: pnmdumpmain.c
  gcc -std=c99 -Wall $< -o $@

test: pnmdump.exe
  python tests/runtests-1.0.py

The exact commands: gcc pnmdumpmain.c -o pnmdumo.exe

IF IN THE DIRECTORY RUN :make test to compile and run the test file.


*Examples:
1. 
$ ./pnmdump.exe --usage
Usage: 
./pnmdump.exe --version 
./pnmdump.exe --usage 
./pnmdump.exe --hexdump [FILE]

2.
$ ./pnmdump.exe
pnmdump: bad arguments
Usage:
./pnmdump.exe --version
./pnmdump.exe --usage
./pnmdump.exe --hexdump [FILE]

3.
$ ./pnmdump.exe --hexdump Makefile
0000000  61 a  6C l  6C l  3A :  20    70 p  6E n  6D m
0000008  64 d  75 u  6D m  70 p  2E .  65 e  78 x  65 e
0000010  0A .  0A .  70 p  6E n  6D m  64 d  75 u  6D m
0000018  70 p  2E .  65 e  78 x  65 e  3A :  20    70 p
0000020  6E n  6D m  64 d  75 u  6D m  70 p  6D m  61 a
0000028  69 i  6E n  2E .  63 c  0A .  20    20    67 g
0000030  63 c  63 c  20    2D -  73 s  74 t  64 d  3D =
0000038  63 c  39 9  39 9  20    2D -  57 W  61 a  6C l
0000040  6C l  20    24 $  3C <  20    2D -  6F o  20  
0000048  24 $  40 @  0A .  0A .  74 t  65 e  73 s  74 t
0000050  3A :  20    70 p  6E n  6D m  64 d  75 u  6D m
0000058  70 p  2E .  65 e  78 x  65 e  0A .  20    20  
0000060  70 p  79 y  74 t  68 h  6F o  6E n  20    74 t
0000068  65 e  73 s  74 t  73 s  2F /  72 r  75 u  6E n
0000070  74 t  65 e  73 s  74 t  73 s  2D -  31 1  2E .
0000078  30 0  2E .  70 p  79 y  0A .
000007d


*Changelog
22/03/2018: Implemented the invert function for p2 and p5 and put assignment part 1 into functions.
20/03/2018: Fixed rotate and rotate 90 for P5 by changing array types to unsigned char.
19/03/2018: Implemented loops to check for both corrupted p5 and p2 files and return an error.
16/03/2018: Implemented rotate and rotate 90 functions.
14/03/2018: Fixed a bug with p5 to p2 conversion and p2 to p5 conversion in which the headers were not printing correctly.
05/03/2018: First implementation of the p2 to p5 conversion.
02/03/2018: Fixed p2 to p5 conversion to pass tests.
27/02/2018: First implementation of p5 to p2 conversion.
23/02/2018: Implemented the hexdump functionality.
21/02/2018: Implemented structure to check for argument errors and give an stderr output.
20/02/2018: Implemented version and usage functionality.
19/02/2018: created the structure for checking input arguments.

