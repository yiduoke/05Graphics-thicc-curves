#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "matrix.h"
#include "parser.h"

int main(int argc, char **argv) {

  screen s;
  struct matrix * edges;
  struct matrix * transform;

  color c;
  c.red = 230;
  c.green = 130;
  c.blue = 50;

  edges = new_matrix(4, 4);
  transform = new_matrix(4, 4);

  if ( argc == 2 )
    parse_file( argv[1], transform, edges, s );
  else
    parse_file( "stdin", transform, edges, s );

  // add_curve(edges, 0, 0, 100, 400, 400, 400, 500, 0, 0.005, 1);
  // draw_lines(edges, s, c);
  // save_extension(s, "thicc.png");

  free_matrix( edges );
  free_matrix( transform );
}  
