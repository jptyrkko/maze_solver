# Libs
gcc -c -o ./lib/uniques.o ./lib/src/uniques.c -I include -O2 -Wno-unused-result
ar rcs ./lib/libuniques.a ./lib/uniques.o
rm ./lib/uniques.o
# Release
gcc -o build/maze_solver src/maze_solver.c -I ./lib -L ./lib -l uniques -O2 -Wno-unused-result
# Debug
gcc -o build_debug/maze_solver src/maze_solver.c -I ./lib -L ./lib -l uniques -Wno-unused-result -g
# Test
gcc -o test/test_maze_solver test/test_maze_solver.c -L ./lib -l uniques -O2  -Wno-unused-result
