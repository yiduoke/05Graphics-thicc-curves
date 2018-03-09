#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "matrix.h"

/*======== void add_circle() ==========
  Inputs:   struct matrix * points
            double cx
            double cy
            double r
            double step
  Returns:

  Adds the circle at (cx, cy) with radius r to points
  ====================*/
void add_circle( struct matrix * points,
                 double cx, double cy, double cz,
                 double r, double step ) {
                   double i;
                   double x;
                   double y;
                   for (i = 0; i < 1 + step; i += step){
                    if (i){ // not the first run
                      add_edge(points, x, y, cz, r * cos(2 * M_PI * i) + cx, r * sin(2 * M_PI * i) + cy, cz);
                      
                    }
                    x = r * cos(2 * M_PI * i) + cx;
                    y = r * sin(2 * M_PI * i) + cy;
                   }
}

/*======== void add_curve() ==========
Inputs:   struct matrix *points
         double x0
         double y0
         double x1
         double y1
         double x2
         double y2
         double x3
         double y3
         double step
         int type
Returns:

Adds the curve bounded by the 4 points passsed as parameters
of type specified in type (see matrix.h for curve type constants)
to the matrix points
====================*/
void add_curve( struct matrix *points, 
                double x0, double y0, 
                double x1, double y1, 
                double x2, double y2, 
                double x3, double y3, 
                double step, int type ) {
                  struct matrix* x_coefs;
                  struct matrix* y_coefs;
                  if (type){ // 1 for bezier
                    x_coefs = generate_curve_coefs(x0, x1, x2, x3, 1);
                    y_coefs = generate_curve_coefs(y0, y1, y2, y3, 1);
                  }
                  else{ // 0 for hermite
                    x_coefs = generate_curve_coefs(x0, x1, x2, x3, 0);
                    y_coefs = generate_curve_coefs(y0, y1, y2, y3, 0);
                  }

                  double ax = x_coefs->m[0][0];
                  double bx = x_coefs->m[1][0];
                  double cx = x_coefs->m[2][0];
                  double dx = x_coefs->m[3][0];

                  double ay = y_coefs->m[0][0];
                  double by = y_coefs->m[1][0];
                  double cy = y_coefs->m[2][0];
                  double dy = y_coefs->m[3][0];

                  double i;
                  double x = x0;
                  double y = y0;

                  for (i = step; i < 1 + step; i += step){
                    add_edge(points, x, y, 0, ax * powf(i, 3) + bx * powf(i,2) + cx * i + dx, ay * powf(i, 3) + by * powf(i,2) + cy * i + dy, 0);
                    x = ax*powf(i, 3) + bx*powf(i,2) + cx * i + dx;
                    y = ay*powf(i, 3) + by*powf(i,2) + cy * i + dy;
                  }
}


/*======== void add_point() ==========
Inputs:   struct matrix * points
         int x
         int y
         int z 
Returns: 
adds point (x, y, z) to points and increment points.lastcol
if points is full, should call grow on points
====================*/
void add_point( struct matrix * points, double x, double y, double z) {

  if ( points->lastcol == points->cols )
    grow_matrix( points, points->lastcol + 100 );
  
  points->m[0][ points->lastcol ] = x;
  points->m[1][ points->lastcol ] = y;
  points->m[2][ points->lastcol ] = z;
  points->m[3][ points->lastcol ] = 1;
  points->lastcol++;
} //end add_point

/*======== void add_edge() ==========
Inputs:   struct matrix * points
          int x0, int y0, int z0, int x1, int y1, int z1
Returns: 
add the line connecting (x0, y0, z0) to (x1, y1, z1) to points
should use add_point
====================*/
void add_edge( struct matrix * points, 
	       double x0, double y0, double z0, 
	       double x1, double y1, double z1) {
  add_point( points, x0, y0, z0 );
  add_point( points, x1, y1, z1 );
}

/*======== void draw_lines() ==========
Inputs:   struct matrix * points
         screen s
         color c 
Returns: 
Go through points 2 at a time and call draw_line to add that line
to the screen
====================*/
void draw_lines( struct matrix * points, screen s, color c) {

 if ( points->lastcol < 2 ) {
   printf("Need at least 2 points to draw a line!\n");
   return;
 }
 
 int point;
 for (point=0; point < points->lastcol-1; point+=2)
   draw_line( points->m[0][point],
	      points->m[1][point],
	      points->m[0][point+1],
	      points->m[1][point+1],
	      s, c);	       
}// end draw_lines









void draw_line(int x0, int y0, int x1, int y1, screen s, color c) {
  
  int x, y, d, A, B;
  //swap points if going right -> left
  int xt, yt;
  if (x0 > x1) {
    xt = x0;
    yt = y0;
    x0 = x1;
    y0 = y1;
    x1 = xt;
    y1 = yt;
  }
  
  x = x0;
  y = y0;
  A = 2 * (y1 - y0);
  B = -2 * (x1 - x0);  

  //octants 1 and 8
  if ( abs(x1 - x0) >= abs(y1 - y0) ) {

    //octant 1    
    if ( A > 0 ) {
      
      d = A + B/2;      
      while ( x < x1 ) {
	plot( s, c, x, y );
	if ( d > 0 ) {
	  y+= 1;
	  d+= B;
	}
	x++;
	d+= A;
      } //end octant 1 while
      plot( s, c, x1, y1 );
    } //end octant 1

    //octant 8
    else {
      d = A - B/2;
      
      while ( x < x1 ) {
	//printf("(%d, %d)\n", x, y);
	plot( s, c, x, y );
	if ( d < 0 ) {
	  y-= 1;
	  d-= B;
	}
	x++;
	d+= A;
      } //end octant 8 while
      plot( s, c, x1, y1 );
    } //end octant 8
  }//end octants 1 and 8

  //octants 2 and 7
  else {
    
    //octant 2    
    if ( A > 0 ) {
      d = A/2 + B;      

      while ( y < y1 ) {
	plot( s, c, x, y );
	if ( d < 0 ) {
	  x+= 1;
	  d+= A;
	}
	y++;
	d+= B;
      } //end octant 2 while
      plot( s, c, x1, y1 );
    } //end octant 2

    //octant 7
    else {
      d = A/2 - B;
      
      while ( y > y1 ) {
	plot( s, c, x, y );
	if ( d > 0 ) {
	  x+= 1;
	  d+= A;
	}
	y--;
	d-= B;
      } //end octant 7 while
      plot( s, c, x1, y1 );
    } //end octant 7   
  }//end octants 2 and 7  
} //end draw_line
