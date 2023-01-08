FILE=build/maze_solver
if [ -f "$FILE" ]; then
    make clean
fi
cmake CMakeLists.txt
make
