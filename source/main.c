// Simple citro2d untextured shape example
#include <citro2d.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define RESOLUTION    10
#define SCREEN_WIDTH  400
#define SCREEN_HEIGHT 240
#define ROWS          SCREEN_HEIGHT / RESOLUTION
#define COLUMNS       SCREEN_WIDTH / RESOLUTION
#define COLOR_LIVE    C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF)
#define COLOR_DEAD    C2D_Color32(0x00, 0x00, 0x00, 0xFF)

int cells[ROWS][COLUMNS];
int previous[ROWS][COLUMNS];

void initCells() {
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLUMNS; j++) {
			cells[i][j] = 0;
		}
	}
}

void randomizeCells() {
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLUMNS; j++) {
			int cell = rand() % (10 + 1);

			if (cell > 9) {
				cells[i][j] = 1;
			}
		}
	}
}

void draw() {
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLUMNS; j++) {
			if (cells[i][j] == 0) {
				C2D_DrawRectangle(j * RESOLUTION, i * RESOLUTION, 0, RESOLUTION, RESOLUTION, COLOR_DEAD, COLOR_DEAD, COLOR_DEAD, COLOR_DEAD);
			} else {
				C2D_DrawRectangle(j * RESOLUTION, i * RESOLUTION, 0, RESOLUTION, RESOLUTION, COLOR_LIVE, COLOR_LIVE, COLOR_LIVE, COLOR_LIVE);
			}
		}
	}
}

int getNeighbourValue(int row, int column) {
	if (row < 0 || row >= ROWS ) { return 0; }
    if (column < 0 || column >= COLUMNS ) { return 0; }
	return (previous[row][column] == 1) ? 1 : 0;
}

int countNeighbours(int row, int column) {
	int sum = 0;

	sum += getNeighbourValue(row - 1, column - 1);
	sum += getNeighbourValue(row - 1, column);
	sum += getNeighbourValue(row - 1, column + 1);
	sum += getNeighbourValue(row + 1, column - 1);
	sum += getNeighbourValue(row + 1, column);
	sum += getNeighbourValue(row + 1, column + 1);
	sum += getNeighbourValue(row, column - 1);
	sum += getNeighbourValue(row, column + 1);

	return sum;
}

void step() {
	memcpy(&previous, &cells, sizeof(cells));

	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLUMNS; j++) {
			int neighbours = countNeighbours(i, j);

			if (previous[i][j] == 0 && neighbours == 3) {
				cells[i][j] = 1;
			} else if (previous[i][j] == 1 && (neighbours < 2 || neighbours > 3)) {
				cells[i][j] = 0;
			}
		}
	}

	draw();
}

//---------------------------------------------------------------------------------
int main(int argc, char* argv[]) {
//---------------------------------------------------------------------------------
	srand(time(NULL));

	// Init libs
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();
	consoleInit(GFX_BOTTOM, NULL);

	// Create screens
	C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);

	// Create colors
	u32 clrClear = C2D_Color32(0xFF, 0xD8, 0xB0, 0x68);

	// Initialize & randomize first cells states
	initCells();
	randomizeCells();

	// Main loop
	while (aptMainLoop())
	{
		hidScanInput();

		// Respond to user input
		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu
		if (kDown & KEY_A)
			randomizeCells();

		printf("\x1b[1;1HGame of Life by AuditeMarlow");
		printf("\x1b[2;1HCPU:     %6.2f%%\x1b[K", C3D_GetProcessingTime()*6.0f);
		printf("\x1b[3;1HGPU:     %6.2f%%\x1b[K", C3D_GetDrawingTime()*6.0f);
		printf("\x1b[4;1HCmdBuf:  %6.2f%%\x1b[K", C3D_GetCmdBufUsage()*100.0f);
		printf("\x1b[5;1HCells:");

		// Render the scene
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(top, clrClear);
		C2D_SceneBegin(top);

		step();

		C3D_FrameEnd(0);
	}

	// Deinit libs
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	return 0;
}
