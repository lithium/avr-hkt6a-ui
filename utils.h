#ifndef UTILS_H
#define UTILS_H

#define MAP(x,imin,imax, omin,omax)  ( ((x)-(imin)) * ((omax)-(omin)) / ((imax)-(imin)) + (omin) )

  // return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

#endif