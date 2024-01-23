#include <iostream>
#include <windows.h>
#include <string>
#include <conio.h>
#include <thread>
#include <time.h>

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

int settingsData[3] = {1, 1, WHITE};
int terminalColumns, terminalRows;
int tmp_count = 0;
string smallLogo = "";

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
COORD newPos = {0, 0};
DWORD written;

void inputMenu(int *chooseMenu, int max, int type_menu);
void flappyBird();

void cursorPos_move(int column, int row) {
    COORD tmpPos;
    tmpPos.X = column;
    tmpPos.Y = row;
    SetConsoleCursorPosition(hConsole, tmpPos);
}

void cursorPos_up() {
    SetConsoleCursorPosition(hConsole, newPos);
}

void clearTerminal() {
    cursorPos_up();

    GetConsoleScreenBufferInfo(hConsole, &bufferInfo);

    COORD size = {bufferInfo.srWindow.Right - bufferInfo.srWindow.Left + 1, bufferInfo.srWindow.Bottom - bufferInfo.srWindow.Top + 1};

    FillConsoleOutputCharacter(hConsole, ' ', size.X * size.Y, newPos, &written);

    cursorPos_up();
    return;
}

void hideCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;

    cursorInfo.dwSize = 1;
    cursorInfo.bVisible = FALSE;

    SetConsoleCursorInfo(hConsole, &cursorInfo);
    return;
}

void utf8Output() {
    SetConsoleOutputCP(CP_UTF8);

    setvbuf(stdout, nullptr, _IOFBF, 1000);
    return;
}

void getTerminalSize(int *columns, int *rows) {
    *columns = bufferInfo.srWindow.Right - bufferInfo.srWindow.Left + 1;
    *rows = bufferInfo.srWindow.Bottom - bufferInfo.srWindow.Top + 1;
    return;
}

void color(int index) {
    SetConsoleTextAttribute(hConsole, index);
    return;
}

void playSound_thread(string file) {
    PlaySound(TEXT(file.c_str()), NULL, SND_FILENAME);
    return;
}

// void playSound_SFX(string file) {
//     if(settingsData[1]) {
        
//     };
//     return;
// }

void playSound_main(string file) {
    if(settingsData[0]) {
        PlaySound(TEXT(file.c_str()), NULL, SND_FILENAME|SND_LOOP|SND_ASYNC);
    }
    return;
}

void stopSound() {
    PlaySound(NULL, 0, 0);
    return;
}

void errorBox(string output) {
    //
    //  ________________________ 
    // |/                      \|
    // |     Not available      |
    // |\                      /|
    //  ````````````````````````
    //

    int i, j;
    int boxSize = 26;
    int sizeColumn = 0, sizeRow = 0;
    for(i = 0; i < (terminalColumns - boxSize) / 2; i++) {
        sizeColumn = sizeColumn + 1;
    };
    string text;

    color(RED);
    for(i = 0; i < terminalRows - 5; i++) {
        if (i == (terminalRows / 2) - 3) {
            text = "";
            cursorPos_move(sizeColumn, sizeRow);

            text = text + " ";
            for(j = 0; j < boxSize - 2; j++) {
                text = text + "_";
            };
            
            text = text + " ";
            cout << text;

            text = "";
            cursorPos_move(sizeColumn, sizeRow + 1);

            text = text + "|/";
            for(j = 0; j < boxSize - 4; j++) {
                text = text + " ";
            }; 
            text = text + "\\|";
            cout << text;

            text = "";
            cursorPos_move(sizeColumn, sizeRow + 2);
            text = text + "|";
            for(j = 0; j < boxSize - 2 - (output.length() - 1); j++) {
                if (j == ((boxSize - 2) - output.length() - 1) / 2) {
                    text = text + output;
                } else {
                    text = text + " ";
                };
            };
            text = text + "|";
            cout << text;

            text = "";
            cursorPos_move(sizeColumn, sizeRow + 3);
            text = text + "|\\";
            for(j = 0; j < boxSize - 4; j++) {
                text = text + " ";
            }; 
            text = text + "/|";
            cout << text;

            text = "";
            cursorPos_move(sizeColumn, sizeRow + 4);

            text = text + " ";
            for(j = 0; j < boxSize - 2; j++) {
                text = text + "`";
            };
            
            text = text + " ";
            cout << text;
            break;
        } else {
            sizeRow = sizeRow + 1;
        }
    };

    color(settingsData[2]);
    _getch();
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


void showLogoFullTerminal(string logo[], int sizeLogo) {
    //
    //   _|  |                                   |     _)           | 
    //  |    |   _` |  __ \   __ \   |   |       __ \   |   __|  _` | 
    //  __|  |  (   |  |   |  |   |  |   |       |   |  |  |    (   | 
    // _|   _| \__,_|  .__/   .__/  \__, |      _.__/  _| _|   \__,_| 
    //                _|     _|     ____/                             
    //                                            By KhanhNguyen9872  

    string text = "";
    for(int i=0; i<=terminalRows; i++) {
        if(i == (terminalRows/2) - ((sizeLogo) / 2)) {
            text = text + centerText(logo, sizeLogo);
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
        cursorPos_up();
        cout << text;
        Sleep(100);
    }
    clearTerminal();
    return;
}

void banner() {
    string logo[6] = { \
        "   _|  |                                   |     _)           | ", \
        "  |    |   _` |  __ \\   __ \\   |   |       __ \\   |   __|  _` | ", \
        "  __|  |  (   |  |   |  |   |  |   |       |   |  |  |    (   | ", \
        " _|   _| \\__,_|  .__/   .__/  \\__, |      _.__/  _| _|   \\__,_| ", \
        "                _|     _|     ____/                             ", \
        "                                             By KhanhNguyen9872  "};
    showLogoFullTerminal(logo, sizeof(logo)/sizeof(logo[0]));

    string logo2[15] = { \
        "                 ;                   ", \
        "                 ;;                  ", \
        "                 ;';.                ", \
        "                 ;  ;;               ", \
        "                 ;   ;;              ", \
        "                 ;    ;;             ", \
        "                 ;    ;;             ", \
        "                 ;   ;'              ", \
        "                 ;  '                ", \
        "             ,;;;,;                  ", \
        "             ;;;;;;                  ", \
        "             `;;;;'                  ", \
        "", \
        "", \
        "Use headphones for better experience." \
    };
    showLogoFullTerminal(logo2, sizeof(logo2)/sizeof(logo2[0]));
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

void bottomKeymap(string text) {
    string lineLastTer = "";
    for(int j=0; j < terminalColumns; j++) {
        lineLastTer = lineLastTer + "_";
    };
    lineLastTer = lineLastTer + "\n";
    color(WHITE);
    cout << lineLastTer;
    color(GREEN);
    cout << text;
    return;
}

void showTip(string tip) {
    if (tip.length() >= terminalColumns - 18) {
        return;
    };
    if (tip == "") {
        string listTip[6] = {
            "The person who doesn't play is the winner!",
            "You can change the brightness to be more suitable!",
            "The game was developed by KhanhNguyen9872!",
            "Good luck for you!",
            "How about your day?",
            "Do what you want!"
        };
        
        int p = rand() % (sizeof(listTip) / sizeof(listTip[0]));
        tip = listTip[p];
    };
    int i;
    cursorPos_up();
    color(YELLOW);
    int sizeRow = 0;
    for(i = 0; i < terminalRows; i++) {
        if (i == terminalRows - 3) {
            cursorPos_move(0, sizeRow);
            for(i = 0; i < terminalColumns - 16; i++) {
                cout << " ";
            }
            cursorPos_move(3, sizeRow);
            cout << "Tip: " << tip;
            break;
        } else {
            sizeRow = sizeRow + 1;
        };
    };
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
    int sizeRow = 0;
    int sizeColumn = 0;

    color(LIGHTCYAN);
    for(int i=0; i<terminalRows-4; i++) {
        if(i == terminalRows-5) {
            for(int j=0; j<terminalColumns; j++) {
                if(j == terminalColumns-15) {

                    cursorPos_move(sizeColumn, sizeRow);
                    cout << " __________ ";

                    cursorPos_move(sizeColumn, sizeRow + 1);
                    text = "|";
                    for(int k=10; k<=100; k=k+10) {
                        if(progress >= k) {
                            text = text + "=";
                        } else {
                            text = text + " ";
                        }
                    }
                    text = text + "|";
                    cout << text;

                    cursorPos_move(sizeColumn, sizeRow + 2);
                    cout << " `````````` ";
                    break;
                } else {
                    sizeColumn = sizeColumn + 1;
                }
            }
            cursorPos_move(sizeColumn, sizeRow + 3);
            cout << " LOADING";

            text = "";
            for(int k=0; k<tmp_count; k++) {
                text = text + ".";
            };
            for(int k=0; k< 4 - tmp_count; k++) {
                text = text + " ";
            };
            cout << text + "\n";

            tmp_count = tmp_count + 1;
            if(tmp_count > 4) {
                tmp_count = 0;
            };
        } else {
            sizeRow = sizeRow + 1;
        };
    };
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

void lockSizeTerminal() {
    int columns = terminalColumns, rows = terminalRows;
    while(true) {
        getTerminalSize(&columns, &rows);
        if ((columns != terminalColumns) || (rows != terminalRows)) {
            resizeTerminal(terminalColumns, terminalRows);
        }
        Sleep(500);
    }
    return;
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
    };
    
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
            break;
        } else {
            text = text + "\n";
        }
    }
    
    color(settingsData[2]);
    clearTerminal();
    cout << text;
    bottomKeymap("| 'W' -> UP | 'S' -> DOWN | 'ENTER/SPACE' -> ENTER | 'ESC' -> BACK |");
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

int getBrightness() {
    int currentBrightness = 0;
    if (settingsData[2] == WHITE) {
        currentBrightness = 3;
    } else if (settingsData[2] == LIGHTGRAY) {
        currentBrightness = 2;
    } else if (settingsData[2] == DARKGRAY) {
        currentBrightness = 1;
    }
    return currentBrightness;
}

void setBrightness(int value) {
    if (value == 3) {
        settingsData[2] = WHITE;
    } else if (value == 2) {
        settingsData[2] = LIGHTGRAY;
    } else if (value == 1) {
        settingsData[2] = DARKGRAY;
    };
    return;
}

void kepmappingSettings() {
    errorBox("Not available");
    return;
}

void brightnessSettings() {
    //  
    //  ______________
    // [=========     ]
    //  ``````````````
    //
    int sizeBar = 15;
    int max = 3;

    int currentBrightness = getBrightness();
    
    int i, j;
    string text;
    string lineSpace = "";
    for(j = 0; j < (terminalColumns - sizeBar) / 2; j++) {
        lineSpace = lineSpace + " ";
    };
    string perProcess = "";
    for(i = 0; i < (sizeBar / max); i++) {
        perProcess = perProcess + "=";
    };
    string blankPerProcess = "";
    for(i = 0; i < (sizeBar / max); i++) {
        blankPerProcess = blankPerProcess + " ";
    };
    while(true) {
        if (currentBrightness == -1) {
            return;
        }
        text = "";
        for(i = 0; i < terminalRows - 4; i++) {
            if ((terminalRows / 2) - 2 == i) {
                text = text + lineSpace + " ";
                for(j = 0; j < sizeBar; j++) {
                    text = text + "_";
                };
                text = text + "\n" + lineSpace + "[";

                for(j = 0; j < currentBrightness; j++) {
                    text = text + perProcess;
                };
                for(j = 0; j < max - currentBrightness; j++) {
                    text = text + blankPerProcess;
                };

                text = text + "]" + "\n" + lineSpace + " ";

                for(j = 0; j < sizeBar; j++) {
                    text = text + "`";
                };
                text = text + "\n";
            } else {
                text = text + "\n";
            };
        };
        
        color(settingsData[2]);
        clearTerminal();
        cout << text;
        bottomKeymap("| 'A' -> LOW | 'D' -> HIGH | 'ENTER/SPACE/ESC' -> BACK |");
        inputMenu(&currentBrightness, 2, 2);
        Sleep(50);
    };
    return;
}

void setResolution(int value) {
    if(value == 0) {
        terminalColumns = 80;
        terminalRows = 20;
    } else if (value == 1) {
        terminalColumns = 100;
        terminalRows = 26;
    } else if (value == 2) {
        terminalColumns = 120;
        terminalRows = 30;
    } else if (value == 3) {
        terminalColumns = 140;
        terminalRows = 36;
    } else if (value == 4) {
        terminalColumns = 160;
        terminalRows = 40;
    };
    smallLogo = "";
    return;
}

void resolutionSettings() {
    string menu[5] = {
        " 80 x 20",
        "100 x 26",
        "120 x 30",
        "140 x 36",
        "160 x 40"
    };
    int sizeMenu = sizeof(menu) / sizeof(menu[0]);
    int choose = 0;
    string titleMenu;
    while(true) {
        if (choose == -1) {
            Sleep(25);
            return;
        };
        titleMenu = "| Current resolution: " + to_string(terminalColumns) + " x " + to_string(terminalRows) + " |";
        showMenu(titleMenu, menu, sizeMenu, &choose);
        inputMenu(&choose, sizeMenu, 3);
        Sleep(100);
    };
    return;
}

void settingsMenu() {
    string menu[6] = {
        "",
        "",
        "    Brightness   ",
        "    Keymapping   ",
        "    Resolution   ",
        "    Back         "
    };
    int sizeMenu = sizeof(menu)/sizeof(menu[0]);
    int choose = 0;
    while(true) {
        if (choose == -1) {
            Sleep(25);
            return;
        };

        if (settingsData[0]) {
            menu[0] =  "    Music [x]    ";
        } else {
            menu[0] =  "    Music [ ]    ";
        };
        
        if (settingsData[1]) {
            menu[1] =  "    SFX [x]      ";
        } else {
            menu[1] =  "    SFX [ ]      ";
        };

        showMenu("| Settings |", menu, sizeMenu, &choose);
        inputMenu(&choose, sizeMenu - 1, 1);
        Sleep(100);
    };
}

void flushInput() {
    while (_kbhit()) {
        _getch();
    };
    return;
}

void inputMenu(int *chooseMenu, int max, int type_menu) {
    if (_kbhit()) {
        char p = _getch();

        if ((p == 'w') || (p == 'W')) /* W */ {
            switch (type_menu) {
                case 0:
                case 1:
                case 3:
                    if(*chooseMenu > 0) {
                        *chooseMenu = *chooseMenu - 1;
                    } else {
                        *chooseMenu = max;
                    };
                    break;
            };
        } else if ((p == 'a') || (p == 'A')) /* A */ {
            switch (type_menu) {
                case 2:
                    if(*chooseMenu > 1) {
                        *chooseMenu = *chooseMenu - 1;
                        setBrightness(*chooseMenu);
                    };
                    break;
            };
        } else if ((p == 'd') || (p == 'D')) /* D */ {
            switch (type_menu) {
                case 2:
                    if(*chooseMenu <= max) {
                        *chooseMenu = *chooseMenu + 1;
                        setBrightness(*chooseMenu);
                    };
                    break;
            };
        } else if ((p == 's') || (p == 'S')) /* S */ {
            switch (type_menu) {
                case 0:
                case 1:
                case 3:
                    if(*chooseMenu < max) {
                        *chooseMenu = *chooseMenu + 1;
                    } else {
                        *chooseMenu = 0;
                    };
                    break;
            };
        } else if ((p == '\r') || (p == ' ')) /* ENTER/SPACE */ {
            switch(type_menu) {
                case -1:
                    showTip("");
                    break;
                case 0:
                    if(*chooseMenu == 0) {
                        flappyBird();
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
                        settingsData[0] = !settingsData[0];
                        playSound_main("sound\\mainmenu.wav");
                        if (!settingsData[0]) {
                            stopSound();
                        };
                    } else if (*chooseMenu == 1) {
                        settingsData[1] = !settingsData[1];
                    } else if (*chooseMenu == 2) {
                        brightnessSettings();
                    } else if (*chooseMenu == 3) {
                        kepmappingSettings();
                    } else if (*chooseMenu == 4) {
                        resolutionSettings();
                    } else if (*chooseMenu == 5) {
                        *chooseMenu = -1;
                    };
                    break;
                case 2:
                    *chooseMenu = -1;
                case 3:
                    setResolution(*chooseMenu);
            };
        } else if (p == 27) /* ESC */ {
            switch(type_menu) {
                case 1:
                    *chooseMenu = -1;
                case 2:
                    *chooseMenu = -1;
                case 3:
                    *chooseMenu = -1;
            }
        } else {
            return;
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
    int sizeMenu = sizeof(menu)/sizeof(menu[0]);
    int chooseMenu = 0;
    while(true) {
        showMenu("", menu, sizeMenu, &chooseMenu);
        inputMenu(&chooseMenu, sizeMenu - 1, 0);
        Sleep(100);
    };
}

void flappyBird() {
    color(settingsData[2]);
    clearTerminal();
    cout << "Game here!";
    _getch();
    return;
}

int main() {
    system("color 07 >NUL 2>&1");
    srand(time(NULL));

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

    Sleep(1000);
    banner();
    Sleep(1000);

    // Loading time... [cho đẹp thôi chứ k có load gì đâu =)))]
    showTip("");
    flushInput();
    for(int i=0; i<=100; i++) {
        loadingFrame(i);
        inputMenu(&tmp_count, 0, -1);
        if(i<20) {
            Sleep(100);
        } else if(i<70) {
            Sleep(20);
        } else if (i<90) {
            Sleep(30);
        } else {
            Sleep(60);
        };
    };

    Sleep(500);
    clearTerminal();
    Sleep(500);
    
    playSound_main("sound\\mainmenu.wav");
    
    flushInput();
    mainMenu();
    return 0;
}
