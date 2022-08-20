// main.cpp implements the driver for the game

#include <iostream>
#include <stdlib.h>
#include <conio.h>

#include "minesweeper2.h"

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define KEY_ESC 27

// GLOBAL
MineField *mf = nullptr;
char** recent_report = nullptr;
int field_size = 0;

// current selected cell
int row_select = 0;
int col_select = 0;


void redraw_field() {
    // clear the screen
    system("cls");

    // print the field
    if (recent_report != nullptr) {
        for (int row = 0; row < field_size; row++) {
            for (int col = 0; col < field_size; col++) {
                if (row == row_select && col == col_select) { // printing selected cell
                    if (recent_report[row][col] == 'X') {  // a mine
                        // bold, inverse, red X
                        std::cout << "\033[1;7;31m";
                    } else {
                        // inverse
                        std::cout << "\033[7m";
                    }
                    std::cout << recent_report[row][col] << "\033[0m";
                } else { // printing unselected cell
                    if (recent_report[row][col] == 'X') {  // a mine
                        // bold, red X
                        std::cout << "\033[1;31m";
                    }
                    std::cout << recent_report[row][col]  << "\033[0m";
                }
                std::cout << " ";
            }
            std::cout << std::endl;
        }
    } else {
        // print empty field while highlighting which cell is selected
        for (int row = 0; row < field_size; row++) {
            for (int col = 0; col < field_size; col++) {
                if (row == row_select && col == col_select) { // printing selected cell
                    std::cout << "\033[1;7;31m";
                    std::cout << " ";
                    std::cout << "\033[0m";
                } else { // printing unselected cell
                    std::cout << " ";
                }
                std::cout << " ";
            }
            std::cout << std::endl;
        }
    }

    // print guidance
    std::cout << "===============================" << std::endl;
    std::cout << "Move - Arrow Keys" << std::endl;
    std::cout << "Flag/Question/Clear - Spacebar" << std::endl;
    std::cout << "Reveal - Enter" << std::endl;
    std::cout << "Reset - R" << std::endl;
    std::cout << "Quit - Esc" << std::endl;
    std::cout << "===============================" << std::endl;
}

int main(int argc, char* argv[]) {
    // parse command line arguments: prog size mines
    int size = 0;
    int mines = 0;
    
    if (argc == 3) {
        size = atoi(argv[1]);
        mines = atoi(argv[2]);
        std::cout << "Size: " << size << std::endl << "Mines: " << mines << std::endl;
    } else {
        std::cout << "You may also run" << argv[0] << " SIZE MINES to start game." << std::endl;
        std::cout << "Size: ";
        std::cin >> size;
        std::cout << "Mines: ";
        std::cin >> mines;
    }

    if (size < 1) {
        std::cout << "Size must be positive" << std::endl;
        return 1;
    }
    if (mines < 1 || mines > size * size) {
        std::cout << "Mines must be between 1 and " << size * size << std::endl;
        return 1;
    }

    field_size = size;
    mf = new MineField(size, mines); // still needs to be reset

    while (true) {
        int c, ex;
        bool game_started = false;

        redraw_field();

        // Display empty field and wait for user to reveal first cell
        while(!game_started) {    
            c = getch();
            if (!c || c == 224) {
                // arrow
                switch(ex = getch()) {
                    case KEY_UP:
                        if (row_select > 0) {
                            row_select--;
                        }
                        break;
                    case KEY_DOWN:
                        if (row_select < size - 1) {
                            row_select++;
                        }
                        break;
                    case KEY_LEFT:
                        if (col_select > 0) {
                            col_select--;
                        }
                        break;
                    case KEY_RIGHT:
                        if (col_select < size - 1) {
                            col_select++;
                        }
                        break;
                    default:
                        break;
                }
                redraw_field();
            } else {
                switch(c) {
                    case ' ':
                        redraw_field();
                        // flag/question/clear
                        std::cout << "You must reveal a cell first" << std::endl;
                        break;
                    case '\r':
                        // reveal
                        game_started = true;
                        // reset the field with the selected cell as the first cell to be revealed
                        mf->reset( row_select, col_select );
                        redraw_field();
                        break;
                    case KEY_ESC: // esc
                        // quit
                        return 0;
                        break;
                }
            }
            
        }

        // game started
        bool game_restarting = false;
        while (!game_restarting) {
            redraw_field();
            c = getch();
            if (!c || c == 224) {
                // arrow
                switch(ex = getch()) {
                    case KEY_UP:
                        if (row_select > 0) {
                            row_select--;
                        }
                        break;
                    case KEY_DOWN:
                        if (row_select < size - 1) {
                            row_select++;
                        }
                        break;
                    case KEY_LEFT:
                        if (col_select > 0) {
                            col_select--;
                        }
                        break;
                    case KEY_RIGHT:
                        if (col_select < size - 1) {
                            col_select++;
                        }
                        break;
                    default:
                        break;
                }
            } else {
                switch(c) {
                    case ' ':
                        // flag/question/clear
                        mf->flag(row_select, col_select);
                        recent_report = mf->field_report(field_size);
                        break;
                    case '\r':
                    {
                        // reveal
                        int revealed = mf->reveal(row_select, col_select);
                        recent_report = mf->field_report(field_size);
                        if (revealed != -1) {
                            // game ends
                            redraw_field();
                            if (revealed == 0) {
                                std::cout << "You win!" << std::endl;
                            } else {
                                std::cout << "You should have lost " << revealed << " times, for mines are moved for you (when you about to trigger it) this many times." << std::endl;
                            }
                            #ifdef _WIN32
                                system("pause");
                            #elif __APPLE__
                                system("read");
                            #elif __linux__
                                system("read");
                            #endif
                            return 0;
                        }
                        break;
                    }
                    case 'r': // reset
                    case 'R':
                        game_restarting = true;
                        recent_report = nullptr;
                        break;
                    case KEY_ESC: // esc
                        // quit
                        return 0;
                        break;
                }
            }
        }
    }


    return 0;
}
