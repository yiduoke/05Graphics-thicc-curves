#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "matrix.h"
#include "parser.h"


/*======== void parse_file () ==========
Inputs:   char * filename
          struct matrix * transform,
          struct matrix * pm,
          screen s
Returns:

Goes through the file named filename and performs all of the actions listed in that file.
The file follows the following format:
     Every command is a single word that takes up a line
     Any command that requires arguments must have those arguments in the second line.
     The commands are as follows:
         line: add a line to the edge matrix -
	    takes 6 arguemnts (x0, y0, z0, x1, y1, z1)
	 ident: set the transform matrix to the identity matrix -
	 scale: create a scale matrix,
	    then multiply the transform matrix by the scale matrix -
	    takes 3 arguments (sx, sy, sz)
	 translate: create a translation matrix,
	    then multiply the transform matrix by the translation matrix -
	    takes 3 arguments (tx, ty, tz)
	 rotate: create an rotation matrix,
	    then multiply the transform matrix by the rotation matrix -
	    takes 2 arguments (axis, theta) axis should be x y or z
	 apply: apply the current transformation matrix to the
	    edge matrix
	 display: draw the lines of the edge matrix to the screen
	    display the screen
	 save: draw the lines of the edge matrix to the screen
	    save the screen to a file -
	    takes 1 argument (file name)
	 quit: end parsing

See the file script for an example of the file format


IMPORTANT MATH NOTE:
the trig functions int math.h use radian mesure, but us normal
humans use degrees, so the file will contain degrees for rotations,
be sure to conver those degrees to radians (M_PI is the constant
for PI)
====================*/
char ** parse_args( char * line ){
  char ** output = (char**)calloc(5, sizeof("ugaediwbdhadkhwd"));
  int i=0;
  while (line){
      char * str = strsep(&line," ");
      output[i]= str;
      i++;
  }
  return output;
}

// line: add a line to the point matrix -  takes 6 arguemnts (x0, y0, z0, x1, y1, z1)
// ident: set the transform matrix to the identity matrix
// scale: create a scale matrix, then multiply the transform matrix by the scale matrix -  takes 3 arguments (sx, sy, sz)
// move: create a translation matrix, then multiply the transform matrix by the translation matrix - takes 3 arguments (tx, ty, tz)
// rotate: create a rotation matrix, then multiply the transform matrix by the rotation matrix - takes 2 arguments (axis theta)
// apply: apply the current transformation matrix to the edge matrix
// display: draw the lines of the point matrix to the screen, display the screen
// save: draw the lines of the point matrix to the screen/frame save the screen/frame to a file - takes 1 argument (file name)

void parse_file ( char * filename,
                  struct matrix * transform,
                  struct matrix * edges,
                  screen s) {
  char state[40];

  color c;
  c.red = 0;
  c.green = MAX_COLOR;
  c.blue = MAX_COLOR;

  strcpy(state, "nothing");

  FILE *f;
  char line[256];
  clear_screen(s);

  if ( strcmp(filename, "stdin") == 0 )
    f = stdin;
  else
    f = fopen(filename, "r");

  while ( fgets(line, 255, f) != NULL ) {
    line[strlen(line)-1]='\0';
    printf(":%s:\n",line);
    // checks each command that needs arguments
    if (!strncmp(line, "line", 4)){
      strcpy(state, "line");
    }
    else if(!strncmp(line, "scale", 5)){
      strcpy(state, "scale");
    }
    else if(!strncmp(line, "move", 4)){
      strcpy(state, "translate");
    }
    else if(!strncmp(line, "rotate", 6)){
      strcpy(state, "rotate");
    }
    else if(!strncmp(line, "save", 4)){
      strcpy(state, "save");
    }
    else if(!strncmp(line, "circle", 6)){
      strcpy(state, "circle");
    }
    else if(!strncmp(line, "hermite", 7)){
      strcpy(state, "hermite");
    }
    else if(!strncmp(line, "bezier", 6)){
      strcpy(state, "bezier");
    }

    // now no argument commands
    else if(!strncmp(line, "ident", 5)){
      ident(transform);
    }
    else if(!strncmp(line, "apply", 5)){
      matrix_mult(transform, edges);
      clear_screen(s);
    }
    else if(!strncmp(line, "display", 7)){
      draw_lines(edges, s, c);
      display(s);
    }
    else if(!strncmp(line, "quit", 4)){
      break;
    }
    else{
      if(!strncmp(state, "line", 4)){

        int x0, y0, z0, x1, y1, z1;
        sscanf( line, "%d %d %d %d %d %d", &x0, &y0, &z0, &x1, &y1, &z1 );
        add_edge(edges, x0, y0, z0, x1, y1, z1);
        strcpy(state, "nothing");
      }
      else if(!strncmp(state, "scale", 5)){

        float x, y, z;
        sscanf( line, "%f %f %f", &x, &y, &z);
        matrix_mult(make_scale(x, y, z), transform);
        strcpy(state, "nothing");
      }
      else if(!strncmp(state, "translate", 9)){

        int x, y, z;
        sscanf( line, "%d %d %d", &x, &y, &z);
        matrix_mult(make_translate(x, y, z), transform);
        strcpy(state, "nothing");
      }
      else if(!strncmp(state, "circle", 6)){
        
                float cx, cy, cz, r;
                sscanf( line, "%f %f %f %f", &cx, &cy, &cz, &r);
                add_circle(edges, cx, cy, cz, r, 0.005);
                strcpy(state, "nothing");
      }
      else if(!strncmp(state, "hermite", 7)){
        
                float px0, py0, px1, py1, rx0, ry0, rx1, ry1;
                sscanf( line, "%f %f %f %f %f %f %f %f", &px0, &py0, &px1, &py1, &rx0, &ry0, &rx1, &ry1);
                add_curve(edges, px0, py0, px1, py1, rx0, ry0, rx1, ry1, 0.005, 0);
                strcpy(state, "nothing");
      }
      else if(!strncmp(state, "bezier", 6)){
        
                float px0, py0, px1, py1, rx0, ry0, rx1, ry1;
                sscanf( line, "%f %f %f %f %f %f %f %f", &px0, &py0, &px1, &py1, &rx0, &ry0, &rx1, &ry1);
                add_curve(edges, px0, py0, px1, py1, rx0, ry0, rx1, ry1, 0.005, 1);
                strcpy(state, "nothing");
      }
      else if(!strncmp(state, "rotate", 6)){

        char axis[1];
        int theta;
        sscanf( line, "%s %d", axis, &theta);

        if (!strncmp(axis, "x", 1)){
          matrix_mult(make_rotX(theta), transform);
        }
        else if (!strncmp(axis, "y", 1)){
          matrix_mult(make_rotY(theta), transform);
        }
        else {
          matrix_mult(make_rotZ(theta), transform);
        }
        strcpy(state, "nothing");
      }
      else if(!strncmp(state, "save", 4)){

        char file[100];
        sscanf( line, "%s", file);
        display(s);
        save_extension(s, file);
        strcpy(state, "nothing");
      }
    }
  }
}
