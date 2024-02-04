#ifndef _WIN32
    #error "Only supported Windows!"
#endif

#include <iostream>
#include <windows.h>
#include <string>
#include <conio.h>
#include <thread>
#include <time.h>
#include <fstream>
#include <sstream>

#pragma comment(lib, "winmm.lib")

#define configFileName  "flappy.conf"
#define scoreFileName   "score.txt"
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

string version_code = "1.0.0";
char keymapData[7][2] = {
    {0, 119},    // UP       'w'
    {0, 115},    // DOWN     's'
    {0, 97},    // LEFT      'a'
    {0, 100},    // RIGHT    'd'
    {0, 27},     // ESC      
    {0, 32},   // SPACE      ' '
    {0, 13}     // ENTER     '\r'
};

int listHighScore[7];
int sizelistHighScore = sizeof(listHighScore) / sizeof(listHighScore[0]);

int settingsData[10] = {
    1,  // music
    1,  // sfx
    WHITE, // brightness
    0,  // auto mode
    0,  // skin
    1,  // cmd mode v1/v2 (0/1)
    1,  // enable load conf
    1,  // enable color
    1,  // enable sound
    1   // enable resolution
};
int terminalColumns, terminalRows;
int tmp_int[3] = {0, 0, 0};
int listWall[10][3];
int sizelistWall = sizeof(listWall) / sizeof(listWall[0]);
bool isInGame = -1;
bool gameStarted = 0;
bool isBenchmark = 0;

string smallLogo = "";

HWND consoleWindow = GetConsoleWindow();
HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
DWORD dwConsoleMode;

string backGround[5] = {
    // 
    // "                   /`\\             "
    // "  ___             /   \\   /```\\   "
    // " /   \\  __      /     \\_/     \\  "
    // "/     \\/  \\   /                \\_"
    // "            \\_/                    "
    //

    "                   /`\\\\             ",
    "  ___             /   \\\\   /```\\\\   ",
    " /   \\\\  __      /     \\\\_/     \\\\  ",
    "/     \\\\/  \\\\   /                \\\\_",
    "            \\\\_/                    "
};
int sizeBackground = sizeof(backGround) / sizeof(backGround[0]);

string skinFlyAnimation[3][4][3] = {
    {
        {
            "  /",
            ">@@(O>",
            "  \\"
        },
        {
            "  )",
            ">@@(O>",
            "  )"
        },
        {
            "  \\",
            ">@@(O>",
            "  /"
        },
        {
            " __",
            ">@@(O>",
            " ``"
        } 
    },
    {
        {
            " ( O>",
            "\\ @ @/",
            " ^ ^"
        },
        {
            " ( O>",
            "/ @ @\\",
            " ^ ^"
        },
        {
            " ( O>",
            "\\ @ @/",
            " ^ ^"
        },
        {
            " ( O>",
            "/ @ @\\",
            " ^ ^"
        }
    },
    {
        {
            "  ...",
            " | []|",
            "/.....\\"
        },
        {
            "  ...",
            " | []|",
            "/.....\\"
        },
        {
            "  ...",
            " | []|",
            "/.....\\"
        },
        {
            "  ...",
            " | []|",
            "/.....\\"
        }
    }
};

string skinDeadAnimation[3][3] = {
    {
        "",
        ">@@(*>",
        ""
    },
    {
        " ( *>",
        " |@ @|",
        " ^ ^"
    },
    {
        "  ...",
        " | XX|",
        "/.....\\"
    }
};

void inputMenu(int *chooseMenu, int max, int type_menu);
void flappyBird();

void cursorPos_move(int column, int row) {
    COORD tmpPos;
    tmpPos.X = column;
    tmpPos.Y = row;
    SetConsoleCursorPosition(hOutput, tmpPos);
};

void cursorPos_up() {
    cursorPos_move(0, 0);
};

void clearTerminal() {
    int i;
    string line = "";
    string text = "";
    for(i = 0; i < terminalColumns; i++) {
        line = line + " ";
    };
    for(i = 0; i < terminalRows - 1; i++) {
        text = text + line + "\n";
    };
    text = text + line;
    
    cursorPos_up();
    cout << text;
    cursorPos_up();
    return;
};

void hideCursor() {  // Windows API
    CONSOLE_CURSOR_INFO cursorInfo;

    cursorInfo.dwSize = 1;
    cursorInfo.bVisible = FALSE;

    SetConsoleCursorInfo(hOutput, &cursorInfo);
    return;
};

void getTerminalSize(int *columns, int *rows) {  // Windows API
    GetConsoleScreenBufferInfo(hOutput, &bufferInfo);
    *columns = bufferInfo.srWindow.Right - bufferInfo.srWindow.Left + 1;
    *rows = bufferInfo.srWindow.Bottom - bufferInfo.srWindow.Top + 1;
    if(!settingsData[5]) {
        *columns = *columns - 1;
    };
    return;
};

void color(int index) {  // Windows API
    if (settingsData[7]) {
        SetConsoleTextAttribute(hOutput, index);
    };
    return;
};

void playSound_thread(string file) {  // Windows API
    if (settingsData[8]) {
        PlaySound(TEXT(file.c_str()), NULL, SND_FILENAME);
    };
    return;
};

void disableTouch() {  // Windows API
    GetConsoleMode(hInput, &dwConsoleMode);
    dwConsoleMode &= ~ENABLE_PROCESSED_INPUT;
    dwConsoleMode &= ~ENABLE_QUICK_EDIT_MODE;
    SetConsoleMode(hInput, dwConsoleMode);
    return;
};

void disableCloseButton(bool isDisable) {  // WIndows API
    if (isDisable) {
        EnableMenuItem(GetSystemMenu(consoleWindow, FALSE), SC_CLOSE, MF_GRAYED);
    } else {
        EnableMenuItem(GetSystemMenu(consoleWindow, FALSE), SC_CLOSE, MF_ENABLED);
    };
    return;
};

void disableMaximizeButton() {  // Windows API
    SetWindowLong(consoleWindow, GWL_STYLE, GetWindowLong(consoleWindow, GWL_STYLE) & ~WS_MAXIMIZEBOX);
    return;
};

void configureTerminal() {
    system("color 07 >NUL 2>&1"); // default color CMD
    hideCursor();
    disableTouch();
    disableMaximizeButton();
    disableCloseButton(false);
    return;
};

void __getch(int p[]) {
    p[0] = 0;
    p[1] = _getch();
    if((p[1] == 0) || (p[1] == 224)) {
        p[1] = _getch();
        p[0] = 1;
    };
    return;
};

void flushStdin() {
    while (_kbhit()) {
        _getch();
    };
    return;
};

void anyKey() {
    flushStdin();
    _getch();
    flushStdin();
    return;
};

// void playSound_SFX(string file) {
//     if(settingsData[1]) {
        
//     };
//     return;
// }

void playSound_main(string file) {  // Windows API
    if (settingsData[8]) {
        if(settingsData[0]) {
            PlaySound(TEXT(file.c_str()), NULL, SND_FILENAME|SND_LOOP|SND_ASYNC);
        };
    };
    return;
};

void stopSound() {
    if (settingsData[8]) {
        PlaySound(NULL, 0, 0);
    };
    return;
};

string readFile(string fileName) {
    ifstream f(fileName);
    string text = "";
    string line;
    if (f.is_open()) {
        while (getline(f, line)) {
            if (line.empty() || line[0] == '#') {
                continue;
            };
            text = text + line + "\n";
        };
        f.close();
    };
    return text;
};

void writeFile(string fileName, string data) {
    ofstream f(fileName);
    if (f.is_open()) {
        f << data;
        f.close();
    };
};

void writeConfig(string key, string value) {
    if (settingsData[6]) {
        string fileData = readFile(configFileName);
        istringstream file(fileData);
        string line;
        string key_;
        string value_;
        ofstream f(configFileName);
        if (f.is_open()) {
            f << key << "=" << value << "\n";
            while (getline(file, line)) {
                if (line.empty() || line[0] == '#') {
                    continue;
                };
                istringstream iss(line);
                if (getline(iss, key_, '=') && (key_ == key)) {
                    continue;
                } else {
                    getline(iss, value_);
                    f << key_ << "=" << value_ << "\n";
                };
            };
            f.close();
        };
    };
    return;
};

void titleTerminal(string name) {
    SetConsoleTitle((LPCTSTR)name.c_str());
    return;
};

void showBlur() {
    int i, j;
    do {
        i = rand() % 15;
    } while ((i == BLACK) || (i == RED));
    color(i);
    for(i = 0; i < terminalRows; i++) {
        for(j = 0; j < terminalColumns; j = j + 5) {
            cursorPos_move(j, i);
            cout << "-";
        };
    };
    color(settingsData[2]);
    return;
};

void bottomKeymap(string text) {
    string lineLastTer = "";
    int i, j;
    for(j=0; j < terminalColumns; j++) {
        lineLastTer = lineLastTer + "_";
    };
    lineLastTer = lineLastTer + "\n";
    i = text.length();
    for(j = 0; j < terminalColumns - i; j++) {
        text = text + " ";
    };
    cursorPos_move(0, terminalRows - 2);
    color(WHITE);
    cout << lineLastTer;
    color(GREEN);
    cout << text;
    color(settingsData[2]);
    return;
};

void errorBox(string output, string bottom, bool isBlur) {
    //
    //     ________________________   .
    //    /                        \ /
    //   /      Not available       /
    //  / \                        /
    // `   ````````````````````````
    //

    int i, j;
    int boxSize = 26;
    int sizeColumn = 0;
    int sizeRow;

    if(bottom == "") {
        bottom = "Press any key to continue";
    };
    
    for(i = 0; i < (terminalColumns - boxSize) / 2; i++) {
        sizeColumn = sizeColumn + 1;
    };
    string text;

    if (isBlur) {
        showBlur();
    };

    while(true) {
        color(RED);
        sizeRow = 0;
        for(i = 0; i < terminalRows - 5; i++) {
            if (i == (terminalRows / 2) - 3) {
                text = "";
                cursorPos_move(sizeColumn, sizeRow);

                text = text + " ";
                for(j = 0; j < boxSize - 2; j++) {
                    text = text + "_";
                };
                
                text = text + "   /";
                cout << text;

                text = "";
                cursorPos_move(sizeColumn, sizeRow + 1);

                text = text + "/ ";
                for(j = 0; j < boxSize - 4; j++) {
                    text = text + " ";
                }; 
                text = text + " \\ /";
                cout << text;

                text = "";
                cursorPos_move(sizeColumn - 1, sizeRow + 2);
                text = text + "/ ";
                for(j = 0; j < boxSize - 2 - (output.length() ); j++) {
                    if (j == ((boxSize - 2) - output.length() ) / 2) {
                        text = text + " " + output;
                    } else {
                        text = text + " ";
                    };
                };
                text = text + " /";
                cout << text;

                text = "";
                cursorPos_move(sizeColumn - 2, sizeRow + 3);
                text = text + "/ \\ ";
                for(j = 0; j < boxSize - 4; j++) {
                    text = text + " ";
                }; 
                text = text + " /";
                cout << text;

                text = "";
                cursorPos_move(sizeColumn - 3, sizeRow + 4);

                text = text + "/   ";
                for(j = 0; j < boxSize - 2; j++) {
                    text = text + "`";
                };
                
                text = text + " ";
                cout << text;
                break;
            } else {
                sizeRow = sizeRow + 1;
            };
        };

        color(settingsData[2]);
        bottomKeymap(bottom);
        if(_kbhit()) {
            flushStdin();
            return;
        };
        Sleep(250);
    };
    return;
};

void showOverlayResolution() {
    color(settingsData[2]);
    string resolutionString = to_string(terminalColumns) + " x " + to_string(terminalRows);
    string text = "";
    string text2 = "";
    int i, j;
    for(i = 0; i < terminalColumns; i++) {
        text = text + "=";
    };

    text2 = text2 + "| +-";
    for(i = 0; i < resolutionString.length(); i++) {
        text2 = text2 + "-";
    };
    text2 = text2 + "-+\n| | " + resolutionString + " |\n| +-";
    for(i = 0; i < resolutionString.length(); i++) {
        text2 = text2 + "-";
    };
    text2 = text2 + "-+";

    color(settingsData[2]);
    for(i = 0; i < terminalRows; i++) {
        if (i == 0) {
            cursorPos_move(0, i);
            cout << text;
        } else if (i == terminalRows - 1) {
            cursorPos_move(0, i);
            cout << text;
        } else {
            if (i == 1) {
                cursorPos_move(0, i);
                cout << text2;
            };
            cursorPos_move(0, i);
            cout << "|";
            cursorPos_move(terminalColumns - 1, i);
            cout << "|";
        };
    };
    return;
};

void resizeTerminal(short column, short row) {
    string cmd;
    if (settingsData[9]) {
        if(!settingsData[5]) {
            column = column + 1;
        };
        cmd = "MODE " + to_string(column) + "," + to_string(row) + " >NUL 2>&1";
        if (system(cmd.c_str()) != 0) {
            clearTerminal();
            errorBox("API ERROR", "Please run on real Windows!", false);
            exit(1);
        };
        showOverlayResolution();
    };
    if (settingsData[5]) {
        cmd = "Enabled";
    } else {
        cmd = "Disabled";
    };
    titleTerminal("Flappy Bird - KhanhNguyen9872 - C++  |  Res: " + to_string(terminalColumns) + " x " + to_string(terminalRows) + "  |  v2 mode: " + cmd);
    return;
};

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
    } else {
        terminalColumns = 80;
        terminalRows = 20;
        value = 0;
    };
    smallLogo = "";
    writeConfig("resolution", to_string(value));
    resizeTerminal(terminalColumns, terminalRows);
    return;
};

int getResolutionValue() {
    if ((terminalColumns == 80) && (terminalRows == 20)) {
        return 0;
    } else if ((terminalColumns == 100) && (terminalRows == 26)) {
        return 1;
    } else if ((terminalColumns == 120) && (terminalRows == 30)) {
        return 2;
    } else if ((terminalColumns == 140) && (terminalRows == 36)) {
        return 3;
    } else if ((terminalColumns == 160) && (terminalRows == 40)) {
        return 4;
    } else {
        setResolution(0);
        return 0;
    };
};

int readConfig(string key) {
    if (settingsData[6]) {
        string fileData = readFile(configFileName);
        string line;
        string key_;
        string value_ = "0";
        istringstream file(fileData);
        try {
            while (getline(file, line)) {
                if (line.empty() || line[0] == '#') {
                    continue;
                };
                istringstream iss(line);
                if (getline(iss, key_, '=') && (key_ == key)) {
                    if (getline(iss, value_)) {
                        return stoi(value_);
                    } else {
                        writeConfig(key, "-1");
                        return -1;
                    };
                };
            };
            return stoi(value_);
        } catch (...) {
            writeConfig(key, "-1");
            return -1;
        };
    };
    return -1;
};

void showUser(string username) {
    // 
    // +-----------------------+
    // | User: KhanhNguyen9872 |
    // +-----------------------+
    //
    int i;
    int j = 5 * getResolutionValue();
    int sizeColumn = 0;
    int sizeRow = 0;

    string text;

    if (username.length() > 6 + j) {
        text = "";
        for(i = 0; i < 6 + j; i++) {
            if (i >= username.length()) {
                break;
            };
            text = text + username[i];
        };

        if (i >= username.length()) {
            username = text;
        } else {
            username = text + "...";
        };
    };

    username = "User: " + username;

    text = "+-";
    for(i = 0; i < username.length(); i++) {
        text = text + "-";
    };
    text = text + "-+";

    for(i = 0; i < terminalColumns; i++) {
        if (i == terminalColumns - 7 - username.length()) {
            break;
        } else {
            sizeColumn = sizeColumn + 1;
        };
    };

    for(i = 0; i < terminalRows; i++) {
        if (i == 1) {
            cursorPos_move(sizeColumn, sizeRow);
            cout << text;

            cursorPos_move(sizeColumn, sizeRow + 1);
            cout << "| " << username << " |";

            cursorPos_move(sizeColumn, sizeRow + 2);
            cout << text;

            break;
        } else {
            sizeRow = sizeRow + 1;
        };
    };

    return;
};

string centerText(string text[], int size) {
    string finalText = "";
    string lineSpace = "";
    int Tmp, i;

    Tmp = (terminalColumns - text[0].length()) / 2;
    for(i=0; i < Tmp; i++) {
        lineSpace = lineSpace + " ";
    };

    for(i=0; i < size; i++) {
        finalText = finalText + lineSpace + text[i] + "\n";  
    };
    return finalText;
};


void showLogoFullTerminal(string logo[], int sizeLogo, bool isClear, bool isShowUser) {
    //
    //   _|  |                                   |     _)           | 
    //  |    |   _` |  __ \   __ \   |   |       __ \   |   __|  _` | 
    //  __|  |  (   |  |   |  |   |  |   |       |   |  |  |    (   | 
    // _|   _| \__,_|  .__/   .__/  \__, |      _.__/  _| _|   \__,_| 
    //                _|     _|     ____/                             
    //                                            By KhanhNguyen9872  
    int i;
    string text = "";

    for(i=0; i <= terminalRows; i++) {
        if(i == (terminalRows/2) - ((sizeLogo) / 2)) {
            text = text + centerText(logo, sizeLogo);
            break;
        } else {
            text = text + "\n";
        };
    };

    bool _i = false;
    clearTerminal();
    if(isShowUser) {
        color(WHITE);
        showUser(getenv("username"));
    };
    for(i=0;i < 40; i++) {
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
        if (isClear) {
            clearTerminal();
        } else {
            cursorPos_up();
        };
        cout << text;
        Sleep(100);
    };
    clearTerminal();
    return;
};

void showBoxText(string text, bool isBlur) {
    int sizeColumn = 0;
    int sizeRow = 0;
    int i, j;
    string tmp;
    for(i = 0; i < (terminalColumns - text.length()) / 2; i++) {
        sizeColumn = sizeColumn + 1;
    };
    sizeColumn = sizeColumn - 1;

    if (isBlur) {
        showBlur();
    };

    for(i = 0; i < terminalRows; i++) {
        if (i == (terminalRows / 2) - 3) {
            color(YELLOW);
            cursorPos_move(sizeColumn, sizeRow);
            tmp = "  ";
            for(j = 0; j < text.length() + 2; j++) {
                tmp = tmp + "_";
            };
            cout << tmp;

            cursorPos_move(sizeColumn, sizeRow + 1);
            tmp = " | ";
            for(j = 0; j < text.length(); j++) {
                tmp = tmp + " ";
            }
            cout << tmp << " |";

            cursorPos_move(sizeColumn, sizeRow + 2);
            cout << " | " << text << " |";

            cursorPos_move(sizeColumn, sizeRow + 3);
            tmp = " | ";
            for(j = 0; j < text.length(); j++) {
                tmp = tmp + " ";
            }
            cout << tmp << " |";

            cursorPos_move(sizeColumn, sizeRow + 4);
            tmp = "  ";
            for(j = 0; j < text.length() + 2; j++) {
                tmp = tmp + "`";
            };
            cout << tmp;
            return;
        } else {
            sizeRow = sizeRow + 1;
        };
    };
    return;
};

bool showYesorNo(string text) {
    //
    //   ____________________
    //  |                    |
    //  | Exit to main menu? |
    //  |                    |
    //   ````````````````````
    //
    int p[2];
    bool tmp = true;

    flushStdin();
    while(true) {
        showBoxText(text, tmp);
        bottomKeymap("| [y] -> YES | [n] -> NO |");
        if (_kbhit()) {
            __getch(p);

            if (((p[1] == 'y') || (p[1] == 'Y')) && (!p[0])) {
                return 1;
            } else if (((p[1] == 'n') || (p[1] == 'N')) && (!p[0])) {
                return 0;
            };
        };
        tmp = false;
        Sleep(250);
    };

    return 0;
};

void banner() {
    string logo[6] = { \
        "   _|  |                                   |     _)           | ", \
        "  |    |   _` |  __ \\   __ \\   |   |       __ \\   |   __|  _` | ", \
        "  __|  |  (   |  |   |  |   |  |   |       |   |  |  |    (   | ", \
        " _|   _| \\__,_|  .__/   .__/  \\__, |      _.__/  _| _|   \\__,_| ", \
        "                _|     _|     ____/                             ", \
        "                                             By KhanhNguyen9872  "};
    showLogoFullTerminal(logo, sizeof(logo)/sizeof(logo[0]), true, false);

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
        "               ``                    ", \
        "                                     ", \
        "Use headphones for better experience." \
    };
    showLogoFullTerminal(logo2, sizeof(logo2)/sizeof(logo2[0]), false, true);
    return;
};

void showTip(string tip) {
    if (tip.length() >= terminalColumns - 18) {
        return;
    };
    if (tip == "") {
        string listTip[6] = {
            "Fact: The person who doesn't play is the winner!",
            "Tip: You can change the brightness to be more suitable!",
            "Fact: The game was developed by KhanhNguyen9872!",
            "Tip: Reduce resolution for best performance!",
            "Tip: You can change the key according to your preference!",
            "Tip: Try Auto mode for fun!"
        };
        
        int p = rand() % (sizeof(listTip) / sizeof(listTip[0]));
        tip = listTip[p];
    };
    int i;
    string text = "";
    for(i = 0; i < terminalColumns - 16; i++) {
        text = text + " ";
    };

    cursorPos_up();
    color(YELLOW);
    int sizeRow = 0;
    for(i = 0; i < terminalRows; i++) {
        if (i == terminalRows - 3) {
            cursorPos_move(0, sizeRow);
            cout << text;
            cursorPos_move(3, sizeRow);
            cout << tip;
            break;
        } else {
            sizeRow = sizeRow + 1;
        };
    };
    return;
};

void showAnimation(string output[], string animation[], int sizeAnimation, int countUp) {
    bool noSpace;
    int i, j, m;
    int k = (terminalRows / 4) - countUp;
    int l = terminalRows - 3;
    int sizeRow = 0;
    for(i = 0; i < l; i++) {
        if (i == k) {
            for(j = 0; j < sizeAnimation; j++) {
                if (output == NULL) { // print direct to terminal
                    cursorPos_move(3, sizeRow + j);
                    cout << animation[j];
                } else { // insert data to output array
                    noSpace = 0;
                    for(m = 0; m < animation[j].length(); m++) {
                        if (!noSpace) { // remove space char in first string
                            if (animation[j][m] == ' ') {
                                continue;
                            } else {
                                noSpace = 1;
                            };
                        };
                        output[sizeRow + j][3 + m] = animation[j][m];
                    };
                };
            };
            break;
        } else {
            sizeRow = sizeRow + 1;
        };
    };
    return;
};

void showChangeScene() {
    int i;
    string text = "";
    string text2 = "";
    string listText[10] = {
        "=",
        "/",
        "-",
        "@",
        "+",
        "\\",
        "|",
        "#",
        "%",
        "*"
    };
    string p = listText[rand() % (sizeof(listText) / sizeof(listText[0]))];

    for(i = 0; i < terminalColumns; i++) {
        text = text + p;
        text2 = text2 + " ";
    };

    bool randomVar = rand() % 1;

    int k = 5 - getResolutionValue();
    
    color(settingsData[2]);
    Sleep(400);
    if (randomVar) {
        cursorPos_up();
        for(i = 0; i < terminalRows; i++) {
            cout << text;
            Sleep(k);
        };

        Sleep(150);
        cursorPos_up();
        for(i = 0; i < terminalRows; i++) {
            cout << text2;
            Sleep(k);
        };
    } else {
        for(i = terminalRows - 1; i >= 0; i--) {
            cursorPos_move(0, i);
            cout << text;
            Sleep(k);
        };

        Sleep(150);
        for(i = terminalRows - 1; i >= 0; i--) {
            cursorPos_move(0, i);
            cout << text2;
            Sleep(k);
        };
    };

    Sleep(250);
    return;
};

bool checkTerminalActive() {  // Windows API
    return consoleWindow == GetForegroundWindow();
};

void exitProgram() {
    if (showYesorNo("Do you want to exit?")) {
        showChangeScene();
        Sleep(500);
        exit(0);
    };
    return;
};

string getRoad() {
    int i;
    string text = "[";
    for(i = 0; i < terminalColumns - 2; i++) {
        text = text + "/";
    };
    text = text + "]";
    return text;
};

string getOutput(string output[], int sizeOutput) {
    int i;
    int k = terminalRows - 2;
    string fullOutput = "";
    for(i = 0; i < sizeOutput; i++) {
        fullOutput = fullOutput + output[i];
        if (i != sizeOutput - 1) {
            fullOutput = fullOutput + "\n";
        };
    };
    return fullOutput;
};

void wipeOutput(string output[], int sizeOutput) {
    string lineBlank = "";
    int i;
    for(i = 0; i < terminalColumns; i++) {
        lineBlank = lineBlank + " ";
    };
    for(i = 0; i < sizeOutput; i++) {
        output[i] = lineBlank;
    };
    return;
};

void loadingFrame(int progress, bool isShowBird) {
    //
    //   __________
    //  |==        |
    //   ``````````
    //   LOADING...
    //

    // 
    // 
    //   ( O>   |    \         | 
    //  / @ @\  |   >@@( O>    | 
    //   ^ ^    |    /         | 
    //
    //
    //
    // [/////////////////////////////////////////]

    int sizeAnimation = sizeof(skinFlyAnimation[settingsData[4]]) / sizeof(skinFlyAnimation[settingsData[4]][0]);
    int i, j, k;

    string text = "";
    int sizeRow = 0;
    int sizeColumn = 0;

    color(LIGHTCYAN);
    for(i = 0; i < terminalRows - 4; i++) {
        if(i == terminalRows-5) {
            for(j = 0; j < terminalColumns; j++) {
                if(j == terminalColumns - 15) {

                    cursorPos_move(sizeColumn, sizeRow);
                    cout << " __________ ";

                    cursorPos_move(sizeColumn, sizeRow + 1);
                    text = "|";
                    for(k = 10; k <= 100; k = k + 10) {
                        if(progress >= k) {
                            text = text + "=";
                        } else {
                            text = text + " ";
                        };
                    };
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
            for(k=0; k < tmp_int[0]; k++) {
                text = text + ".";
            };
            for(k=0; k < 4 - tmp_int[0]; k++) {
                text = text + " ";
            };
            cout << text + "\n";

            tmp_int[0] = tmp_int[0] + 1;
            if(tmp_int[0] > 4) {
                tmp_int[0] = 0;
            };
        } else {
            sizeRow = sizeRow + 1;
        };
    };

    if(isShowBird) {
        color(CYAN);
        text = getRoad();
        showAnimation(NULL, skinFlyAnimation[settingsData[4]][tmp_int[1]], sizeof(skinFlyAnimation[settingsData[4]][tmp_int[1]]) / sizeof(skinFlyAnimation[settingsData[4]][tmp_int[1]][0]), 0);
    
        cursorPos_move(0, terminalRows - 6);
        cout << text;

        if ((tmp_int[1] >= 3) || (tmp_int[1] <= 0) ) {
            tmp_int[2] = !tmp_int[2];
        };

        if(!tmp_int[2]) {
            tmp_int[1] = tmp_int[1] + 1;
        } else {
            tmp_int[1] = tmp_int[1] - 1;
        };
    };

    if (progress >= 100) {
        showChangeScene();
    };
    
    return;
};

string menuText(string text[], int size, int choose) {
    //      
    // =>> |   Start  | <<=
    //     | Settings |
    //     |  Credit  |
    //     |   Exit   |
    //

    string finalString = "";
    string lineSpace = "";
    int i;

    for(i = 0; i < ((terminalColumns - text[0].length()) / 2) - 6; i++) {
        lineSpace = lineSpace + " ";
    };
    
    for(i = 0; i < size; i++) {
        if(i == choose) {
            finalString = finalString + lineSpace + "==> | " + text[i] + " | <==" + "\n";
        } else {
            finalString = finalString + lineSpace + "    | " + text[i] + " |    " + "\n";
        };
    };
    return finalString;
};

void lockSizeTerminal() {
    if (!settingsData[9]) {
        return;
    };
    int columns = 0, rows = 0;
    while(true) {
        getTerminalSize(&columns, &rows);
        if ((columns != terminalColumns) || (rows != terminalRows)) {
            resizeTerminal(terminalColumns, terminalRows);
            disableMaximizeButton();
        };
        Sleep(500);
    };
    return;
};

string getNameKey(short value, bool isTwoChar) {
    if (isTwoChar) {
        switch(value) {
            case 59:
            case 60:
            case 61:
            case 62:
            case 63:
            case 64:
            case 65:
            case 66:
            case 67:
            case 68:
                return "F" + to_string(value - 58);
            case 71:
                return "HOME";
            case 79:
                return "END";
            case 82:
                return "INSER";
            case 83:
                return "DEL";
            case 73:
                return "PG UP";
            case 81:
                return "PG DN";
            case 72:
                return "UP";
            case 80:
                return "DOWN";
            case 75:
                return "LEFT";
            case 77:
                return "RIGHT";
            default:
                return "NULL";
        };
    } else {
        switch(value) {
            case 8:
                return "BACKS";
            case 9:
                return "TAB";
            case 13:
                return "ENTER";
            case 27:
                return "ESC";
            case 32:
                return "SPACE";
            default:
                break;
        };
    };
    return string(1, value);
};

bool setKeymap(int value, int key, bool isTwoChar) {
    int keyAllow[5] = {
        8, 9, 13, 27, 32
    };
    int i;
    if(isTwoChar) {
        if (getNameKey(key, isTwoChar) != "NULL") {
            keymapData[value][0] = 1;
            keymapData[value][1] = key;
            return 1;
        };
    } else if (key > 7) {
        for(i = 0; i < sizeof(keyAllow) / sizeof(keyAllow[0]); i++) {
            if (key == keyAllow[i]) {
                keymapData[value][0] = 0;
                keymapData[value][1] = key;
                return 1;
            };
        };
        if ((key >= ' ') && (key <= '~') ) {
            keymapData[value][0] = 0;
            keymapData[value][1] = key;
            return 1;
        };
    };
    return 0;
};

void showMenu(string titleMenu, string* menu, int sizeMenu, int *chooseMenu) {
    //   ___ _                       ___ _        _ 
    //  | __| |__ _ _ __ _ __ _  _  | _ |_)_ _ __| |
    //  | _|| / _` | '_ \ '_ \ || | | _ \ | '_/ _` |
    //  |_| |_\__,_| .__/ .__/\_, | |___/_|_| \__,_|
    //             |_|  |_|   |__/                  
    //
    string text = "";
    int i;

    if(smallLogo == "") {
        string logo[6] = {
            " ___ _                       ___ _        _ ", \
            "| __| |__ _ _ __ _ __ _  _  | _ |_)_ _ __| |", \
            "| _|| / _` | '_ \\ '_ \\ || | | _ \\ | '_/ _` |", \
            "|_| |_\\__,_| .__/ .__/\\_, | |___/_|_| \\__,_|", \
            "           |_|  |_|   |__/                  ", \
            "" \
        };
        for(i = 0; i < (logo[4].length() - version_code.length() - 1) - 2; i++) {
            text = text + logo[4][i];
        };
        logo[4] = text + "v" + version_code;
        smallLogo = centerText(logo, sizeof(logo)/sizeof(logo[0]));
    };

    text = "";
    int titleMenuSize = 0;

    if(titleMenu != "") {
        string p[1] = {
            titleMenu
        };
        titleMenu = centerText(p, sizeof(p)/sizeof(p[0]));
        titleMenuSize = 1;
    };

    int padding = terminalRows - 7 - sizeMenu - titleMenuSize;

    for(i=0; i <= padding; i++) {
        if(i == 3) {
            text = text + smallLogo + titleMenu + "\n" + menuText(menu, sizeMenu, *chooseMenu);
        } else if (i == padding - 1) {
            break;
        } else {
            text = text + "\n";
        };
    };
    
    color(settingsData[2]);
    clearTerminal();
    cout << text;
    bottomKeymap("| [" + getNameKey(keymapData[0][1], keymapData[0][0]) + "] -> UP | [" + getNameKey(keymapData[1][1], keymapData[1][0]) + "] -> DOWN | [" + getNameKey(keymapData[6][1], keymapData[6][0]) + "] -> ENTER | [" + getNameKey(keymapData[4][1], keymapData[4][0]) + "] -> BACK |");
    return;
};

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
    int i, j, k;
    int line = sizeof(credit_info) / sizeof(credit_info[0]);
    string text, tmp;

    clearTerminal();
    Sleep(350);
    color(YELLOW);
    
    while(true) {
        text = "";
        for(i=0; i<terminalRows - count; i++) {
            if (i == terminalRows - 1 - count) {
                if (count - 1 < line) {
                    tmp = "";
                    for(k=0; k<count; k++) {
                        for(j=0; j < (terminalColumns - credit_info[k].length()) / 2; j++) {
                            tmp = tmp + " ";
                        };
                         tmp = tmp + credit_info[k] + "\n";
                    };
                };
                text = text + tmp;
                count = count + 1;
                
                break;
            } else {
                text = text + "\n";
            };
        };
        if (terminalRows - count - 1 == 0) {
            color(MAGENTA);
            cout << "\n\n";
            tmp = ">> Press any key to exit <<";
            for(i=0; i < (terminalColumns - tmp.length()) / 2; i++) {
                cout << " ";
            };
            cout << tmp;
            anyKey();
            return;
        };
        clearTerminal();
        cout << text;
        Sleep(250);
    };
    return;
};

void setBrightness(int value) {
    if (value == 3) {
        settingsData[2] = WHITE;
    } else if (value == 2) {
        settingsData[2] = LIGHTGRAY;
    } else if (value == 1) {
        settingsData[2] = DARKGRAY;
    } else {
        value = 3;
        settingsData[2] = WHITE;
    };
    writeConfig("brightness", to_string(value));
    return;
};

int getBrightness() {
    if (settingsData[2] == WHITE) {
        return 3;
    } else if (settingsData[2] == LIGHTGRAY) {
        return 2;
    } else if (settingsData[2] == DARKGRAY) {
        return 1;
    } else {
        setBrightness(3);
        return 3;
    };
};

void changeKeymapping(int value) {
    //
    //  _______________
    // |               |
    // | INPUT NEW KEY |
    // |               |
    //  ```````````````
    //
    int i, j = 1;
    string text;
    string lineSpace = "";
    int p[2];
    showBoxText("INPUT NEW KEY", false);
    bottomKeymap("");
    flushStdin();
    __getch(p);
    for(i = 0; i < sizeof(keymapData) / sizeof(keymapData[0]); i++) {
        if((p[0] == keymapData[i][0]) && (p[1] == keymapData[i][1])) {
            j = 0;
            break;
        };
    };
    if(j) {
        if (setKeymap(value, p[1], p[0])) {
            writeConfig("key" + to_string(value), to_string(500 + p[1]));
        } else {
            flushStdin();
            errorBox("Key unavailable", "", true);
        };
    } else {
        if((p[0] == keymapData[value][0]) && (p[1] == keymapData[value][1])) {
            return;
        } else {
            flushStdin();
            errorBox("Key already set", "", true);
        };
    };
    flushStdin();
    return;
};

void keymappingSettings() {
    int i, j, k;
    int choose = 0;
    string text;
    string template_menu[7] = {
        "UP       ",
        "DOWN     ",
        "LEFT     ",
        "RIGHT    ",
        "EXIT/BACK",
        "JUMP     ",
        "ENTER    "
    };
    int sizeMenu = sizeof(template_menu) / sizeof(template_menu[0]);
    string *menu = new string[sizeMenu];
    clearTerminal();
    while(true) {
        if(choose == -1) {
            return;
        };
        
        for(i = 0; i < sizeMenu; i++) {
            k = keymapData[i][1];
            if (keymapData[i][0]) {
                k = k + 500;
            };
            text = "(" + to_string(k) + ")";
            k = text.length();
            for(j = 0; j < 5 - k; k++) {
                text = text + " ";
            };
            text = text + " '" + getNameKey(keymapData[i][1], keymapData[i][0]) + "'";
            k = text.length();
            for(j = 0; j < 13 - k; j++) {
                text = text + " ";
            };
            
            menu[i] = text + "  -->  " + template_menu[i];
        };
        showMenu("| Keymapping Settings |", menu, sizeMenu, &choose);
        inputMenu(&choose, sizeMenu - 1, -2);
        Sleep(100);
    };
    return;
};

void stringToOutput(string str, string output[], int sizeOutput) {
    int size = str.length();
    int row = 0;
    int i; 
    int column = 0;
    for(i = 0; i < size; i++) {
        if (row >= sizeOutput) {
            return;
        };
        if (str[i] == '\n') {
            column = 0;
            row = row + 1;
            continue;
        };
        output[row][column] = str[i];
        column = column + 1;
    };
    return;
};

void showBackground(string output[], int countStart) {
    int i, j, row, count;
    j = terminalRows - 3 - sizeBackground;
    for(row = 0; row < sizeBackground; row++) {
        count = countStart;
        for(i = 0; i < terminalColumns; i++) {
            output[j + row][i] = backGround[row][count];
            count = count + 1;
            if(count > backGround[row].length() - 1) {
                count = 0;
            };
        };
    };
    return;
};

void showBird(string output[], int countAnimation[], int sizeInAnimation, int skinIndex, int countGoUp) {
    int index;
    if (countAnimation == NULL) {
        index = 0;
    } else {
        index = countAnimation[0];
    };
    showAnimation(output, skinFlyAnimation[skinIndex][index], sizeInAnimation, countGoUp);

    if (countAnimation != NULL) {
        if ((countAnimation[0] >= 3) || (countAnimation[0] <= 0) ) {
            countAnimation[1] = !countAnimation[1];
        };

        if(!countAnimation[1]) {
            countAnimation[0] = countAnimation[0] + 1;
        } else {
            countAnimation[0] = countAnimation[0] - 1;
        };
    };
    return;
};

void brightnessSettings() {
    //  
    //  ______________
    // [=========     ]
    //  ``````````````
    //
    if (settingsData[7]) {
        int sizeBar = 15;
        int max = 3;

        int currentBrightness = getBrightness();
        
        int i, j;
        string output[terminalRows - 2];
        int sizeOutput = sizeof(output) / sizeof(output[0]);
        string road = getRoad();

        string text;
        string tmp;
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
            };
            text = "";
            for(i = 0; i < terminalRows - 4; i++) {
                if ((terminalRows / 2) - 2 == i) {
                    text = text + lineSpace + " ";
                    for(j = 0; j < sizeBar; j++) {
                        text = text + "_";
                    };
                    text = text + "\n";

                    if(currentBrightness > 1) {
                        tmp = string(lineSpace);
                        tmp.resize(lineSpace.length() - 5);
                        text = text + tmp + "<--  ";
                    } else {
                        text = text + lineSpace;
                    };

                    text = text + "[";

                    for(j = 0; j < currentBrightness; j++) {
                        text = text + perProcess;
                    };
                    for(j = 0; j < max - currentBrightness; j++) {
                        text = text + blankPerProcess;
                    };

                    text = text + "]";
                    if(currentBrightness < max) {
                        text = text + "  -->";
                    };

                    text = text + "\n" + lineSpace + " ";

                    for(j = 0; j < sizeBar; j++) {
                        text = text + "`";
                    };
                    text = text + "\n";
                } else {
                    text = text + "\n";
                };
            };
            
            color(settingsData[2]);
            wipeOutput(output, sizeOutput);
            stringToOutput(text, output, sizeOutput);
            showBackground(output, 0);
            output[sizeOutput - 1] = road;
            text = getOutput(output, sizeOutput);
            clearTerminal();
            cout << text;
            bottomKeymap("| [" + getNameKey(keymapData[2][1], keymapData[2][0]) + "] -> LOW | [" + getNameKey(keymapData[3][1], keymapData[3][0]) + "] -> HIGH | [" + getNameKey(keymapData[4][1], keymapData[4][0]) + "] -> BACK |");
            inputMenu(&currentBrightness, 2, 2);
            Sleep(50);
        };
    } else {
        errorBox("COLOR DISABLED", "", true);
    };
    return;
};

void resolutionSettings() {
    string menu[5] = {
        " 80 x 20",
        "100 x 26",
        "120 x 30",
        "140 x 36",
        "160 x 40"
    };
    int sizeMenu = sizeof(menu) / sizeof(menu[0]);
    int choose = getResolutionValue();
    string titleMenu;
    while(true) {
        if (choose == -1) {
            flushStdin();
            return;
        };
        titleMenu = "| Current resolution: " + to_string(terminalColumns) + " x " + to_string(terminalRows) + " |";
        showMenu(titleMenu, menu, sizeMenu, &choose);
        inputMenu(&choose, sizeMenu - 1, 3);
        Sleep(100);
    };
    return;
};

void highScore() {
    int i;
    string menu[sizelistHighScore];
    int sizeMenu = sizeof(menu) / sizeof(menu[0]);
    for(i = 0; i < sizeMenu; i++) {
        menu[i] = to_string(listHighScore[i]);
    };
    int choose = 0;
    while(true) {
        if (choose == -1) {
            flushStdin();
            return;
        };
        showMenu("| High score |", menu, sizeMenu, &choose);
        bottomKeymap("| [" + getNameKey(keymapData[4][1], keymapData[4][0]) + "][" + getNameKey(keymapData[5][1], keymapData[5][0]) + "] -> MAIN MENU |");
        inputMenu(&choose, sizeMenu - 1, 4);
        Sleep(100);
    };
    return;
};

void benchmark() {
    errorBox("Unavailable", "", true);
    return;
};

bool setSkin(int index) {
    if ((index >= 0) && (index < (sizeof(skinFlyAnimation) / sizeof(skinFlyAnimation[0])))) {
        settingsData[4] = index;
        writeConfig("skin", to_string(index));
        return 1;
    };
    return 0;
};

void previewSkin(int index) {
    int i = terminalRows - 2;
    int choose = 0;
    int countAnimation[2] = {1, 0};
    int sizeInAnimation = sizeof(skinFlyAnimation[index][countAnimation[0]]) / sizeof(skinFlyAnimation[index][countAnimation[0]][0]);
    string road = getRoad();
    string output[i];
    string finalOutput;
    while(true) {
        if(choose == -1) {
            break;
        };
        if(choose == -2) {
            if (setSkin(index)) {
                showBoxText("Completed", true);
                bottomKeymap("Press any key to continue!");
                anyKey();
            } else {
                errorBox("Cannot set skin", "", true);
            };
            break;
        };
        wipeOutput(output, i);
        showBackground(output, 0);
        showBird(output, countAnimation, sizeInAnimation, index, 0);
        output[i - 1] = road;
        finalOutput = getOutput(output, i);
        clearTerminal();
        cout << finalOutput;
        bottomKeymap("| [" + getNameKey(keymapData[6][1], keymapData[6][0]) + "] -> SET | [" + getNameKey(keymapData[4][1], keymapData[4][0]) + "] -> BACK |");
        inputMenu(&choose, 0, -7);
        Sleep(250);
    };
    flushStdin();
    return;
};

void changeSkin() {
    string space = "";
    int i;
    int choose = 0;
    int sizeAnimation = sizeof(skinFlyAnimation) / sizeof(skinFlyAnimation[0]);
    string menu[sizeAnimation];

    if (sizeAnimation > 9) {
        space = space + " ";
    } else if (sizeAnimation > 99) {
        space = space + "  ";
    };

    for(i = 0; i < sizeAnimation; i++) {
        menu[i] = "skin_" + to_string(i + 1) + space;
    };
    while(true) {
        if (choose == -1) {
            flushStdin();
            return;
        };
        showMenu("| Current skin: skin_" + to_string(settingsData[4] + 1) + " |", menu, sizeAnimation, &choose);
        inputMenu(&choose, sizeAnimation - 1, 5);
        Sleep(100);
    };
    return;
};

void settingsMenu() {
    string menu[7] = {
        "",
        "",
        "    Brightness   ",
        "    Keymapping   ",
        "    Resolution   ",
        "",
        "    Back         "
    };
    int sizeMenu = sizeof(menu)/sizeof(menu[0]);
    int choose = 0;
    while(true) {
        if (choose == -1) {
            flushStdin();
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

        if (settingsData[3]) {
            menu[5] =  "    Auto [x]     ";
        } else {
            menu[5] =  "    Auto [ ]     ";
        };

        showMenu("| Settings |", menu, sizeMenu, &choose);
        inputMenu(&choose, sizeMenu - 1, 1);
        Sleep(100);
    };
    return;
};

int pausedMenu(bool isShowPauseInGame) {
    int choose = 0;

    // paused ingame
    flushStdin();
    if (isShowPauseInGame) {
        while(true) {
            if (choose == -1) {
                color(settingsData[2]);
                return 0;
            };
            if (choose == -2) {
                break;
            };
            showBoxText("PAUSED", false);
            bottomKeymap("| [" + getNameKey(keymapData[4][1], keymapData[4][0]) + "][" + getNameKey(keymapData[5][1], keymapData[5][0]) + "] -> RESUME | [" + getNameKey(keymapData[6][1], keymapData[6][0]) + "] -> PAUSED MENU |");
            inputMenu(&choose, 0, -5);
            Sleep(100);
        };
    };

    // paused menu
    choose = 0;
    string menu[6] = {
        "  Continue  ",
        "  New game  ",
        " High score ",
        "  Settings  ",
        " Main menu  ",
        "    Exit    "
    };

    int sizeMenu = sizeof(menu) / sizeof(menu[0]);

    clearTerminal();
    while(true) {
        if (choose == -1) {
            return 0;
        };
        if (choose == -2) {
            return 1;
        };
        if (choose == -3) {
            return 2;
        };
        showMenu("| Paused |", menu, sizeMenu, &choose);
        inputMenu(&choose, sizeMenu - 1, -4);
        Sleep(100);
    };
    return 1;
};

void launchPaused(int *chooseMenu) {
    bool tmp = true;
    int tmp2 = 0;
    while(true) {
        tmp2 = pausedMenu(tmp);
        if (tmp2 == 1) {
            if (showYesorNo("Back to main menu?")) {
                *chooseMenu = -1;
                break;
            } else {
                tmp = false;
            };
        } else if (tmp2 == 2) {
            if (showYesorNo("New game?")) {
                *chooseMenu = -2;
                break;
            } else {
                tmp = false;
            };
        } else {
            *chooseMenu = -3;
            break;
        };
    };
    return;
};

void moreOptions() {
    string menu[2] = {
        " Benchmark ",
        "    Back   "
    };
    int sizeMenu = sizeof(menu)/sizeof(menu[0]);
    int choose = 0;
    while(true) {
        if (choose == -1) {
            return;
        };
        showMenu("", menu, sizeMenu, &choose);
        inputMenu(&choose, sizeMenu - 1, 6);
        Sleep(100);
    };
    return;
};

void mainMenu() {
    isInGame = 0;
    string menu[7] = {
        "   Start  ",
        "High score",
        "   Skin   ",
        "   More   ",
        " Settings ",
        "  Credit  ",
        "   Exit   "
    };
    int sizeMenu = sizeof(menu)/sizeof(menu[0]);
    int chooseMenu = 0;
    flushStdin();
    while(true) {
        showMenu("", menu, sizeMenu, &chooseMenu);
        inputMenu(&chooseMenu, sizeMenu - 1, 0);
        Sleep(100);
    };
    return;
};

void configError(int key) {
    resizeTerminal(terminalColumns, terminalRows);
    thread lockSizeTer(lockSizeTerminal);
    int j;
    int size = sizeof(keymapData) / sizeof(keymapData[0]);
    for(j = 0; j < size; j++) {
        writeConfig("key" + to_string(j), "-1");
    };
    clearTerminal();
    errorBox("Keymap Error [key" + to_string(key) + "]", "Keymap configuration has been reset! You can restart the game now!", false);
    while(true) {
        _getch();
    };
    exit(-1);
    return;
};

void loadConfig() {
    if (settingsData[6]) {
        bool isTwoChar;
        int i, j, k;
        int size = sizeof(keymapData) / sizeof(keymapData[0]);
        int newKeymapData[size][2];
        for(i = 0; i < sizeof(newKeymapData) / sizeof(newKeymapData[0]); i++) {
            newKeymapData[i][1] = -1;
            newKeymapData[i][0] = 0;
        };
        if (settingsData[9]) {
            setResolution(readConfig("resolution"));
        } else {
            setResolution(0);
        };
        if (settingsData[8]) {
            settingsData[0] = readConfig("music");
            settingsData[1] = readConfig("sfx");
        };
        if (settingsData[7]) {
            setBrightness(readConfig("brightness"));
        };
        if (!setSkin(readConfig("skin"))) {
            writeConfig("skin", "0");  
        };
        for(i = 0; i < size; i++) {
            isTwoChar = 0;
            j = readConfig("key" + to_string(i));
            if(j > 500) {
                j = j - 500;
                isTwoChar = 1;
            };
            if (j > 7) {
                for(k = 0; k < size; k++) {
                    if((j == newKeymapData[k][1]) && (isTwoChar == newKeymapData[k][0])) {
                        configError(i);
                    };
                };
            };
            if (setKeymap(i, j, isTwoChar)) {
                newKeymapData[i][0] = isTwoChar;
                newKeymapData[i][1] = j;
            } else {
                writeConfig("key" + to_string(i), "-1");
            };
        };
        // check keymap after set
        for(i = 0; i < size; i++) {
            for(j = 0; j < size; j++) {
                if(i == j) {
                    continue;
                };
                if(keymapData[i] == keymapData[j]) {
                    configError(i);
                };
            };
        };
    } else {
        setResolution(0);
    };
    return;
};

void loadHighScore() {
    int i;
    for(i = 0; i < sizelistHighScore; i++) {
        listHighScore[i] = 0;
    };

    int countLine = 0;
    string line;
    string data = readFile(scoreFileName);
    if (data == "") {
        return;
    };
    istringstream file(data);
    while(getline(file, line)) {
        try {
            i = stoi(line);
            countLine = countLine + 1;
        } catch (...) {
            continue;
        };
    };

    i = 0;
    int *tmpList = new int[countLine];
    istringstream f(data);
    while(getline(f, line)) {
        tmpList[i] = stoi(line);
        i = i + 1;
    };

    int maxIndex;
    int j;
    int max;
    for(i = sizelistHighScore - 1; i >= 0 ; i--) {
        maxIndex = 0;
        max = tmpList[maxIndex];
        for(j = 1; j < countLine; j++) {
            if (max < tmpList[j]) {
                max = tmpList[j];
                maxIndex = j;
            };
        };

        if (listHighScore[i] < max) {
            listHighScore[i] = max;
        };
        tmpList[maxIndex] = -1;
    };
    return;
};

int getHighScore(int currentScore) {
    int i;
    int j, k, l;
    j = listHighScore[0];
    for(i = 1; i < sizelistHighScore; i++) {
        if (j < listHighScore[i]) {
            j = listHighScore[i];
            k = listHighScore[i];
        };
    };
    for(i = 0; i < sizelistHighScore; i++) {
        l = listHighScore[i] - currentScore;

        if (l > 0 && l < k) {
            j = listHighScore[i];
            k = l;
        };
    };
    return j;
};

void showHighScore(string output[], int highScore, bool isHigher) {
    if (highScore > 0) {
        string text;
        string highScore_str;
        if (isHigher) {
            highScore_str = "| HIGH SCORE: ";
        } else {
            highScore_str = "| TARGET: ";
        };
        highScore_str = highScore_str + to_string(highScore) + " |";
        int highScore_length = highScore_str.length() - 4;

        int i, k;
        int j = terminalColumns - highScore_length - 6;
        
        text = "+-";
        for(i = 0; i < highScore_length; i++) {
            text = text + "-";
        };
        text = text + "-+";

        if (isHigher) {
            i = 2;
        } else {
            i = 0;
        };

        for(k = 0; k < text.length(); k++) {
            output[3 - i][j + k] = text[k];
        };
        
        for(k = 0; k < highScore_str.length(); k++) {
            output[4 - i][j + k] = highScore_str[k];
        };

        for(k = 0; k < text.length(); k++) {
            output[5 - i][j + k] = text[k];
        };
    };
    return;
};

void showScore(string output[], int score, int higherScore, bool isHigher) {
    showHighScore(output, higherScore, isHigher);
    if (!isHigher) {
        string text;
        string score_str = "| SCORE: " + to_string(score) + " |";
        int score_length = score_str.length() - 4;

        int i, k;
        int j = terminalColumns - score_length - 6;
        
        text = "+-";
        for(i = 0; i < score_length; i++) {
            text = text + "-";
        };
        text = text + "-+";

        for(k = 0; k < text.length(); k++) {
            output[1][j + k] = text[k];
        };

        for(k = 0; k < score_str.length(); k++) {
            output[2][j + k] = score_str[k];
        };

        for(k = 0; k < text.length(); k++) {
            output[3][j + k] = text[k];
        };
    };
    return;
};

void showWall(string output[], int column, int up, int down) {
    string text = "|__|";
    string text2 = "|``|";
    int i, j, k;

    // up
    for(i = 0; i < up; i++) {
        for(k = 0; k < text.length(); k++) {
            output[i][column + k] = text[k]; 
        };
    };

    // down
    j = terminalRows - 3;
    for(i = down + 1; i < j; i++) {
        for(k = 0; k < text.length(); k++) {
            output[i][column + k] = text2[k]; 
        };
    };
    return;
};

void showAllWall(string output[], int *nextWall, int *score, int countWall) {
    int i, j;

    for(i = 0; i < sizelistWall; i++) {
        if ((listWall[i][0] > -1) && (listWall[i][0] < terminalColumns - 3)) {
            // display Wall
            showWall(output, listWall[i][0], listWall[i][1], listWall[i][2]);
            if(gameStarted) {
                listWall[i][0] = listWall[i][0] - 1; // decrease terminalColumn
            };
        } else if ((listWall[i][1] > -1) && (gameStarted)) {
            // remove Wall cannot display
            for(j = 0; j < 3; j++) {
                listWall[i][j] = -1;
            };
            *nextWall = i + 1;
            *score = *score + 1;
            if (*nextWall > sizelistWall - 1) {
                *nextWall = 0;
            };
        };
    };
    return;
};

bool gameOver(int score, int y, int minY, int maxY) {
    if(!settingsData[3]) {
        int i;
        for(i = sizelistHighScore - 1; i >= 0; i--) {
            if (score > listHighScore[i]) {
                writeFile(scoreFileName, readFile(scoreFileName) + to_string(score) + "\n");
                listHighScore[i] = score;
                break;
            };
        };
    };
    int choose = 0;
    while(true) {
        if (choose == -1) {
            return 1;
        };
        if (choose == -2) {
            return 0;
        };
        showBoxText("Game over", false);
        bottomKeymap("| [" + getNameKey(keymapData[6][1], keymapData[6][0]) + "] -> TRY AGAIN | [" + getNameKey(keymapData[4][1], keymapData[4][0]) + "] -> MAIN MENU | Y: " + to_string(y) + " | minY: " + to_string(minY) + " | maxY: " + to_string(maxY) + " |");
        inputMenu(&choose, 0, -6);
        Sleep(200);
    };
    return 0;
};

void resetWall() {
    int i, j;
    for(i = 0; i < sizelistWall; i++) {
        for(j = 0; j < sizeof(listWall[i]) / sizeof(listWall[i][0]); j++) {
            listWall[i][j] = -2;
        };
    };
    return;
};

void addWall(int countWall) {
    listWall[countWall][0] = terminalColumns - 5;
    listWall[countWall][1] = (rand() % (terminalRows - 13)) + 1; // up
    listWall[countWall][2] = listWall[countWall][1] + 6; // down
    return;
};

void checkWall(int nextWall, int y, int maxUp, bool *isOver) {
    if(!gameStarted) {
        return;
    };
    int i;
    if (listWall[nextWall][0] == -2) {
        return;
    };
    if (listWall[nextWall][0] < 8) { // < size Animation Bird + 1
        if ((listWall[nextWall][1] < 0) || (listWall[nextWall][2] < 0)) {
            return;
        };
        maxUp = maxUp + 1;
        if ((maxUp - listWall[nextWall][1] <= y) || (maxUp - listWall[nextWall][2] >= y)) {
            *isOver = 1;
        };
    };
    return;
};

void flappyBird() { 
    isInGame = 1;
    int i;
    int minY = 0;
    int maxY = 0;
    string output[terminalRows - 2];
    int sizeOutput = sizeof(output) / sizeof(output[0]);
    bool isOver = 0;
    gameStarted = 0;
    bool highScoreIsScore = 0;
    int countStart = rand() % sizeBackground;
    int nextWall = 0;
    int countWall = 0;
    int wallCreateDistance = 20;
    for(i = 0; i < getResolutionValue(); i++) {
        wallCreateDistance = wallCreateDistance + 5;
    };
    int distance = wallCreateDistance - 4;
    int countAnimation[2] = {1, 0};
    int score = 0;
    int highScore_ = getHighScore(score);
    int x = 0;
    int y = 0;
    int oldX = x;
    int choose = 0;
    int sizeBird = sizeof(skinFlyAnimation[settingsData[4]]) / sizeof(skinFlyAnimation[settingsData[4]][0]);
    int maxUp = terminalRows / 4;
    int sizeInAnimation = sizeof(skinFlyAnimation[settingsData[4]][countAnimation[0]]) / sizeof(skinFlyAnimation[settingsData[4]][countAnimation[0]][0]);
    string text;
    string outputGame;
    resetWall();
    string lineMap = getRoad();
    color(settingsData[2]);
    showChangeScene();
    flushStdin();
    while(true) {
        if (choose == -1) { // return to main menu
            resetWall();
            isInGame = 0;
            showChangeScene();
            disableCloseButton(false);
            flushStdin();
            return;
        };
        if (choose == -3) { // continue option in pausedMenu
            gameStarted = 1;
            choose = 0;
            continue;
        };
        if(gameStarted) {
            maxY = maxUp + 1 - listWall[nextWall][1];
            minY = maxUp + 1 - listWall[nextWall][2];
            if ((y == -(terminalRows - sizeBird - maxUp - 1)) || (isOver)) {
                showAnimation(NULL, skinDeadAnimation[settingsData[4]], sizeof(skinDeadAnimation[settingsData[4]]) / sizeof(skinDeadAnimation[settingsData[4]][0]), y);
                gameStarted = 0;
                disableCloseButton(false);
                if (gameOver(score, y, minY, maxY)) {
                    choose = -1;
                    continue;
                } else {
                    choose = -2;
                };
            };
        };
        if (choose == -2) { // new Game
            resetWall();
            choose = 0;
            score = 0;
            distance = wallCreateDistance - 4;
            x = 0;
            y = 0;
            isOver = 0;
            nextWall = 0;
            countWall = 0;
            highScoreIsScore = 0;
            countAnimation[0] = 1;
            countAnimation[1] = 0;
            highScore_ = getHighScore(score);
            countStart = rand() % sizeBackground;
            oldX = 0;
            gameStarted = 0;
            disableCloseButton(false);
            flushStdin();
            continue;
        };
        if ((settingsData[3]) && (gameStarted)) { // cheat mode activated
            if ((minY == maxUp + 3) && (maxY == maxUp + 3)) { // maxUp + 1 - (-2)
                // skip due to noWall
                if (y < maxUp / 2) {
                    y = y + 2;
                } else {
                    y = y - 1;
                };
            } else {
                if (y <= minY + 2) {
                    y = y + 2;
                } else {
                    y = y - 1;
                };
            };
            choose = 0;
        } else if (choose == -9) { // received by SPACE button
            choose = 0;
            if(!gameStarted) {
                disableCloseButton(true);
                gameStarted = 1;
                continue;
            };
            if (!settingsData[3]) {
                if (y == maxUp - 1) {
                    y = y + 1;
                } else if (y < maxUp) {
                    y = y + 2;
                };
            };
        } else {
            if (gameStarted) {
                y = y - 1;
            };
        };
        if(gameStarted) {
            if((x - oldX) > 50) {
                countStart = countStart + 1;
                if(countStart > sizeBackground - 1) {
                    countStart = 0;
                };
                oldX = x;
            };
            x = x + 1;
            if (distance >= wallCreateDistance) {
                addWall(countWall);
                countWall = countWall + 1;
                distance = 0;
            };
            
            if (countWall >= sizelistWall) {
                countWall = 0;
            };
            distance = distance + 1;
            if (!highScoreIsScore) {
                if (score > highScore_) {
                    highScore_ = getHighScore(score);
                };
                if (score > highScore_) {
                    highScoreIsScore = 1;
                };
            } else {
                highScore_ = score;
            };
        };

        text = "| ";
        if (gameStarted) {
            if (isBenchmark) {
                text = text + "BENCHMARK";
            } else if (settingsData[3]) {
                text = text + "AUTO MODE";
            } else {
                text = text + "[" + getNameKey(keymapData[5][1], keymapData[5][0]) + "] -> GO UP";
            };
        } else {
            text = text + "[" + getNameKey(keymapData[5][1], keymapData[5][0]) + "] -> PLAY";
        };
        text = text + " | [" + getNameKey(keymapData[4][1], keymapData[4][0]) + "] -> PAUSE | X: " + to_string(x) + " | Y: " + to_string(y) + " | minY: " + to_string(minY) + " | maxY: " + to_string(maxY) + " |";
        wipeOutput(output, sizeOutput);
        showBackground(output, countStart);
        showAllWall(output, &nextWall, &score, countWall);
        showBird(output, countAnimation, sizeInAnimation, settingsData[4], y);
        showScore(output, score, highScore_, highScoreIsScore);
        output[terminalRows - 3] = lineMap;
        outputGame = getOutput(output, sizeOutput);

        // output
        clearTerminal();
        cout << outputGame;

        bottomKeymap(text);
        inputMenu(&choose, 0, -3);
        checkWall(nextWall, y, maxUp, &isOver);
        Sleep(150);

        if ((!checkTerminalActive()) && (!settingsData[3]) && (gameStarted)) {
            launchPaused(&choose);
        };
    };
    return;
};

void inputMenu(int *chooseMenu, int max, int type_menu) {
    if (_kbhit()) {
        int p[2];
        __getch(p);
        if ((p[1] == keymapData[0][1]) && (p[0] == keymapData[0][0])) /* UP */ {
            switch (type_menu) {
                case -4: // pausedMenu
                case -2: // keymappingSettings
                case 0:  // mainMenu
                case 1:  // settingsMenu 
                case 3:  // resolutionSettings
                case 4:  // highScore
                case 5:  // changeSkin
                case 6:  // moreOptions
                    if(*chooseMenu > 0) {
                        *chooseMenu = *chooseMenu - 1;
                    } else {
                        *chooseMenu = max;
                    };
                    break;
            };
        } else if ((p[1] == keymapData[2][1]) && (p[0] == keymapData[2][0])) /* LEFT */ {
            switch (type_menu) {
                case 2: // brightnessSettings
                    if(*chooseMenu > 1) {
                        *chooseMenu = *chooseMenu - 1;
                        setBrightness(*chooseMenu);
                    };
                    break;
            };
        } else if ((p[1] == keymapData[3][1]) && (p[0] == keymapData[3][0])) /* RIGHT */ {
            switch (type_menu) {
                case 2:
                    if(*chooseMenu <= max) {
                        *chooseMenu = *chooseMenu + 1;
                        setBrightness(*chooseMenu);
                    };
                    break;
            };
        } else if ((p[1] == keymapData[1][1]) && (p[0] == keymapData[1][0])) /* DOWN */ {
            switch (type_menu) {
                case -4:
                case -2:
                case 0:
                case 1:
                case 3:
                case 4:
                case 5:
                case 6:
                    if(*chooseMenu < max) {
                        *chooseMenu = *chooseMenu + 1;
                    } else {
                        *chooseMenu = 0;
                    };
                    break;
            };
        } else if ((p[1] == keymapData[6][1]) && (p[0] == keymapData[6][0])) /* ENTER */ {
            switch(type_menu) {
                case -7:
                case -6:
                case -5:
                    *chooseMenu = -2;
                    break;
                case -4:
                    if(*chooseMenu == 0) {
                        *chooseMenu = -1;
                    } else if (*chooseMenu == 1) {
                        *chooseMenu = -3;
                    } else if (*chooseMenu == 2) {
                        highScore();
                    } else if (*chooseMenu == 3) {
                        settingsMenu();
                    } else if (*chooseMenu == 4) {
                        *chooseMenu = -2;
                    } else if (*chooseMenu == 5) {
                        exitProgram();
                    };
                    break;
                case -2:
                    changeKeymapping(*chooseMenu);
                    break;
                case 0:
                    if(*chooseMenu == 0) {
                        flappyBird();
                    } else if (*chooseMenu == 1) {
                        highScore();
                    } else if (*chooseMenu == 2) {
                        changeSkin();
                    } else if (*chooseMenu == 3) {
                        moreOptions();
                    } else if (*chooseMenu == 4) {
                        settingsMenu();
                    } else if (*chooseMenu == 5) {
                        credit();
                    } else if (*chooseMenu == 6) {
                        exitProgram();
                    };
                    break;
                case 1:
                    if(*chooseMenu == 0) {
                        if (settingsData[8]) {
                            settingsData[0] = !settingsData[0];
                            playSound_main("sound\\mainmenu.wav");
                            if (!settingsData[0]) {
                                stopSound();
                            };
                            writeConfig("music", to_string(settingsData[0]));
                        } else {
                            errorBox("SOUND DISABLED", "", true);
                        };
                    } else if (*chooseMenu == 1) {
                        if (settingsData[8]) {
                            settingsData[1] = !settingsData[1];
                            writeConfig("sfx", to_string(settingsData[1]));
                        } else {
                            errorBox("SOUND DISABLED", "", true);
                        };
                    } else if (*chooseMenu == 2) {
                        brightnessSettings();
                    } else if (*chooseMenu == 3) {
                        keymappingSettings();
                    } else if (*chooseMenu == 4) {
                        if(isInGame) {
                            errorBox("Unavailable in game", "You must return to main menu first!", true);
                        } else {
                            if (settingsData[9]) {
                                resolutionSettings();
                            } else {
                                errorBox("RESOLUTION DISABLED", "", true);
                            };
                        };
                    } else if (*chooseMenu == 5) {
                        if(isInGame) {
                            errorBox("Unavailable in game", "You must return to main menu first!", true);
                        } else {
                            settingsData[3] = !settingsData[3];
                        };
                        // writeConfig("cheat", to_string(settingsData[1]));  // cheat mode not need saved!
                    } else if (*chooseMenu == 6) {
                        *chooseMenu = -1;
                    };
                    break;
                case 3:
                    if(*chooseMenu > -1) {
                        setResolution(*chooseMenu);
                    };
                    break;
                case 5:
                    previewSkin(*chooseMenu);
                    break;
                case 6:
                    if (*chooseMenu == 0) {
                        benchmark();
                    } else if (*chooseMenu == 1) {
                        *chooseMenu = -1;
                    };
                    break;
            };
        } else if ((p[1] == keymapData[4][1]) && (p[0] == keymapData[4][0])) /* ESC */ {
            switch(type_menu) {
                case -7:
                case -6:
                case -5:
                case -4:
                case -2:
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                    *chooseMenu = -1;
                    break;
                case -3: // flappyBird
                    launchPaused(&*chooseMenu);
                    break;
            };
        } else if ((p[1] == keymapData[5][1]) && (p[0] == keymapData[5][0])) /* SPACE */ {
            switch(type_menu) {
                case -5:
                case 4:
                    *chooseMenu = -1;
                    break;
                case -3:
                    *chooseMenu = -9;
                    break;
                case -1: // loadingFrame
                    showTip("");
                    break;
            };
        } else {
            return;
        };
    };
    return;
};

void checkARG(int argc, char const *argv[]) {
    if (argc > 1) {
        setResolution(0);
        int i, j;
        string arg;
        string tmp;
        string text = "";
        text = text + ">> Using debug arguments can affect performance and experience!\n";
        for(i = 1; i < argc; i++) {
            arg = "";
            tmp = "INVALID";
            j = 0;
            // append char to string
            while(argv[i][j] != '\0') {
                arg = arg + argv[i][j];
                j = j + 1;
            };
            // check arg
            if ((arg == "-h" ) || (arg == "--help")) {
                clearTerminal();
                cout << " >> HELP: \n";
                cout << "COMMAND: " << argv[0] << " [ARG]\n";
                cout << "ARG: ";
                cout << "\t--disable-v2\t\t Disable v2 mode CMD\n";
                cout << "\t--disable-conf\t\t Disable config file (read/write)\n";
                cout << "\t--disable-color\t\t Disable color\n";
                cout << "\t--disable-sound\t\t Disable all sound (Music/SFX)\n";
                cout << "\t--disable-resolution\t Disable resolution (resize)\n";
                cout << "\nPRESS ANY KEY TO EXIT\n";
                flushStdin();
                _getch();
                exit(-1);
            };
            if (arg == "--disable-v2") {
                settingsData[5] = 0;
                tmp = "OK";
            };
            if (arg == "--disable-conf") {
                settingsData[6] = 0;
                tmp = "OK";
            };
            if (arg == "--disable-color") {
                settingsData[7] = 0;
                tmp = "OK";
            };
            if (arg == "--disable-sound") {
                settingsData[8] = 0;
                settingsData[0] = 0;
                settingsData[1] = 0;
                tmp = "OK";
            };
            if (arg == "--disable-resolution") {
                settingsData[9] = 0;
                tmp = "OK";
            };
            text = text + "argument: [" + arg + "] => " + tmp + "\n";
        };
        clearTerminal();
        cout << text;
        cout << "\n>> Program will start in 5 seconds...\n";
        Sleep(5250);
    };
    return;
};

int main(int argc, char const *argv[]) {
    SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS); // Realtime priority Process
    ios_base::sync_with_stdio(true); // Enable synchronization with stdio (slower performance)

    checkARG(argc, &*argv);

    configureTerminal();
    loadConfig();
    clearTerminal();
    resizeTerminal(terminalColumns, terminalRows);
    thread lockSizeTer(lockSizeTerminal);

    srand(time(NULL));
    
    loadHighScore();

    Sleep(1000);
    clearTerminal();

    banner();
    Sleep(1000);

    // Loading time... [cho đẹp thôi chứ k có load gì đâu =)))]
    showTip("");
    tmp_int[1] = 1;
    flushStdin();
    for(int i = 0; i <= 100; i = i + 2) {
        loadingFrame(i, true);
        inputMenu(&tmp_int[0], 0, -1);
        if(i < 20) {
            Sleep(200);
        } else if(i < 70) {
            Sleep(40);
        } else if (i < 90) {
            Sleep(60);
        } else {
            Sleep(120);
        };
    };

    Sleep(500);
    clearTerminal();
    
    playSound_main("sound\\mainmenu.wav");
    
    mainMenu();
    return 0;
};
