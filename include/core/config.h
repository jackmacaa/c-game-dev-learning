#ifndef CONFIG_H
#define CONFIG_H

#define SCREEN_WIDTH 1440
#define SCREEN_HEIGHT 900
#define TARGET_FPS 60

#define PLAYER_TEXTURE_PATH "assets/walk-sheet.png"
#define PLAYER_SPRITE_COLUMNS 6
#define PLAYER_SPRITE_ROWS 4

// 6x4 walk sheet mapping:
// row 0 = down, row 1 = left, row 2 = right, row 3 = up
// columns are walk animation frames.
#define PLAYER_DOWN_COL 0
#define PLAYER_DOWN_ROW 0
#define PLAYER_LEFT_COL 0
#define PLAYER_LEFT_ROW 1
#define PLAYER_RIGHT_COL 0
#define PLAYER_RIGHT_ROW 2
#define PLAYER_UP_COL 0
#define PLAYER_UP_ROW 3

#endif // CONFIG_H
