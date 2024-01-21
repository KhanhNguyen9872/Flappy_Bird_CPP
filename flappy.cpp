#include <iostream>
#include <windows.h>
#include <string>
#include <conio.h>
#include <thread>

#pragma comment(lib, "winmm.lib")

#define BLACK			0
#define BLUE			1
#define GREEN			2
#define CYAN			3
#define RED				4
#define MAGENTA			5
#define BROWN			6
#define LIGHTGRAY		7
#define DARKGRAY		8
#define LIGHTBLUE		9
#define LIGHTGREEN		10
#define LIGHTCYAN		11
#define LIGHTRED		12
#define LIGHTMAGENTA	13
#define YELLOW			14
#define WHITE			15

using namespace std;

bool sound_active = true;
int terminalColumns, terminalRows;
string smallLogo = "";
int tmp_count = 0;

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
COORD newPos = {0, 0};
DWORD written;

void inputMenu(int *chooseMenu, int max, int type_menu);

void clearTerminal() {
    GetConsoleScreenBufferInfo(hConsole, &bufferInfo);

    COORD size = {bufferInfo.srWindow.Right - bufferInfo.srWindow.Left + 1, bufferInfo.srWindow.Bottom - bufferInfo.srWindow.Top + 1};

    SetConsoleCursorPosition(hConsole, newPos);

    FillConsoleOutputCharacter(hConsole, ' ', size.X * size.Y, newPos, &written);

    SetConsoleCursorPosition(hConsole, newPos);
    return;
}

void hideCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;

    cursorInfo.dwSize = 1;
    cursorInfo.bVisible = FALSE;

    SetConsoleCursorInfo(hConsole, &cursorInfo);
    return;
}

void getTerminalSize(int& columns, int& rows) {
    columns = bufferInfo.srWindow.Right - bufferInfo.srWindow.Left + 1;
    rows = bufferInfo.srWindow.Bottom - bufferInfo.srWindow.Top + 1;
}

void color(int index) {
    SetConsoleTextAttribute(hConsole, index);
    return;
}

string centerText(string text[], int size) {
    string finalText = "";
    string lineSpace = "";

    for(int i=0; i < (terminalColumns - text[0].length()) / 2; i++) {
        lineSpace = lineSpace + " ";
    }

    for(int i=0; i < size; i++) {
        finalText = finalText + lineSpace + text[i] + "\n";  
    }
    return finalText;
}

void banner() {
    //
    //   _|  |                                   |     _)           | 
    //  |    |   _` |  __ \   __ \   |   |       __ \   |   __|  _` | 
    //  __|  |  (   |  |   |  |   |  |   |       |   |  |  |    (   | 
    // _|   _| \__,_|  .__/   .__/  \__, |      _.__/  _| _|   \__,_| 
    //                _|     _|     ____/                             
    //                                            By KhanhNguyen9872  

    string logo[6] = { \
        "   _|  |                                   |     _)           | ", \
        "  |    |   _` |  __ \\   __ \\   |   |       __ \\   |   __|  _` | ", \
        "  __|  |  (   |  |   |  |   |  |   |       |   |  |  |    (   | ", \
        " _|   _| \\__,_|  .__/   .__/  \\__, |      _.__/  _| _|   \\__,_| ", \
        "                _|     _|     ____/                             ", \
        "                                             By KhanhNguyen9872  "};
    string text = "";
    for(int i=0; i<=terminalRows; i++) {
        if(i == (terminalRows/2) - ((sizeof(logo) / sizeof(logo[0])) / 2)) {
            text = text + centerText(logo, sizeof(logo) / sizeof(logo[0]));
            break;
        } else {
            text = text + "\n";
        }
    }

    bool _i = false;
    clearTerminal();
    for(int i=0;i<40; i++) {
        if (i<5) {
            color(BLACK);
        } else if(i<8) {
            color(DARKGRAY);
        } else if (i<10) {
            color(LIGHTGRAY);
        } else if (i>36) {
            color(DARKGRAY);
        } else if (i>38) {
            color(LIGHTGRAY);
        } else {
            if(!_i) {
                color(WHITE);
                _i = true;
            }
        }
        clearTerminal();
        cout << text;
        Sleep(100);
    }
    clearTerminal();
    return;
}

void resizeTerminal(int column, int row) {
    string cmd = "MODE " + to_string(column) + "," + to_string(row);
    system(cmd.c_str());
    return;
}

void titleTerminal(string name) {
    string title = "title " + name;
    system(title.c_str());
    return;
}

void loadingFrame(int progress) {
    //
    //   __________
    //  |==        |
    //   ``````````
    //   LOADING...
    //

    string text = "";
    string lineSpace = "";
    string tmp = "";

    for(int i=0; i<terminalRows-4; i++) {
        if(i == terminalRows-5) {
            for(int j=0; j<terminalColumns; j++) {
                if(j == terminalColumns-15) {
                    tmp = tmp + lineSpace + " __________ \n";
                    tmp = tmp + lineSpace + "|";
                    for(int k=10; k<=100; k=k+10) {
                        if(progress >= k) {
                            tmp = tmp + "=";
                        } else {
                            tmp = tmp + " ";
                        }
                    }
                    tmp = tmp + "|\n";
                    tmp = tmp + lineSpace + " `````````` \n";
                    break;
                } else {
                    lineSpace = lineSpace + " ";
                }
            }
            text = text + tmp + lineSpace + " LOADING";
            for(int k=0; k<tmp_count; k++) {
                text = text + ".";
            }
            text = text + "\n";
            tmp_count = tmp_count + 1;
            if(tmp_count > 4) {
                tmp_count = 0;
            }
        } else {
            text = text + "\n";
        }
    }
    clearTerminal();
    cout << text;
    return;
}

string menuText(string text[], int size, int choose) {
    //      
    // =>> | New game | <<=
    //     | Settings |
    //     |  Credit  |
    //     |   Exit   |
    //

    string finalString = "";
    string lineSpace = "";

    for(int i=0; i < ((terminalColumns - text[0].length()) / 2) - 6; i++) {
        lineSpace = lineSpace + " ";
    }
    
    for(int i=0; i<size; i++) {
        if(i == choose) {
            finalString = finalString + lineSpace + "==> | " + text[i] + " | <==" + "\n";
        } else {
            finalString = finalString + lineSpace + "    | " + text[i] + " |    " + "\n";
        }
    };
    return finalString;
}

void playSound(string file) {
    if (sound_active) {
        PlaySound(TEXT(file.c_str()), NULL, SND_FILENAME|SND_LOOP|SND_ASYNC);
    }
    return;
}

void stopSound() {
    PlaySound(NULL, 0, 0);
    return;
}

void lockSizeTerminal() {
    int columns, rows;
    while(true) {
        getTerminalSize(columns, rows);
        if ((columns != terminalColumns) || (rows != terminalRows)) {
            resizeTerminal(terminalColumns, terminalRows);
        }
        Sleep(1000);
    }
}

void showMenu(string titleMenu, string* menu, int sizeMenu, int *chooseMenu) {
    //   ___ _                       ___ _        _ 
    //  | __| |__ _ _ __ _ __ _  _  | _ |_)_ _ __| |
    //  | _|| / _` | '_ \ '_ \ || | | _ \ | '_/ _` |
    //  |_| |_\__,_| .__/ .__/\_, | |___/_|_| \__,_|
    //             |_|  |_|   |__/                  
    //

    if(smallLogo == "") {
        string logo[6] = {
            " ___ _                       ___ _        _ ", \
            "| __| |__ _ _ __ _ __ _  _  | _ |_)_ _ __| |", \
            "| _|| / _` | '_ \\ '_ \\ || | | _ \\ | '_/ _` |", \
            "|_| |_\\__,_| .__/ .__/\\_, | |___/_|_| \\__,_|", \
            "           |_|  |_|   |__/                  ", \
            "" \
        };

        smallLogo = centerText(logo, sizeof(logo)/sizeof(logo[0]));
    }
    
    string text = "";
    int titleMenuSize = 0;

    if(titleMenu != "") {
        string p[1] = {
            titleMenu
        };
        titleMenu = centerText(p, sizeof(p)/sizeof(p[0]));
        titleMenuSize = 1;
    };

    int padding = terminalRows - 7 - sizeMenu - titleMenuSize;

    for(int i=0; i <= padding; i++) {
        if(i == 3) {
            text = text + smallLogo + titleMenu + "\n" + menuText(menu, sizeMenu, *chooseMenu);
        } else if (i == padding - 1) {
            for(int j=0; j < terminalColumns; j++) {
                text = text + "_";
            };
            text = text + "\n";
            break;
        } else {
            text = text + "\n";
        }
    }

    color(WHITE);
    clearTerminal();
    cout << text;
    color(GREEN);
    cout << "| 'W' -> UP | 'S' -> DOWN | 'ENTER/SPACE' -> ENTER |";
    return;
}

void credit() {
    string credit_info[12] = {
        "", \
        "FLAPPY BIRD", \
        "", \
        "Admin: Nguyen Van Khanh", \
        "", \
        "Design: Nguyen Van Khanh", \
        "", \
        "Music: Nguyen Van Khanh", \
        "", \
        "", \
        "KhanhNguyen9872. All rights reversed!", \
        ""
    };

    int count = 0;
    int line = sizeof(credit_info) / sizeof(credit_info[0]);
    string text, tmp;

    clearTerminal();
    Sleep(350);
    color(YELLOW);
    
    while(true) {
        text = "";
        for(int i=0; i<terminalRows - count; i++) {
                if (i == terminalRows - 1 - count) {
                    if (count - 1 < line) {
                        tmp = "";
                        for(int k=0; k<count; k++) {
                            for(int j=0; j < (terminalColumns - credit_info[k].length()) / 2; j++) {
                                tmp = tmp + " ";
                            }
                            tmp = tmp + credit_info[k] + "\n";
                        };
                    }
                    text = text + tmp;
                    count = count + 1;
                    
                    break;
                } else {
                    text = text + "\n";
                }
        };
        if (terminalRows - count - 1 == 0) {
            color(MAGENTA);
            cout << "\n\n";
            tmp = ">> Press any key to exit <<";
            for(int i=0; i < (terminalColumns - tmp.length()) / 2; i++) {
                cout << " ";
            }
            cout << tmp;
            _getch();
            return;
        }
        clearTerminal();
        cout << text;
        Sleep(250);
    };
    return;
}

void settingsMenu() {
    string menu[2] = {
        "",
        "      Back       "
    };
    int choose = 0;
    while(true) {
        if (choose == -1) {
            Sleep(25);
            return;
        } else if (choose == 0) {
            if (sound_active) {
                menu[0] =  "    Sound [x]    ";
            } else {
                menu[0] =  "    Sound [ ]    ";
            };
        };
        showMenu("Settings", menu, sizeof(menu)/sizeof(menu[0]), &choose);
        inputMenu(&choose, 1, 1);
        Sleep(100);
    };
}

void inputMenu(int *chooseMenu, int max, int type_menu) {
    if (_kbhit()) {
        char p = _getch();
        if ((p == 'w') || (p == 'W')) {
            switch (type_menu) {
                case 0:
                case 1:
                    if(*chooseMenu > 0) {
                        *chooseMenu = *chooseMenu - 1;
                    } else {
                        *chooseMenu = max;
                    };
                    break;
            };
        } else if ((p == 's') || (p == 'S')) {
            switch (type_menu) {
                case 0:
                case 1:
                    if(*chooseMenu < max) {
                        *chooseMenu = *chooseMenu + 1;
                    } else {
                        *chooseMenu = 0;
                    };
                    break;
            };
        } else if ((p == '\r') || (p == ' ')) {
            switch(type_menu) {
                case 0:
                    if(*chooseMenu == 0) {
                        clearTerminal();
                        cout << "Start game!";
                        cin.get();
                    } else if (*chooseMenu == 1) {
                        settingsMenu();
                    } else if (*chooseMenu == 2) {
                        credit();
                    } else if (*chooseMenu == 3) {
                        exit(0);
                    };
                    break;
                case 1:
                    if(*chooseMenu == 0) {
                        sound_active = !sound_active;
                        if (sound_active) {
                            playSound("sound\\mainmenu.wav");
                        } else {
                            stopSound();
                        };
                    } else if (*chooseMenu == 1) {
                        *chooseMenu = -1;
                    };
                    break;
            };
        };
    };
    return;
}

void mainMenu() {
    string menu[4] = {
        "New game",
        "Settings",
        " Credit ",
        "  Exit  "
    };
    int chooseMenu = 0;
    while(true) {
        showMenu("", menu, sizeof(menu)/sizeof(menu[0]), &chooseMenu);
        inputMenu(&chooseMenu, 3, 0);
        Sleep(100);
    };
}

int main() {
    terminalColumns = 80;
    terminalRows = 20;

    if((terminalColumns % 2 != 0) || (terminalRows % 2 != 0)) {
        cout << "ERROR: Columns and rows must be divisible by 2.\n";
        return 1;
    }

    thread lockSizeTer(lockSizeTerminal);

    hideCursor();
    titleTerminal("Flappy Bird - KhanhNguyen9872 - C++");

    clearTerminal();
    Sleep(500);
    banner();

    Sleep(500);
    // Loading time... [cho đẹp thôi chứ k có load gì đâu =)))]
    color(LIGHTCYAN);
    for(int i=0; i<=100; i++) {
        loadingFrame(i);
        if(i<20) {
            Sleep(100);
        } else if(i<70) {
            Sleep(20);
        } else if (i<90) {
            Sleep(30);
        } else {
            Sleep(60);
        }        
    };

    Sleep(500);
    clearTerminal();
    Sleep(500);
    
    if(sound_active) {
        playSound("sound\\mainmenu.wav");
    }
    
    mainMenu();

    string p[2] = {
        "Game paused!",
        ""
    };
    cout << centerText(p, sizeof(p)/sizeof(p[0]));
    cin.get();
    return 0;
}
