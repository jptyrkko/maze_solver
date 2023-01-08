/************************************************************************** \
*
* PROGRAM       test_maze_solver
*
* DESCRIPTION   Unit tests for maze_solver.c
*
* ARGUMENTS     -
*
* RETURNS       Error status (0 == Success)
*
* HISTORY       2023 01 01   JPT   First implementation
*
* NOTES         The testable becomes included in this file.
*
\**************************************************************************/

#include <assert.h>
#include "../src/maze_solver_functions.c"



/************************************************************************** \
*
* FUNCTION      test_goalfound
*
* DESCRIPTION   Goal vicinity detection test
*
* ARGUMENTS     pentti      The current position and state
*               maze        The maze map
*               mazeparam   The maze parameter struct
*
* GLOBALS       -
*
* RETURNS       -
*
* HISTORY       2023 01 01   JPT   First implementation
*
* NOTES         -
*
\**************************************************************************/

void test_goalfound(pentti_t *pentti, char *maze, mazeparam_t *mazeparam)
{
  set_pentti_at(pentti, maze, mazeparam, 1, 7);
  assert(goalfound (pentti) == 1);
  set_pentti_at(pentti, maze, mazeparam, 1, 8);
  assert(goalfound (pentti) == 0);
}



/************************************************************************** \
*
* FUNCTION      test_outside
*
* DESCRIPTION   Out-of-bounds detection test
*
* ARGUMENTS     mazeparam   The maze parameter struct
*
* GLOBALS       -
*
* RETURNS       -
*
* HISTORY       2023 01 01   JPT   First implementation
*
* NOTES         Cells outside the maze are returned as '#'.
*
\**************************************************************************/

void test_outside(mazeparam_t *mazeparam)
{
  assert(outside(mazeparam, 0, 0) == 0);
  assert(outside(mazeparam, mazeparam -> rows - 1, mazeparam -> cols - 1) == 0);
  assert(outside(mazeparam, mazeparam -> rows,     mazeparam -> cols - 1) == 1);
  assert(outside(mazeparam, mazeparam -> rows - 1, mazeparam -> cols)     == 1);
}



/************************************************************************** \
*
* FUNCTION      test_mazeval
*
* DESCRIPTION   Maze square content retrieval test
*
* ARGUMENTS     maze        The maze map
*               mazeparam   The maze parameter struct
*
* GLOBALS       -
*
* RETURNS       -
*
* HISTORY       2023 01 01   JPT   First implementation
*
* NOTES         -
*
\**************************************************************************/

void test_mazeval(char *maze, mazeparam_t *mazeparam)
{
  assert(mazeval(maze, mazeparam,  0, 7) == 'E');
  assert(mazeval(maze, mazeparam,  1, 7) == ' ');
  assert(mazeval(maze, mazeparam,  0, 8) == '#');
  assert(mazeval(maze, mazeparam, -1, 1) == '#');
}



/************************************************************************** \
*
* FUNCTION      test_set_pentti_at
*
* DESCRIPTION   Pentti value field update test
*
* ARGUMENTS     pentti      The current position and state
*               maze        The maze map
*               mazeparam   The maze parameter struct
*
* GLOBALS       -
*
* RETURNS       -
*
* HISTORY       2023 01 01   JPT   First implementation
*
* NOTES         -
*
\**************************************************************************/

void test_set_pentti_at(pentti_t *pentti, char *maze, mazeparam_t *mazeparam)
{
  pentti -> steps = 0;
  set_pentti_at (pentti, maze, mazeparam, 10, 20);
  assert(pentti -> northrow ==    9);
  assert(pentti -> northcol ==   20);
  assert(pentti -> northidx ==  9 * mazeparam -> cols + 20);
  assert(pentti -> eastrow  ==   10);
  assert(pentti -> eastcol  ==   21);
  assert(pentti -> eastidx  == 10 * mazeparam -> cols + 21);
  assert(pentti -> southrow ==   11);
  assert(pentti -> southcol ==   20);
  assert(pentti -> southidx == 11 * mazeparam -> cols + 20);
  assert(pentti -> westrow  ==   10);
  assert(pentti -> westcol  ==   19);
  assert(pentti -> westidx  == 10 * mazeparam -> cols + 19);
  assert(pentti -> steps    ==    1);
}



/************************************************************************** \
*
* FUNCTION      test_turn_pentti
*
* DESCRIPTION   Pentti value field update test for turning
*
* ARGUMENTS     pentti      The current position and state
*               maze        The maze map
*               mazeparam   The maze parameter struct
*
* GLOBALS       -
*
* RETURNS       -
*
* HISTORY       2023 01 01   JPT   First implementation
*
* NOTES         A turn does not count as a step.
*
\**************************************************************************/

void test_turn_pentti(pentti_t *pentti, char *maze, mazeparam_t *mazeparam)
{
  pentti -> unfolded_heading = 4;
  set_pentti_at (pentti, maze, mazeparam, 1, 1);
  assert(pentti -> folded_heading == 0);
  assert(pentti -> southval == ' ');
  assert(pentti -> eastval  == '#');
  assert(pentti -> aheadval == '#');
  assert(pentti -> sideval  == ' ');
  turn_pentti (pentti, maze, mazeparam, 1);
  assert(pentti -> folded_heading == 1);
  assert(pentti -> southval == ' ');
  assert(pentti -> eastval  == '#');
  assert(pentti -> aheadval == '#');
  assert(pentti -> sideval  == '#');
}



/************************************************************************** \
*
* FUNCTION      test_exclude
*
* DESCRIPTION   Dijkstra algorithm exclusion marker test
*
* ARGUMENTS     mazeparam   The maze parameter struct
*               exclusion   Exclusion map layer
*
* GLOBALS       -
*
* RETURNS       -
*
* HISTORY       2023 01 01   JPT   First implementation
*
* NOTES         -
*
\**************************************************************************/

void test_exclude(mazeparam_t *mazeparam, int *exclusion)
{
  assert(exclusion_at(mazeparam, exclusion, 1, 1) == 0);
  exclude(mazeparam, exclusion, 1, 1);
  assert(exclusion_at(mazeparam, exclusion, 1, 1) == 1);
  assert(exclusion_at(mazeparam, exclusion, 2, 1) == 0);
}



/************************************************************************** \
*
* FUNCTION      test_set_parent_data
*
* DESCRIPTION   Test for parental data setting and retrieval
*
* ARGUMENTS     parent_data  Parallel map layer for coordinate pairs
*               mazeparam    The maze parameter struct
*
* GLOBALS       -
*
* RETURNS       -
*
* HISTORY       2023 01 01   JPT   First implementation
*
* NOTES         Tests also get_parent_data.
*
\**************************************************************************/

void test_set_parent_data(parent_data_t *parent_data, mazeparam_t *mazeparam)
{
  int row, col;
  get_parent_data(parent_data, mazeparam, 1, 1, &row, &col);
  assert(row == -1);
  assert(col == -1);
  set_parent_data(parent_data, mazeparam, 1, 1, 30, 40);
  get_parent_data(parent_data, mazeparam, 1, 1, &row, &col);
  assert(row == 30);
  assert(col == 40);
}



/************************************************************************** \
*
* FUNCTION      test_dijkstra
*
* DESCRIPTION   Dijkstra's algorithm exclusion and parent data tests
*
* ARGUMENTS     maze        The maze map [INPUT]
*               mazeparam   The maze parameter struct
*
* GLOBALS       -
*
* RETURNS       -
*
* HISTORY       2023 01 01   JPT   First implementation
*
* NOTES         These tests are performed in Dijkstra algorthm frame, as
*               their resources are available only within that frame.
*
\**************************************************************************/

void test_dijkstra(char *maze, mazeparam_t *mazeparam)
{
  int mazesize = mazeparam -> rows * mazeparam -> cols;
  assert(mazesize == 19 * 37);
  int *exclusion = (int *) malloc (mazesize * sizeof(int));
  parent_data_t *parent_data =
    (parent_data_t *) malloc (mazesize * sizeof(parent_data_t));
  memset(exclusion, 0, mazesize * sizeof(int));
  memset(parent_data,
         (unsigned char) 0xff,
         mazesize * sizeof(parent_data_t));   /* Produces int -1's */
  test_exclude(mazeparam, exclusion);
  test_set_parent_data(parent_data, mazeparam);
  free(parent_data);
  free(exclusion);
}



/************************************************************************** \
*
* FUNCTION      test_mazeinit
*
* DESCRIPTION   Maze allocation, initialization and sanity tests
*
* ARGUMENTS     filename    Maze file name
*               maze        The maze map
*               mazeparam   The maze parameter struct
*
* GLOBALS       -
*
* RETURNS       -
*
* HISTORY       2023 01 01   JPT   First implementation
*
* NOTES         IMPORTANT: Allocates memory via loadmaze().
*
*               Tests mazeinit() components as if mazeinit() had been called.
*
*               Error code values are ad-hoc for pre- and postallocation.
*
\**************************************************************************/

void test_mazeinit(char *filename, char **maze, mazeparam_t *mazeparam)
{
  int errcode;
  mazeparam -> olist = NULL;
  errcode = measuremaze(filename, mazeparam);
  assert(mazeparam -> rows == 190);
  assert(mazeparam -> cols == 37);
  assert(errcode == 0);
  errcode = loadmaze(filename, maze, mazeparam);
  assert(errcode == 0);
  errcode = endpoints(*maze, mazeparam);
  assert(errcode == 0);
  assert(mazeparam -> startrow == 18);
  assert(mazeparam -> startcol == 18);
  assert(mazeparam -> startcategory == START_EDGE);
  assert(mazeparam -> exitrow [0] == 0);
  assert(mazeparam -> exitcol [0] == 7);
  errcode = wall_sanity(*maze, mazeparam, 1);
  assert(errcode==0);
  *(*maze + LIN(18, 2)) = ' ';   /* Faulty data */
  *(*maze + LIN(18, 3)) = ' ';
  *(*maze + LIN(18, 4)) = ' ';
  errcode = wall_sanity(*maze, mazeparam, 1);
  assert(errcode==1);
  *(*maze + LIN(18, 2)) = '#';   /* Cleanup for following functions */
  *(*maze + LIN(18, 3)) = '#';
  *(*maze + LIN(18, 4)) = '#';
}



/************************************************************************** \
*
* FUNCTION      main
*
* DESCRIPTION   Program entry point
*
* ARGUMENTS     -
*
* GLOBALS       -
*
* RETURNS       Error code (0 == Success)
*
* HISTORY       2023 01 01   JPT   First implementation
*
* NOTES         -
*
\**************************************************************************/

int main () {
  mazeparam_t mazeparam;
  char        *maze = NULL;
  pentti_t    pentti;
  char        filename [PATH_MAX];
  strcpy(filename, "../data/maze-task-first.txt");
  FILE *f = fopen (filename, "r");
  fclose (f);
  test_mazeinit(filename, &maze, &mazeparam);
  test_set_pentti_at(&pentti, maze, &mazeparam);
  test_turn_pentti(&pentti, maze, &mazeparam);
  test_goalfound(&pentti, maze, &mazeparam);
  test_outside(&mazeparam);
  test_mazeval(maze, &mazeparam);
  test_dijkstra(maze, &mazeparam);
  free(maze);
  return (0);
}
