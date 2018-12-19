#ifndef HGVARS_H
#define HGVARS_H

#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<math.h>
#include<endian.h>
#include<string.h>
#include<ctype.h>
#include<unistd.h>

extern char d[21][80]; // 2D char array to represent entire dungeon
extern char* path; // array(pointer) for storing absolute path of the file
extern unsigned char h[21][80]; // 2D array to store hardness values
extern unsigned char* r; // Pointer to hold info on rooms
extern unsigned char pcjx; // character to hold the x position of PC(@)
extern unsigned char pciy; // character to hold the y position of PC(@)
extern int n; // number of rooms
extern int cn; // connected number of rooms
extern int nhz; // number of nodes with hardness zero
extern int ld; // flag for load
extern int sv; // flag for save
extern int dant[21][80],dat[21][80]; // 2D arrays to store the distances of every node to PC(@) for both tunneling and non tunneling monsters
extern unsigned int fversion; // for storing file version
extern unsigned int fsize; // for storing the size of the file



void gvars_def(); // function for initializing global variables

int rndm(int u, int l); // function for generating random numbers within a range

#endif
