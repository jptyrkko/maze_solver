This is a maze solver application written for Buutti for evaluation purposes.

The use of the directories is as follows:

build         Location of the Release executable
build_debug   Location of the Debug executable
data          Input data, mazes as text files
examples      Output data, examples of runs
include       Location of the library header
lib           Library implementing a STL ordered set substitute
src           Location of the main program
test          Unit tests for the main program

Please note the following:

* The application is delivered unbuilt but it has two alternative automatic
  build mechanisms. The primary mechanism is "build_all_with_cmake.sh", a shell
  script calling cmake and make. It builds all of the executables, except
  the build_debug. An auxiliary build script "build_all_without_cmake.sh"
  will also build the application debug version and place it in build_debug.

* The executable is in "build" diretory. It is invoked as
  ./maze_solver ../data/the_chosen_maze.txt

* The input maze files in "data" have been provided by Buutti.

* The "examples" directory contains the outputs of the example runs.
  Both mazes are solved with three different algorithms and shown in various
  degrees (20, 150, and 200 steps as requested) of solving. Visited maze
  squares have been marked with 'a', 'b', 'c', and '.' depending on which
  step count interval they belong to. If the shortest path is drawn on
  these markers, the markers coinciding with the path are converted to
  'A', 'B', 'C', and 'O' respectively. Some algorithms that do not use
  path branching may use the uppercase symbols only.

* The source has its main() function separated into its own file. As the
  unit tests in the "test" library are supposed to be executable without
  any testing framework, the test application includes all the non-main
  functions of the main application into its body. Unit tests are then started
  by executing the test_maze_solver executable in the "test" directory, and
  they are run by calling the included functions from the test application's
  own main() function. The test set is written for demonstration purposes and
  is therefore not exhaustive.
  
The algorithms used are Dijkstra, Pledge, and Wall Follower. Pledge algorithm
can actually serve as the Wall Follower algorithm with merely a single added
mode flag. The mazes are subjected to various sanity checks and data harvesting
before the solving, but the algorithms run on a supposition that the only
thing known about the goal is that it is located at the external wall.
The maze and its extra data layers are stored in the memory as arrays which
can be always indexed by a linear index or row-and-column indices. Some
algorithms employ the compass points of north, east, south, west, with
north being up (towards smaller row numbers) and east being right (towrads
bigger column numbers).

No "install" functions have been provided to CMake, as this application
is not intended to become a permanent part of the target platform. The
executables are expected to be run from their respective directories:
build, build_debug, and test.
