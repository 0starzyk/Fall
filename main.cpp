#include <iostream>
#include <windows.h>
#include <time.h>
#include <stdlib.h>

class Screen
{
public:
    void setup(SHORT font_width, SHORT font_height, int x_position, int y_position, int console_width, int console_height)
    {
        CONSOLE_FONT_INFOEX console_font = {0};
        console_font.cbSize = sizeof console_font;
        console_font.dwFontSize.X = font_width;
        console_font.dwFontSize.Y = font_height;
        wcscpy(console_font.FaceName, L"Terminal");
        SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), 0, &console_font);

        HWND console = GetConsoleWindow();
        RECT ConsoleRect;
        GetWindowRect(console, &ConsoleRect);
        MoveWindow(console, x_position, y_position, console_width, console_height, TRUE);
    }

    int getWidth() const { return width; }
    
    int getHeight() const { return height; }

    void drawPoint(int x, int y, wchar_t c)
    {
        pointer[x + width * y] = c;
    }

    void drawLine(int left, int right, int height, wchar_t c)
    {
        for (int i = left; i <= right; i++)
        {
            drawPoint(i, height, c);
        }
    }

    void drawRectangle(int top, int bottom, int right, int left, wchar_t c)
    {
        for (int i = right; i <= left; i++)
        {
            drawPoint(i, top, c);
            drawPoint(i, bottom, c);
        }
        for (int i = top + 1; i <= bottom - 1; i++)
        {
            drawPoint(right, i, c);
            drawPoint(left, i, c);
        }
    }
    void clear(wchar_t c)
    {
        for (int i = 0; i < width * height; i++)
            pointer[i] = c;
    }

    Screen(int w, int h, wchar_t* p) : width(w), height(h), pointer(p) {}
    
    ~Screen()
    {
        delete [] pointer;
    }
    
private:
    int width;
    int height;
public:
    wchar_t* pointer;
};

class Bullet
{
public:
    int x, y;

    Bullet(int X, int Y) : x(X), y(Y) {}
};

class Board
{
public:
    int x1, x2, y;
    Board(int X1, int X2, int Y) : x1(X1), x2(X2), y(Y) {}
};

int main()
{
    // Screen setup
    Screen screen(51, 51, new wchar_t[51 * 51]);
	screen.setup(8, 8, 100, 100, 360, 368);
	HANDLE Console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(Console);
	DWORD dwBytesWritten = 0;

    // Auxiliary variables
    bool game_over = false;
    DWORD tm = 40;
    int score = 0;

    // Game objects
    Bullet bullet(25, 0);
    Board board(20, 29, 50);

    // Start randomizing
    srand((unsigned)time(NULL));

    // Game loop
    while (!game_over)
    {
        // Input
        if (GetAsyncKeyState(VK_RIGHT) && board.x2 < 50)
        {
            board.x1++;
            board.x2++;
        }
        else if (GetAsyncKeyState(VK_LEFT) && board.x1 > 0)
        {
            board.x1--;
            board.x2--;
        }
        
        // Logic
        if (bullet.y == 50)
            game_over = true;
        
        if (bullet.y + 1 == board.y && bullet.x >= board.x1 && bullet.x <= board.x2)
        {
            bullet.x = rand() % 50;
            bullet.y = 0;
            score++;

            if (score == 10 || score == 20 || score == 30 || score == 40)
            {
                board.x1++;
                board.x2--;
            }

            if (tm > 15)
                tm -= 1;
        }

        // Progression
        bullet.y++;        

        // Clearing screen and drawing objects
        screen.clear(L' ');
        screen.drawPoint(bullet.x, bullet.y, L'*');
        screen.drawLine(board.x1, board.x2, board.y, L'$');

        // End-game condition
        if (game_over)
        {
            wsprintfW(&screen.pointer[18], L"Your score is %d", score);
            wsprintfW(&screen.pointer[20 + 23*51], L"PRESS SPACE", score);
        }

        // Drawing score
        if (!game_over)
        {
            wsprintfW(&screen.pointer[0], L"SCORE: %d", score);
        }

        // Displaying
        WriteConsoleOutputCharacterW(Console, screen.pointer, screen.getWidth() * screen.getHeight(), {0,0}, &dwBytesWritten);

        // Screen freeze
        Sleep(tm);
    }
    
    // Waiting for space
    while (!GetAsyncKeyState(VK_SPACE));
}