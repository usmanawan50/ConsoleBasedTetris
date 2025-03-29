#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include <string>
#include <thread>
#include <vector>
#include <fstream>
using namespace std;

// global arrays/variables. 
wstring tetrablocks[7];				// array to store tetris block shapes.
int Field_Width = 12;				// width of the game's Field.
int Field_Height = 18;				// height of the game's Field.
unsigned char *pField = nullptr; 	// store all elements of the game's Field as unsigned characters.
int Screen_Width = 120;				// Console screen size X (columns).
int Screen_Height = 30;				// Console screen size Y (rows).

// declare prototypes.
int Rotate_Block(int px, int py, int r);	// to rotate tetris blocks.
int Check_Block_Fitting(int tetrablock_index, int Block_Rotation, int posX, int posY);	// check if active tetrablock fits the screen bottom.
void delay(int milliseconds);

int main()
{
	// create assets(shapes of tetris blocks).
	tetrablocks[0].append(L"..#.");		// shape A.
	tetrablocks[0].append(L"..#.");
	tetrablocks[0].append(L"..#.");
	tetrablocks[0].append(L"..#.");

	tetrablocks[1].append(L"..#.");		// shape B.
	tetrablocks[1].append(L".##.");
	tetrablocks[1].append(L".#..");
	tetrablocks[1].append(L"....");

	tetrablocks[2].append(L".#..");		// shape C.
	tetrablocks[2].append(L".##.");
	tetrablocks[2].append(L"..#.");
	tetrablocks[2].append(L"....");

	tetrablocks[3].append(L"....");		// shape D.
	tetrablocks[3].append(L".##.");
	tetrablocks[3].append(L".##.");
	tetrablocks[3].append(L"....");

	tetrablocks[4].append(L"..#.");		// shape E.
	tetrablocks[4].append(L".##.");
	tetrablocks[4].append(L"..#.");
	tetrablocks[4].append(L"....");

	tetrablocks[5].append(L"....");		// shape F.
	tetrablocks[5].append(L".##.");
	tetrablocks[5].append(L"..#.");
	tetrablocks[5].append(L"..#.");

	tetrablocks[6].append(L"....");		// shape G.
	tetrablocks[6].append(L".##.");
	tetrablocks[6].append(L".#..");
	tetrablocks[6].append(L".#..");

	// initialize the game's Field.
	pField = new unsigned char[Field_Width * Field_Height];	// buffer for the game's Field.

	for (int x = 0; x < Field_Width; x++) {					// Field boundary.
		for (int y = 0; y < Field_Height; y++) {
			pField[y * Field_Width + x] = (x == 0 || x == Field_Width - 1 || y == Field_Height - 1) ? 9 : 0;
			// Field blocks are all empty(position = 0) unless block is at the border(position = 9). 
		}
	}

	// initialize game's console screen.
	// creates an array of blank wchars for the screen and give a handle to the screen's buffer and activates it.
	// Windows handles are the pathway between user space and the OS. Various attributes have been assigned to our handle. eg, read/write
	wchar_t *screen = new wchar_t[Screen_Width * Screen_Height];
	for (int i = 0; i < Screen_Width * Screen_Height; i++) screen[i] = L' ';
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	// variables/arrays for Game logic.
	bool GameOver = false;	// Game loop flag.

	int Current_Piece = 0;
	int Current_Rotation = 0;
	int CurrentX = Field_Width / 2;
	int CurrentY = 0;

	bool Key[4]{};		        // Game keys: left arraw, right arrow, down arrow, rotate.
	bool Rotate_Hold = false;	// Check if rotate key is on hold. False by default.

	int Fall_Speed = 15;		// Fall speed of blocks. On level 1, it takes 15 game ticks for block to reach ground.
	int Fall_Speed_Counter = 0; // Counter to increase Fall speed as levels progress.
	bool Push_Down = false;     // when no. of game ticks = game speed, this bool becomes true and block is pushed down.
	int Piece_count = 0;		// no. of blocks used up.
	int score = 0;				// score.
	vector<int> Lines;			// Vector to store lines to be removed.		

	while (!GameOver) {
		// Set Game timing.
		delay(20);	// single game tick = 20ms.
		Fall_Speed_Counter++;
		Push_Down = (Fall_Speed_Counter == Fall_Speed); // when no of game ticks = game speed, pushed down becomes true.

		// Handle user input.
		for (int k = 0; k < 4; k++)								// R, L,  D, Z 	
			Key[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;
		// Goes through the keys array, checks the pressed key by GetAsyncKeyState fn.  
		// x27 hex for right arrow,x25 for left,x28 for down,Z for z.

	// Implement Game Logic(shapes falling, scoring, collision detection, etc).
	// Implement behaviour for keys
		if (Key[1]) {		// left arrow.
			// check if tetrablock fits if pushed left.
			if (Check_Block_Fitting(Current_Piece, Current_Rotation, CurrentX - 1, CurrentY)) {
				CurrentX = CurrentX - 1;
			}
		}

		if (Key[0]) {		// right arrow.
			// check if tetrablock fits if pushed right.
			if (Check_Block_Fitting(Current_Piece, Current_Rotation, CurrentX + 1, CurrentY)) {
				CurrentX = CurrentX + 1;
			}
		}

		if (Key[2]) {		// down arrow.
			// check if tetrablock fits if pushed down.
			if (Check_Block_Fitting(Current_Piece, Current_Rotation, CurrentX, CurrentY + 1)) {
				CurrentY = CurrentY + 1;
			}
		}

		if (Key[3]) {		// z = rotate.
			// check if tetrablock fits if pushed down.
			if (!Rotate_Hold && Check_Block_Fitting(Current_Piece, Current_Rotation + 1, CurrentX, CurrentY)) {
				Current_Rotation = Current_Rotation + 1;
				Rotate_Hold = true;
			}
			else {
				Rotate_Hold = false;
			}
		}

		// Implement Tetrablocks fall.
		if (Push_Down) {
			// increase game speed after every 10 blocks.
			Fall_Speed_Counter = 0;
			Piece_count++;
			if (Piece_count % 10 == 0)
				if (Fall_Speed >= 10) // check for minimum possible speed. 
					Fall_Speed = Fall_Speed - 5;

			// Check if piece can be pushed down.
			if (Check_Block_Fitting(Current_Piece, Current_Rotation, CurrentX, CurrentY + 1)) {
				CurrentY++;		// If fits, push down.
			}
			else {              // If doesn't fit, then:
				// Lock current piece in the field(deactivate it).
				for (int px = 0; px < 4; px++) {
					for (int py = 0; py < 4; py++) {
						// if current position of tetrablock is colliding with bottom, increment its Field value to 1.
						// 0 = empty space, 1 = inactive block in our Field. 
						if (tetrablocks[Current_Piece][Rotate_Block(px, py, Current_Rotation)] == L'#') 
							pField[(CurrentY + py) * Field_Width + (CurrentX + px)] = Current_Piece + 1;
					}
				}

				// Check if a horizontal line is made.
				for (int py = 0; py < 4; py++) {            // Checking near only where last block fell as line could have only appeared there.
					if (CurrentY + py < Field_Height - 1) {
						bool Line = true;							   // By default, horizontal line is assumed to be made.
						for (int px = 1; px < Field_Width - 1; px++) { // Restricting check within boundary.
							// bitwise_and field blocks and line underchecking, if any block is empty(resulted in 0), Line becomes false. 
							Line &= (pField[(CurrentY + py) * Field_Width + px]) != 0;
						}

						// If Line true then:
						if (Line) {
							// Set horizantal line to "=".
							for (int px = 1; px < Field_Width - 1; px++) {
								pField[(CurrentY + py) * Field_Width + px] = 8;

								// Remove horizontal line by destroying it and pushing the row above line downwards.
								Lines.push_back(CurrentY + py);
							}
						}
					}
				}

				// update score.
				score += 25;
				if (!Lines.empty())
					score += (1 << Lines.size() - 1) * 2;

				// Select next active piece.
				// Take block position to default and select new block at random.
				CurrentX = Field_Width / 2;
				CurrentY = 0;
				Current_Rotation = 0;
				Current_Piece = rand() % 7; // did %7 to get a number in the range 1-7(we have 7 shapes).

				// If piece does not fit, implement GAME OVER!
				GameOver = !Check_Block_Fitting(Current_Piece, Current_Rotation, CurrentX, CurrentY);
			}
			// Reset speed counter after block is deactivated.
			Fall_Speed_Counter = 0;
		}



		// Render output.

		// display or draw the field of the game.
		for (int x = 0; x < Field_Width; x++)
			for (int y = 0; y < Field_Height; y++)
				screen[(y + 2) * Screen_Width + (x + 2)] = L" ABCDEFG=X"[pField[y * Field_Width + x]];

		// display or draw current or active piece.
		for (int px = 0; px < 4; px++)
			for (int py = 0; py < 4; py++) 
				if (tetrablocks[Current_Piece][Rotate_Block(px, py, Current_Rotation)] == L'#')
					screen[(CurrentY + py + 2) * Screen_Width + (CurrentX + px + 2)] = Current_Piece + 65;

		// draw score.
		_snwprintf(&screen[2 * Screen_Width + Field_Width + 6], 16, L"Score: %8d", score);

		// Draw the frame of the game and refresh it continuously.
		if (!Lines.empty()) {
			// Draw frame.
			WriteConsoleOutputCharacterW(hConsole, screen, Screen_Width * Screen_Height, { 0, 0 }, &dwBytesWritten);

			delay(400);	// delay a bit before screen refresh.

			// Remove line and push the row above downwards.
			for (auto &v : Lines) 
				for (int px = 1; px < Field_Width - 1; px++) {
					for (int py = v; py > 0; py--) 
						pField[py * Field_Width + px] = pField[(py - 1) * Field_Width + px];
					pField[px] = 0;
				}
			
			Lines.clear();
		}
		// Display Frame.
		WriteConsoleOutputCharacterW(hConsole, screen, Screen_Width * Screen_Height, { 0, 0 }, &dwBytesWritten);
	}
	// Close program and display score and save it.
	CloseHandle(hConsole);
	std::cout << "Game Over! Score: " << score << endl; std::cout << "\nPrevious scores:\n";
	// Open file in append mode (ios::app) to add the new score
	ofstream outfile("example.txt", ios::app);
	if (!outfile) {
		std::cout << "Error: Unable to open the file." << endl;
		return 1;
	}
	outfile << score << endl;
	outfile.close();

	// Reopen the file in read mode to display the scores
	ifstream infile("example.txt");
	if (!infile) {
		cerr << "Error: Unable to open the file." << endl;
		return 1;
	}

	string line;
	while (getline(infile, line)) {
		std::cout << line << endl;
	}
	infile.close();


	system("pause");
	return 0;
}

// define functions.
int Rotate_Block(int px, int py, int r) {
	switch (r % 4)
	{
	case (0):
		return py * 4 + px;			  // 0 degrees.
		break;
	case (1):
		return 12 + py - (px * 4);    // 90 degrees.
		break;
	case (2):
		return 15 - (py * 4) - px;    // 180 degrees.
		break;
	case (3):
		return 3 - py + (px * 4);     // 270 degrees.
		break;
	}
	return 0;
}

int Check_Block_Fitting(int tetrablock_index, int Block_Rotation, int posX, int posY) {
	for (int px = 0; px < 4; px++)
		for (int py = 0; py < 4; py++) {
			// Get ID of tetrablock.
			int pi = Rotate_Block(px, py, Block_Rotation);

			// Get ID into Game Field.
			int fi = (posY + py) * Field_Width + (posX + px);

			// implement collision detection.
			if (posX + px >= 0 && posX + px < Field_Width) {
				// if tetrablock index = # and field block != 0(0 = empty), then tetrablock is colliding with something
				if (tetrablocks[tetrablock_index][pi] == L'#' && pField[fi] != 0)
					return false;	// fail on first hit.
			}
		}

	return true;
}

void delay(int milliseconds) {
    clock_t start_time = clock();
    while (clock() < start_time + milliseconds * (CLOCKS_PER_SEC / 1000));
} 