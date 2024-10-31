/*
*  display.c:
* 화면에 게임 정보를 출력
* 맵, 커서, 시스템 메시지, 정보창, 자원 상태 등등
* io.c에 있는 함수들을 사용함
*/

#include "display.h"
#include "io.h"

// 출력할 내용들의 좌상단(topleft) 좌표
const POSITION resource_pos = { 0, 0 };
const POSITION map_pos = { 1, 0 };


char backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };
char frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };

void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]);
void display_resource(RESOURCE resource);
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
void display_cursor(CURSOR cursor);


void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
	CURSOR cursor)
{
	display_resource(resource);
	display_map(map);
	display_cursor(cursor);
	// display_system_message()
	// display_object_info()
	// display_commands()
	// ...
}

void display_resource(RESOURCE resource) {
	set_color(COLOR_RESOURCE);
	gotoxy(resource_pos);
	printf("spice = %d/%d, population=%d/%d\n",
		resource.spice, resource.spice_max,
		resource.population, resource.population_max
	);
}

// subfunction of draw_map()
void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]) {
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			dest[i][j] = ' '; // 초기값 설정
			for (int k = 0; k < N_LAYER; k++) {
				if (src[k][i][j] >= 0) { // 유효한 문자일 때만
					dest[i][j] = src[k][i][j]; // 가장 위에 있는 레이어의 값 저장
					break; // 첫 번째로 발견된 문자만 사용
				}
			}
		}
	}
}

int get_color_at(POSITION pos) {
	char ch = backbuf[pos.row][pos.column];

	// 문자에 따른 색상 반환
	switch (ch) {
	case 'B':
		return (pos.row >= 15 && pos.column <= 2) ? COLOR_BLUE : COLOR_RED;
	case '5':
		return COLOR_ORANGE;
	case 'P':
		return COLOR_BLACK;
	case 'R':
		return COLOR_GRAY;
	case '#':
		return COLOR_BLACK;
	case 'o':
		return COLOR_YELLOW;
	default:
		return COLOR_DEFAULT; // 기본 색상
	}
}


void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	project(map, backbuf); // 새로운 맵을 backbuf에 프로젝트합니다.

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (frontbuf[i][j] != backbuf[i][j]) { // 변경된 부분을 확인합니다.
				POSITION pos = { i, j };
				char current_char = backbuf[i][j];

				// 초기값을 외부에서 설정
				int color = get_color_at(pos); // 현재 문자의 색상을 가져옵니다.

				// 변경된 부분만 화면에 출력
				printc(padd(map_pos, pos), current_char, color);
				frontbuf[i][j] = backbuf[i][j]; // frontbuf 업데이트
			}
		}
	}
}







// frontbuf[][]에서 커서 위치의 문자를 색만 바꿔서 그대로 다시 출력
void display_cursor(CURSOR cursor) {
	POSITION prev = cursor.previous;
	POSITION curr = cursor.current;

	// 이전 위치를 '빈 공간'으로 설정
	char prev_char = frontbuf[prev.row][prev.column];
	printc(padd(map_pos, prev), ' ', COLOR_DEFAULT); // 빈 공간으로 업데이트

	// 현재 위치에 커서 표시
	char curr_char = frontbuf[curr.row][curr.column];
	printc(padd(map_pos, curr), curr_char, COLOR_CURSOR);
}