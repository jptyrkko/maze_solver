/**************************************************************************\
*
* PROGRAM       maze_solver_functions
*
* DESCRIPTION   A demonstration program solving mazes
*
* ARGUMENTS     Input file name
*
* RETURNS       Error status (0 == Success)
*
* HISTORY       2023 01 01   JPT   First implementation
*
* NOTES         The maze traverser is called Pentti.
*
*               NOTE: The main() function is in the file maze_solver.c. Use
*               this file as an include.
*
\**************************************************************************/

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include "../include/uniques.h"

#define MAX_ALLOWED_ROWLENGTH   1021
#define MAX_EXITS                  4
#define PLEDGE_ENABLE           0x01

enum {START_EDGE, START_INSIDE};

typedef struct {
  int rows;
  int cols;
  int startrow;
  int startcol;
  int startcategory;
  int exitrow [MAX_EXITS];
  int exitcol [MAX_EXITS];
  int exits;
  int pledge_enable;
  olist_t *olist;
} mazeparam_t;

typedef struct {
  int row; int col; int idx; char atval;
  int steps;
  int unfolded_heading;
  int folded_heading;
  int northrow; int northcol; int northidx; char northval;
  int eastrow;  int eastcol;  int eastidx;  char eastval;
  int southrow; int southcol; int southidx; char southval;
  int westrow;  int westcol;  int westidx;  char westval;
  int aheadrow; int aheadcol; int aheadidx; char aheadval;
  int siderow;  int sidecol;  int sideidx;  char sideval;
} pentti_t;

typedef struct {
  int row;
  int col;
} parent_data_t;

typedef int (* solver_t) (pentti_t *, char *, mazeparam_t *);



/**************************************************************************\
*
* MACRO         LIN
*
* DESCRIPTION   Converts a coordinate pair into a linear index
*
* ARGUMENTS     row   The row coordinate
*               col   The column coordinate
*
* GLOBALS       -
*
* RETURNS       Linear index
*
* HISTORY       2023 01 01   JPT   First implementation
*
* NOTES         mazeparam is expected to be a pointer to initialized struct.
*
\**************************************************************************/

#define LIN(row,col) ((row) * mazeparam -> cols + (col))



/**************************************************************************\
*
* MACRO         ROWCOL
*
* DESCRIPTION   Converts a linear index into a coordinate pair
*
* ARGUMENTS     lin   Input linear index
*               row   The receiving row coordinate
*               col   The receiving column coordinate
*
* GLOBALS       -
*
* RETURNS       -
*
* HISTORY       2023 01 01   JPT   First implementation
*
* NOTES         mazeparam is expected to be a pointer to initialized struct.
*
\**************************************************************************/

#define ROWCOL(lin,row,col) {(row) = (lin) / mazeparam -> cols;		\
                             (col) = (lin) % mazeparam -> cols;}



/**************************************************************************\
*
* FUNCTION      goalfound
*
* DESCRIPTION   Signals the goal being found next to Pentti.
*
* ARGUMENTS     pentti       The maze walker object
*
* GLOBALS       -
*
* RETURNS       "Goal found" flag
*
* HISTORY       2023 01 01   JPT   First implementation
*
* NOTES         -
*
\**************************************************************************/

int goalfound(pentti_t *pentti)
{
  return ((int) (   (pentti -> northval == 'E')
                 || (pentti -> southval == 'E')
                 || (pentti -> eastval  == 'E')
                 || (pentti -> westval  == 'E')));
}



/**************************************************************************\
*
* FUNCTION      outside
*
* DESCRIPTION   Determines the belonging of a coordinate pair to the maze
*
* ARGUMENTS     mazeparam    The maze parameter struct
*               row          Inquired row
*               col          Inquired column
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

int outside(mazeparam_t *mazeparam, int row, int col)
{
  return ((int) (   (row < 0)
                 || (col < 0)
                 || (row >= mazeparam -> rows)
                 || (col >= mazeparam -> cols)));
}



/**************************************************************************\
*
* FUNCTION      mazeval
*
* DESCRIPTION   Maze square content retriever.
*
* ARGUMENTS     maze        The maze map
*               mazeparam   The maze parameter struct
*               row         Row of the interrogated location
*               col         Column of the interrogated location
*
* GLOBALS       -
*
* RETURNS       Maze square character
*
* HISTORY       2023 01 01   JPT   First implementation
*
* NOTES         Cells outside the maze are returned as '#'.
*
\**************************************************************************/

char mazeval(char *maze, mazeparam_t *mazeparam, int row, int col)
{
  if (outside(mazeparam, row, col)) return ('#');
  return (maze [LIN(row, col)]);
}



/**************************************************************************\
*
* FUNCTION      update_pentti
*
* DESCRIPTION   Precalculate the surroundings
*
* ARGUMENTS     pentti       The maze walker object
*               maze         The maze map main layer
*               mazeparam    The maze parameter struct
*               row          New placement row
*               col          New placement column
*
* GLOBALS       -
*
* RETURNS       -
*
* HISTORY       2023 01 01   JPT   First implementation
*
* NOTES         This function does not affect Pentti's position or
*               orientation. Use set_pentti_at() or turn_pentti() to change
*               them.
*
\**************************************************************************/

void update_pentti(pentti_t *pentti, char *maze, mazeparam_t *mazeparam)
{
  pentti -> folded_heading = (pentti -> unfolded_heading & 0x3);
  pentti -> idx = LIN (pentti -> row, pentti -> col);
  pentti -> atval = mazeval(maze, mazeparam, pentti -> row, pentti -> col);
  pentti -> northrow = pentti -> row - 1;
  pentti -> northcol = pentti -> col;
  pentti -> northidx = LIN (pentti -> northrow, pentti -> northcol);
  pentti -> northval = mazeval(maze, mazeparam, pentti -> northrow, pentti -> northcol);
  pentti -> southrow = pentti -> row + 1;
  pentti -> southcol = pentti -> col;
  pentti -> southidx = LIN (pentti -> southrow, pentti -> southcol);
  pentti -> southval = mazeval(maze, mazeparam, pentti -> southrow, pentti -> southcol);
  pentti -> eastrow  = pentti -> row;
  pentti -> eastcol  = pentti -> col + 1;
  pentti -> eastidx  = LIN (pentti -> eastrow, pentti -> eastcol);
  pentti -> eastval  = mazeval(maze, mazeparam, pentti -> eastrow, pentti -> eastcol);
  pentti -> westrow  = pentti -> row;
  pentti -> westcol  = pentti -> col - 1;
  pentti -> westidx  = LIN (pentti -> westrow, pentti -> westcol);
  pentti -> westval  = mazeval(maze, mazeparam, pentti -> westrow, pentti -> westcol);
  if (pentti -> folded_heading == 0) {
    pentti -> aheadrow = pentti -> eastrow;
    pentti -> aheadcol = pentti -> eastcol;
    pentti -> aheadidx = pentti -> eastidx;
    pentti -> aheadval = pentti -> eastval;
    pentti -> siderow  = pentti -> southrow;
    pentti -> sidecol  = pentti -> southcol;
    pentti -> sideidx  = pentti -> southidx;
    pentti -> sideval  = pentti -> southval;
  }
  if (pentti -> folded_heading == 1) {
    pentti -> aheadrow = pentti -> northrow;
    pentti -> aheadcol = pentti -> northcol;
    pentti -> aheadidx = pentti -> northidx;
    pentti -> aheadval = pentti -> northval;
    pentti -> siderow  = pentti -> eastrow;
    pentti -> sidecol  = pentti -> eastcol;
    pentti -> sideidx  = pentti -> eastidx;
    pentti -> sideval  = pentti -> eastval;
  }
  if (pentti -> folded_heading == 2) {
    pentti -> aheadrow = pentti -> westrow;
    pentti -> aheadcol = pentti -> westcol;
    pentti -> aheadidx = pentti -> westidx;
    pentti -> aheadval = pentti -> westval;
    pentti -> siderow  = pentti -> northrow;
    pentti -> sidecol  = pentti -> northcol;
    pentti -> sideidx  = pentti -> northidx;
    pentti -> sideval  = pentti -> northval;
  }
  if (pentti -> folded_heading == 3) {
    pentti -> aheadrow = pentti -> southrow;
    pentti -> aheadcol = pentti -> southcol;
    pentti -> aheadidx = pentti -> southidx;
    pentti -> aheadval = pentti -> southval;
    pentti -> siderow  = pentti -> westrow;
    pentti -> sidecol  = pentti -> westcol;
    pentti -> sideidx  = pentti -> westidx;
    pentti -> sideval  = pentti -> westval;
  }
}



/**************************************************************************\
*
* FUNCTION      set_pentti_at
*
* DESCRIPTION   Relocate to arbitrary coordinates and update
*
* ARGUMENTS     pentti       The maze walker object
*               maze         The maze map main layer
*               mazeparam    The maze parameter struct
*               row          New placement row
*               col          New placement column
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

void set_pentti_at(pentti_t *pentti, char *maze, mazeparam_t *mazeparam,
                   int row, int col)
{
  pentti -> row = row;
  pentti -> col = col;
  update_pentti(pentti, maze, mazeparam);
  pentti -> steps ++;
}



/**************************************************************************\
*
* FUNCTION      turn_pentti
*
* DESCRIPTION   Change the orientation
*
* ARGUMENTS     pentti       The maze walker object
*               maze         The maze map main layer
*               mazeparam    The maze parameter struct
*               turn         Addition to the orientation in full quadrants
*
* GLOBALS       -
*
* RETURNS       -
*
* HISTORY       2023 01 01   JPT   First implementation
*
* NOTES         unfolded heading runs from -MAX_INT to MAX_INT while
*               folded_heading is folded to [0 .. 3].
*
*               Turning does not count as a step. Do the step separately
*               when necessary.
*
\**************************************************************************/

void turn_pentti(pentti_t *pentti, char *maze, mazeparam_t *mazeparam,
                 int turn) {
  pentti -> unfolded_heading += turn;
  update_pentti(pentti, maze, mazeparam);
}



/**************************************************************************\
*
* FUNCTION      exclude
*
* DESCRIPTION   Sets exclusion data at given coordinates
*
* ARGUMENTS     mazeparam    The maze parameter struct
*               exclusion    Exclusion data map layer
*               row          Inquired row
*               col          Inquired column
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

void exclude (mazeparam_t *mazeparam, int *exclusion, int row, int col)
{
  int idx = LIN(row, col);
  if (! outside(mazeparam, row, col)) exclusion [idx] = 1;
  else fprintf(stderr, "Exclusion pointing outside maze!\n");
}



/**************************************************************************\
*
* FUNCTION      exclusion_at
*
* DESCRIPTION   Retrieves exclusion data at given coordinates
*
* ARGUMENTS     mazeparam    The maze parameter struct
*               exclusion    Exclusion data map layer
*               row          Inquired row
*               col          Inquired column
*
* GLOBALS       -
*
* RETURNS       Exclusion state at a maze square
*
* HISTORY       2023 01 01   JPT   First implementation
*
* NOTES         -
*
\**************************************************************************/

int exclusion_at(mazeparam_t *mazeparam, int *exclusion, int row, int col)
{
  int idx = LIN(row, col);
  if (outside(mazeparam, row, col)) return (1);
  return (exclusion [idx]);
}



/**************************************************************************\
*
* FUNCTION      get_parent_data
*
* DESCRIPTION   Retrieves parent coordinates from a maze map parallel layer
*
* ARGUMENTS     parent_data  Parallel map layer for coordinate pairs
*               mazeparam    The maze parameter struct
*               row          Inquired row
*               col          Inquired column
*               parentrow    Parent row value
*               parentcol    Parent column value
*
* GLOBALS       -
*
* RETURNS       -
*
* HISTORY       2023 01 01   JPT   First implementation
*
* NOTES         Used to find traversal step descendancy.
*
\**************************************************************************/

void get_parent_data (parent_data_t *parent_data, mazeparam_t *mazeparam,
                      int row, int col, int *parentrow, int *parentcol)
{
  int idx = LIN(row, col);
  *parentrow = parent_data [idx].row;
  *parentcol = parent_data [idx].col;
}



/**************************************************************************\
*
* FUNCTION      set_parent_data
*
* DESCRIPTION   Sets parent coordinates to a maze square of a parallel layer
*
* ARGUMENTS     parent_data  Parallel map layer for coordinate pairs
*               mazeparam    The maze parameter struct
*               targetrow    Receiving row
*               targetcol    Receiving column
*               row          Parent row value
*               col          Parent column value
*
* GLOBALS       -
*
* RETURNS       -
*
* HISTORY       2023 01 01   JPT   First implementation
*
* NOTES         Used to show traversal step descendancy.
*
\**************************************************************************/

void set_parent_data (parent_data_t *parent_data, mazeparam_t *mazeparam,
                      int targetrow, int targetcol, int row, int col)
{
  int idx = LIN(targetrow, targetcol);
  parent_data [idx].row = row;
  parent_data [idx].col = col;
}



/************************************************************************** \
*
* FUNCTION      readln_stripped
*
* DESCRIPTION   Read a maze row with unknown end characters
*
* ARGUMENTS     f     Read file pointer
*               buf   Line buffer
*               len   Line length [OUTPUT]
*
* GLOBALS       -
*
* RETURNS       -
*
* HISTORY       2023 01 01   JPT   First implementation
*
* NOTES         Removes irrelevant characters at the end of the line.
*
\**************************************************************************/

void readln_stripped(FILE *f, char *buf, int *len)
{
  *len = 0;
  buf [0] = '\0';
  (void) fgets (buf, MAX_ALLOWED_ROWLENGTH + 3, f);
  *len = strlen(buf);
  if (*len == 0) {
    buf [0] = '\0';
    return;
  }
  for (int i = *len - 1; i >= 0; i--) {
    if (! isgraph(buf [i])) {
      buf [i] = '\0';
      (*len)--;
    }
    else break;
  }
}



/************************************************************************** \
*
* FUNCTION      measuremaze
*
* DESCRIPTION   Maze size measurement
*
* ARGUMENTS     filename    The name of the text file containing a maze
*               mazeparam   The maze parameter struct
*
* GLOBALS       -
*
* RETURNS       Error code (0 == Success)
*
* HISTORY       2023 01 01   JPT   First implementation
*
* NOTES         Finds maze size for space allocation.
*
*               The outer wall of the maze is presumed to be a rectangle
*               to disambiguate the allowed maze area.
*
\**************************************************************************/

int measuremaze(char *filename, mazeparam_t *mazeparam)
{
  FILE *f  = NULL;
  int  len = 0;
  char buf [MAX_ALLOWED_ROWLENGTH + 3];   /* Allow \r\n\0 at the end */
  int  rv = 0;
  f = fopen (filename, "r");
  if (f == NULL)
  {
    perror("Cannot open input file");
    return (1);
  }
  mazeparam -> rows = 0;
  mazeparam -> cols = 0;
  for (len = 0; 1; len++) {
    char ch = getc (f);
    if (! isgraph(ch)) break;
    if (len > MAX_ALLOWED_ROWLENGTH) {
      fprintf (stderr,
               "Maze too wide, maximum allowed width is %d\n",
               MAX_ALLOWED_ROWLENGTH);
      return (1);
    }
  }
  if (len == 0)  {
    fprintf(stderr, "The first maze row was empty\n");
    fclose(f);
    return (1);
  }
  mazeparam -> cols = len;
  mazeparam -> rows ++;
  while (! feof(f)) {
    readln_stripped(f, buf, &len);
    if (len == 0) break;
    if (len != mazeparam -> cols) {
      fprintf(stderr, "Uneven row lengths in maze\n");
      fclose(f);
      return (1);
    }
    mazeparam -> rows ++;
  }
  fclose(f);
  return (0);
}



/**************************************************************************\
*
* FUNCTION      loadmaze
*
* DESCRIPTION   Maze input loader
*
* ARGUMENTS     filename    The name of the text file containing a maze
*               maze        The maze map [OUTPUT]
*               mazeparam   The maze parameter struct
*
* GLOBALS       -
*
* RETURNS       Error code (0 == Success)
*
* HISTORY       2023 01 01   JPT   First implementation
*
* NOTES         Expects a sanity-checked maze!
*
*               IMPORTANT: Allocates heap memory!
*
\**************************************************************************/

int loadmaze(char *filename, char **maze, mazeparam_t *mazeparam)
{
  FILE *f = NULL;
  char buf [MAX_ALLOWED_ROWLENGTH + 3];   /* Allow \r\n\0 at the end */
  f = fopen(filename, "r");
  if (f == NULL)
  {
    perror("Cannot open input file "
           "(Previous open succeeded? Error in code?)");
    return (1);
  }
  *maze = (char *) malloc(mazeparam -> cols * mazeparam -> rows);
  if (*maze == NULL) {
    fprintf(stderr, "Cannot allocate memory for maze.\n");
    fclose(f);
    return (1);
  }
  for (int r = 0 ; r < mazeparam -> rows ; r++) {
    int len = 0;
    (void) readln_stripped(f, buf, &len);
    memcpy(*(maze) + r * mazeparam -> cols, buf, mazeparam -> cols);
  }
  fclose(f);
  return (0);
}


    
/**************************************************************************\
*
* FUNCTION      endpoints
*
* DESCRIPTION   Maze path endpoint examination
*
* ARGUMENTS     maze        The maze map [OUTPUT]
*               mazeparam   The maze parameter struct
*
* GLOBALS       -
*
* RETURNS       Error code (0 == Success)
*
* HISTORY       2023 01 01   JPT   First implementation
*
* NOTES         Expects a correctly initialized mazeparam struct.
*
\**************************************************************************/

int endpoints (char *maze, mazeparam_t *mazeparam)
{
  mazeparam -> exits = 0;
  mazeparam -> startcategory = START_EDGE;
  for (int row = 0; row < mazeparam -> rows; row++) {
    for (int col = 0; col < mazeparam -> cols; col++) {
      char ch = mazeval(maze, mazeparam, row, col);
      if (ch == '^') {
        mazeparam -> startrow = row;
        mazeparam -> startcol = col;
        if (   (row > 0) && (row < mazeparam -> rows - 1)
            && (col > 0) && (col < mazeparam -> cols - 1)) {
          mazeparam -> startcategory = START_INSIDE;
        }
      }
      if (ch == 'E') {
        if (mazeparam -> exits == MAX_EXITS + 1) {
          fprintf(stderr, "Maximum number of exits (%d) exceeded.\n", MAX_EXITS);
          free(maze);
          return (1);
        }
        mazeparam -> exitrow [mazeparam -> exits] = row;
        mazeparam -> exitcol [mazeparam -> exits] = col;
        (mazeparam -> exits) ++;
      }
    }
  }
  return (0);
}



/**************************************************************************\
*
* FUNCTION      wall_sanity
*
* DESCRIPTION   Checks the sanity of the external walls
*
* ARGUMENTS     maze        The maze map [OUTPUT]
*               mazeparam   The maze parameter struct
*               abortflag   Flag to abort program at wall abnormality
*
* GLOBALS       -
*
* RETURNS       Sanity flag (0 == OK)
*
* HISTORY       2023 01 01   JPT   First implementation
*
* NOTES         Offers a choice to correct errors.
*
\**************************************************************************/

int wall_sanity(char *maze, mazeparam_t *mazeparam, int abortflag)
{
  int flag = 0;
  for (int i = 0; i < mazeparam -> cols; i++) {
    int north = i;
    int south = mazeparam -> cols * (mazeparam -> rows - 1) + i;
    flag = (int) ((maze [north] == ' ') || (maze [south] == ' '));
    if (abortflag && (flag != 0)) return (1);
    if (maze [north] == ' ') maze [north] = '#';
    if (maze [south] == ' ') maze [south] = '#';
  }
  for (int i = 0; i < mazeparam -> rows; i++) {
    int west = mazeparam -> cols * i;
    int east = west + mazeparam -> cols - 1;
    flag = (int) ((maze [west] == ' ') || (maze [east] == ' '));
    if (abortflag && (flag != 0)) return (1);
    if (maze [west] == ' ') maze [west] = '#';
    if (maze [east] == ' ') maze [east] = '#';
  }
  /* The following are incorrectable */
  if (   (mazeparam -> startrow == 0)
      && (maze [LIN(1, mazeparam -> startcol)] == '#')) {
    return (1);
  }
  if (   (mazeparam -> startrow == mazeparam -> rows - 1)
      && (maze [LIN(mazeparam -> rows - 2, mazeparam -> startcol)] == '#')) {
    return (1);
  }
  if (   (mazeparam -> startcol == 0)
      && (maze [LIN(mazeparam -> startrow, 1)] == '#')) {
    return (1);
  }
  if (   (mazeparam -> startcol == mazeparam -> cols - 1)
      && (maze [LIN(mazeparam -> startrow, mazeparam -> cols - 2)] == '#')) {
    return (1);
  }
  return (0);
}



/**************************************************************************\
*
* FUNCTION      printmaze
*
* DESCRIPTION   Maze result printer
*
* ARGUMENTS     maze        The maze map [INPUT]
*               mazeparam   The maze parameter struct
*               cleaned     Erasure of fruitless path branches
*
* GLOBALS       -
*
* RETURNS       Error code (0 == Success)
*
* HISTORY       2023 01 01   JPT   First implementation
*
* NOTES         Expects a preloaded maze and valid mazeparams contents.
*
\**************************************************************************/

int printmaze(char *maze, mazeparam_t *mazeparam, int cleaned)
{
  char s [MAX_ALLOWED_ROWLENGTH + 3];
  for (int r = 0; r < mazeparam -> rows; r++) {
    memcpy(s, maze + mazeparam -> cols * r, mazeparam -> cols);
    s [mazeparam -> cols] = '\0';
    if (r == mazeparam -> startrow) s [mazeparam -> startcol] = '^';
    if (cleaned) {
      for (int c = 0; c < mazeparam -> cols; c++) {
        if (strchr("abc.", s [c])) s [c] = ' ';
      }
    }
    printf("%s\n", s);
  }
}



/**************************************************************************\
*
* FUNCTION      checkside
*
* DESCRIPTION   Blockage and parenthood of a potential next step
*
* ARGUMENTS     mazeparam   The maze parameter struct
*               atval       Character value in the current maze square
*               siderow     Row of the potential next step
*               sidecol     Column of the potential next step
*               atrow       Current row
*               atcol       Current column
*
* GLOBALS       -
*
* RETURNS       Access status (0 == Vacant)
*
* HISTORY       2023 01 01   JPT   First implementation
*
* NOTES         Used for different directions.
*
\**************************************************************************/

void checkside (mazeparam_t *mazeparam, char *maze, int *exclusion,
                parent_data_t *parent_data,
                char atval, int siderow, int sidecol, int atrow, int atcol)
{
  int row, col;
  if (mazeval(maze, mazeparam, siderow, sidecol) == '#') return;
  if (exclusion_at (mazeparam, exclusion, siderow, sidecol)) return;
  get_parent_data (parent_data, mazeparam, siderow, sidecol, &row, &col);
  if ((row == -1) || (col == -1)) {
    insert_unique(&(mazeparam -> olist), LIN(siderow, sidecol));
    set_parent_data(parent_data, mazeparam, siderow, sidecol, atrow, atcol);
  }
}



/**************************************************************************\
*
* FUNCTION      dijkstra
*
* DESCRIPTION   Dijkstra's maze traversing algorithm
*
* ARGUMENTS     pentti      The current position and state
*               maze        The maze map [INPUT]
*               mazeparam   The maze parameter struct
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

int dijkstra(pentti_t *pentti, char *maze, mazeparam_t *mazeparam)
{
  int idx = 0;
  int row = 0;
  int col = 0;
  int found = 0;
  int mazesize = mazeparam -> rows * mazeparam -> cols;
  int *exclusion = (int *) malloc(mazesize * sizeof(int));
  parent_data_t *parent_data = (parent_data_t *) malloc(mazesize * sizeof(parent_data_t));
  memset(exclusion, 0, mazesize * sizeof(int));
  memset(parent_data, (unsigned char) 0xff, mazesize * sizeof (parent_data_t)); /* -1 */
  set_parent_data(parent_data, mazeparam,
                  mazeparam -> startrow, mazeparam -> startcol,
                  mazeparam -> startrow, mazeparam -> startcol);
  insert_unique(&(mazeparam -> olist), LIN(mazeparam -> startrow, mazeparam -> startcol));
  set_pentti_at(pentti, maze, mazeparam, row, col);
  pentti -> steps = 0;
  while (mazeparam -> olist) {
    idx = first_unique(mazeparam -> olist);
    delete_first_unique(&(mazeparam -> olist));
    ROWCOL (idx, row, col);
    exclude (mazeparam, exclusion, row, col);
    set_pentti_at(pentti, maze, mazeparam, row, col);
    maze [pentti -> idx] = '.';
    if (pentti -> steps < 200) maze [pentti -> idx] = 'c';
    if (pentti -> steps < 150) maze [pentti -> idx] = 'b';
    if (pentti -> steps <  20) maze [pentti -> idx] = 'a';
    if ((pentti -> steps == 20) || (pentti -> steps == 150) || (pentti -> steps == 200)) {
      printf("Intermediate state at %dth step\n\n", pentti -> steps);
      printmaze(maze, mazeparam, 0);
      printf("\n\n\n");
    }
    if (goalfound(pentti)) {
      int tgtrow, tgtcol, tracerow, tracecol;
      found = 1;
      if (pentti -> northval == 'E') {
        tgtrow = pentti -> northrow; tgtcol = pentti -> northcol;
      }
      if (pentti -> southval == 'E') {
        tgtrow = pentti -> southrow; tgtcol = pentti -> southcol;
      }
      if (pentti -> eastval == 'E') {
        tgtrow = pentti -> eastrow;  tgtcol = pentti -> eastcol;
      }
      if (pentti -> westval == 'E') {
        tgtrow = pentti -> westrow;  tgtcol = pentti -> westcol;
      }
      set_parent_data(parent_data, mazeparam,
                      tgtrow, tgtcol, pentti -> row, pentti -> col);
      tracerow = pentti -> row; tracecol = pentti -> col;
      while (1) {
        int idx, prow, pcol;
	get_parent_data(parent_data, mazeparam, tracerow, tracecol, &prow, &pcol);
        if ((tracerow == prow) && (tracecol == pcol)) break;
        idx = LIN(tracerow, tracecol);
        if      (maze [idx] == 'a') maze [idx] = 'A';
        else if (maze [idx] == 'b') maze [idx] = 'B';
        else if (maze [idx] == 'c') maze [idx] = 'C';
        else maze [idx] = 'O';
        tracerow = prow;
        tracecol = pcol;
      }
      break;
    }
    checkside(mazeparam, maze, exclusion, parent_data,
              pentti -> atval, pentti -> northrow, pentti -> northcol,
              pentti -> row, pentti -> col);
    checkside(mazeparam, maze, exclusion, parent_data,
              pentti -> atval,pentti -> eastrow, pentti -> eastcol,
              pentti -> row, pentti -> col);
    checkside(mazeparam, maze, exclusion, parent_data,
              pentti -> atval, pentti -> southrow, pentti -> southcol,
              pentti -> row, pentti -> col);
    checkside(mazeparam, maze, exclusion, parent_data,
              pentti -> atval, pentti -> westrow, pentti -> westcol,
              pentti -> row, pentti -> col);
  }  
  prune_uniques(mazeparam -> olist);
  mazeparam -> olist = NULL;
  free(parent_data);
  free(exclusion);
  if (found == 0) {
    printf("Exit not found after %d steps\n\n", pentti -> steps);
    return (1);
  }
  printf("Final state at %dth step\n\n", pentti -> steps);
  printmaze(maze, mazeparam, 0);
  printf("\n\n\n");
  return (0);
}



/**************************************************************************\
*
* FUNCTION      pledge
*
* DESCRIPTION   Pledge's maze traversing algorithm
*
* ARGUMENTS     pentti      The current position and state
*               maze        The maze map [INPUT]
*               mazeparam   The maze parameter struct
*
* GLOBALS       -
*
* RETURNS       Error code (0 == Success)
*
* HISTORY       2023 01 01   JPT   First implementation
*
* NOTES         This algorithm can be used as the classic wall-follower by
*               setting pledge_enable = 0.
*
*               Heading is expressed as quarter-revolutions to the positive
*               direction from east. This number may be reduced to its
*               two lowest bits in order to confine it within [0 .. 3] where
*               necessary.
*
\**************************************************************************/

int pledge(pentti_t *pentti, char *maze, mazeparam_t *mazeparam)
{
  int pledge_mode = 1;
  if (   (mazeparam -> pledge_enable == 0)
      && (mazeparam -> startcategory == START_INSIDE)) {
    printf("Classic wall follower may not have a solution for\n"
           "an internal starting point if the maze is not\n"
           "simply connected. Try with pledge_enable = 1.\n\n");
    return (2);
  }
  pentti -> unfolded_heading = 0;   /* East is the preferred heading */
  set_pentti_at(pentti, maze, mazeparam,
                mazeparam -> startrow, mazeparam -> startcol);
  while (1) {
    if (goalfound(pentti)) break;
    pledge_mode = (int) (   (mazeparam -> pledge_enable != 0)
                         && (pentti -> unfolded_heading == 0));
    if (pentti -> sideval != '#') {
      if (pledge_mode == 0) turn_pentti(pentti, maze, mazeparam, -1);
      set_pentti_at(pentti, maze, mazeparam,
                    pentti -> aheadrow, pentti -> aheadcol);
    }
    else if (pentti -> aheadval == '#')
      turn_pentti(pentti, maze, mazeparam, 1);
    else {
      set_pentti_at(pentti, maze, mazeparam,
                    pentti -> aheadrow, pentti -> aheadcol);
    }
    maze [pentti -> idx] = 'O';
    if (pentti -> steps < 200) maze [pentti -> idx] = 'C';
    if (pentti -> steps < 150) maze [pentti -> idx] = 'B';
    if (pentti -> steps <  20) maze [pentti -> idx] = 'A';
    if (   (pentti -> steps == 20)
        || (pentti -> steps == 150)
        || (pentti -> steps == 200)) {
      printf("Intermediate state at %dth step\n\n", pentti -> steps);
      printmaze(maze, mazeparam, 0);
      printf("\n\n\n");
    }
  }
  printf("Final state at %dth step\n\n", pentti -> steps);
  return (0);
}



/**************************************************************************\
*
* FUNCTION      mazeinit
*
* DESCRIPTION   Maze allocation, initialization and sanity check
*
* ARGUMENTS     filename    Maze file name
*               maze        The maze map
*               mazeparam   The maze parameter struct
*
* GLOBALS       -
*
* RETURNS       Error code (0 == Success)
*
* HISTORY       2023 01 01   JPT   First implementation
*
* NOTES         IMPORTANT: Allocates memory via loadmaze(). Freeing
*               allocated memory is necessary if loadmaze() returns 0
*               meaning successful completion.
*
\**************************************************************************/

int mazeinit (char *filename, char **maze, mazeparam_t *mazeparam) {
  int errcode;
  mazeparam -> olist = NULL;
  errcode = measuremaze(filename, mazeparam);
  if (errcode) return (1);
  errcode = loadmaze(filename, maze, mazeparam);
  if (errcode) return (1);
  errcode = endpoints(*maze, mazeparam);
  if (errcode) {free(*maze); *maze = NULL; return (1);}
  errcode = wall_sanity(*maze, mazeparam, 1);
  if (errcode) {free(*maze); *maze = NULL; return (1);}
  return (0);
}



/**************************************************************************\
*
* FUNCTION      solvemaze
*
* DESCRIPTION   Top level caller to invoke a single solving algorithm
*
* ARGUMENTS     solver     The chosen algorithm (a function pointer)
*               filename   The name of the maze fiole to solve
*               flags      Special requests (e.g. Pledge mode)
*
* GLOBALS       -
*
* RETURNS       Error code (0 == Success)
*
* HISTORY       2023 01 01   JPT   First implementation
*
* NOTES         IMPORTANT: Remember to free the memory allocated by
*               loadmaze() via mazeinit(). A failed mazeinit (nonzero
*               return)has released the memory for you already.
*
\**************************************************************************/

int solvemaze(solver_t solver, char *filename, int flags) {
  mazeparam_t mazeparam;
  char        *maze = NULL;
  pentti_t    pentti;
  int rv = 0;
  rv = mazeinit(filename, &maze, &mazeparam);
  if (rv != 0) return (rv);
  mazeparam.pledge_enable = (int) ((flags & PLEDGE_ENABLE) != 0);
  pentti.row   = mazeparam.startrow;
  pentti.col   = mazeparam.startcol;
  pentti.steps = 0;
  printf("Maze %s, size %d x %d.\n\n"
         "Legend: A = up to 20 steps, B = up to 150 steps,\n"
         "C = up to 200 steps, O = more than 200 steps\n\n",
         filename, mazeparam.cols, mazeparam.rows);
  rv = solver(&pentti, maze, &mazeparam);
  if (rv == 2) {
    free(maze);
    maze = NULL;
    return (rv);
  }
  int mazesize = mazeparam.rows * mazeparam.cols;
  int pl = 0;
  for (int i = 0; i < mazesize; i++) {
    if (strchr("ABCO", maze [i])) pl++;
  }
  printf("Solution, path has %d steps\n\n", pl);
  printmaze(maze, &mazeparam, 1);
  printf("\n\n\n");
  free(maze);
  maze = NULL;
  return (0);
}
