/**************************************************************************\
*
* PROGRAM       maze_solver
*
* DESCRIPTION   A demonstration program solving mazes
*
* ARGUMENTS     Input file name
*
* RETURNS       Error status (0 == Success)
*
* HISTORY       2023 01 01   JPT   First implementation
*
* NOTES         The program functions are included with
*               maze_solver_functions.c. This is to facilitate the easy
*               changing of main() function.
*
\**************************************************************************/

#include "maze_solver_functions.c"

/************************************************************************** \
*
* FUNCTION      main
*
* DESCRIPTION   Program entry point
*
* ARGUMENTS     argc   Command line argument count
*               argv   Command line argument vector (See NOTES)
*
* GLOBALS       -
*
* RETURNS       Error code (0 == Success)
*
* HISTORY       2023 01 01   JPT   First implementation
*
* NOTES         The program is invoked by "maze_solver <maze_file>"
*
\**************************************************************************/

int main (int argc, char *argv [])
{
  char filename [PATH_MAX];
  int  rv = 0;
  if (argc < 2) {
    printf("Missing input file. Use: maze_solver <maze_file>\n");
    return (1);
  }
  strcpy(filename, argv [1]);
  printf("---- DIJKSTRA --------------------------\n\n");
  rv += solvemaze(dijkstra, filename, 0);
  printf("---- PLEDGE ----------------------------\n\n");
  rv += solvemaze(pledge, filename, PLEDGE_ENABLE);
  printf("---- WALL FOLLOWER ---------------------\n\n");
  rv += solvemaze(pledge, filename, 0);
  return (rv);
}
