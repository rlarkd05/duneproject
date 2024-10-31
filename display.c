/*
*  display.c:
* ȭ�鿡 ���� ������ ���
* ��, Ŀ��, �ý��� �޽���, ����â, �ڿ� ���� ���
* io.c�� �ִ� �Լ����� �����
*/

#include "display.h"
#include "io.h"

// ����� ������� �»��(topleft) ��ǥ
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
			dest[i][j] = ' '; // �ʱⰪ ����
			for (int k = 0; k < N_LAYER; k++) {
				if (src[k][i][j] >= 0) { // ��ȿ�� ������ ����
					dest[i][j] = src[k][i][j]; // ���� ���� �ִ� ���̾��� �� ����
					break; // ù ��°�� �߰ߵ� ���ڸ� ���
				}
			}
		}
	}
}

int get_color_at(POSITION pos) {
	char ch = backbuf[pos.row][pos.column];

	// ���ڿ� ���� ���� ��ȯ
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
		return COLOR_DEFAULT; // �⺻ ����
	}
}


void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	project(map, backbuf); // ���ο� ���� backbuf�� ������Ʈ�մϴ�.

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (frontbuf[i][j] != backbuf[i][j]) { // ����� �κ��� Ȯ���մϴ�.
				POSITION pos = { i, j };
				char current_char = backbuf[i][j];

				// �ʱⰪ�� �ܺο��� ����
				int color = get_color_at(pos); // ���� ������ ������ �����ɴϴ�.

				// ����� �κи� ȭ�鿡 ���
				printc(padd(map_pos, pos), current_char, color);
				frontbuf[i][j] = backbuf[i][j]; // frontbuf ������Ʈ
			}
		}
	}
}







// frontbuf[][]���� Ŀ�� ��ġ�� ���ڸ� ���� �ٲ㼭 �״�� �ٽ� ���
void display_cursor(CURSOR cursor) {
	POSITION prev = cursor.previous;
	POSITION curr = cursor.current;

	// ���� ��ġ�� '�� ����'���� ����
	char prev_char = frontbuf[prev.row][prev.column];
	printc(padd(map_pos, prev), ' ', COLOR_DEFAULT); // �� �������� ������Ʈ

	// ���� ��ġ�� Ŀ�� ǥ��
	char curr_char = frontbuf[curr.row][curr.column];
	printc(padd(map_pos, curr), curr_char, COLOR_CURSOR);
}