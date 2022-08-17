// Executables must have the following defined if the library contains
// doctest definitions. For builds with this disabled, e.g. code shipped to
// users, this can be left out.
#ifdef ENABLE_DOCTEST_IN_LIBRARY
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest/doctest.h"
#endif

#include <iostream>
#include <stdlib.h>

#include "minesweeper2.h"

/*
 * Simple main program that demontrates how access
 * CMake definitions (here the version number) from source code.
 */
int main(int argc, char* argv[]) {
    // parse command line arguments: prog size mines
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " SIZE MINES" << std::endl;
        return 1;
    }
    int size = atoi(argv[1]);
    int mines = atoi(argv[2]);
    if (size < 1 || size > 10) {
        std::cout << "Size must be between 1 and 10" << std::endl;
        return 1;
    }
    if (mines < 1 || mines > size * size) {
        std::cout << "Mines must be between 1 and " << size * size << std::endl;
        return 1;
    }

    MineField mf = MineField(size, mines);

    bool won = false;
    while (!won) {
        // print the field
        mf.print_field();
        // ask the user for a move
        std::cout << "Enter a cell (e.g. A1): ";
        std::string cell;
        std::cin >> cell;
        // parse the cell
        int y = cell[0] - 65;
        int x = cell[1] - 48;

        std::cout << "Enter an action from R(eveal)/F(lag)/Q(uestion)/C(lear): ";
        std::string action;
        std::cin >> action;
        // parse the action
        int act = ACTION_REVEAL;
        if (action == "R") {
            act = ACTION_REVEAL;
        } else if (action == "F") {
            act = ACTION_FLAG;
        } else if (action == "Q") {
            act = ACTION_QUESTION;
        } else if (action == "C") {
            act = ACTION_CLEAR;
        } else {
            std::cout << "Invalid action, must be R/F/Q/C" << std::endl;
            continue;
        }
        // make the move  
        won = mf.act(x, y, act);
    }
    // print the field one last time
    mf.print_field();
    std::cout << "You won!" << std::endl;
}
