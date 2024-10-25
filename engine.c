
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "common.h"
#include "io.h"
#include "display.h"

void init(void);
void intro(void);
void Construction(void); 
void Biome(void);
void outro(void);
void cursor_move(DIRECTION dir);
void double_click_cursor_move(DIRECTION dir);
void handle_input(KEY key, CURSOR* cursor, SELECTION* selection);
void display_status(const CURSOR* cursor, const SELECTION* selection);
void clear_status(void);
void sample_obj_move(void);
POSITION sample_obj_next_position(void);


/* ================= control =================== */
int sys_clock = 0;		// system-wide clock(ms)
CURSOR cursor = { { 1, 1 }, {1, 1} };
SELECTION selection;


/* ================= game data =================== */
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH] = { 0 };

RESOURCE resource = {
	.spice = 0,
	.spice_max = 0,
	.population = 0,
	.population_max = 0
};

OBJECT_WORM obj = {
	.pos = {3, 5},
	.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
	.repr = 'W',
	.move_period = 300,
	.next_move_time = 300
};

OBJECT_BUILDING ally_base = {
	.pos1 = {15, 1},
	.pos2 = {15, 2},
	.pos3 = {16, 1},
	.pos4 = {16, 2},
	.repr = 'B',
	.layer = 0
};

OBJECT_BUILDING enemy_base = {
	.pos1 = {1, 58},
	.pos2 = {1, 57},
	.pos3 = {2, 58},
	.pos4 = {2, 57},
	.repr = 'B',
	.layer = 0
};

OBJECT_BUILDING ally_pad = {
	.pos1 = {15, 3},
	.pos2 = {15, 4},
	.pos3 = {16, 3},
	.pos4 = {16, 4},
	.repr = 'P',
	.layer = 0
};

OBJECT_BUILDING enemy_pad = {
	.pos1 = {2, 55},
	.pos2 = {2, 56},
	.pos3 = {1, 55},
	.pos4 = {1, 56},
	.repr = 'P',  // E�� ���� ������ ��Ÿ���� ����
	.layer = 0
};

OBJECT_BUILDING ally_spice = {
	.pos1 = {13, 1},  // �Ʊ� �����̽� ��ġ
	.repr = '5',      // �����̽��� ��Ÿ���� ����
	.layer = 0        // ù ��° ���̾�
};

OBJECT_BUILDING enemy_spice = {
	.pos1 = {4, 58},
	.repr = '5', // �����̽��� ��Ÿ���� ����
	.layer = 0   // ù ��° ���̾�
};

// ���� ����ü ����
OBJECT_BUILDING ROCK_1 = {
	.pos1 = {4, 25},
	.pos2 = {3, 25},
	.pos3 = {4, 26},
	.pos4 = {3, 26},
	.repr = 'R',
	.layer = 0
};

OBJECT_BUILDING ROCK_2 = {
	.pos1 = {12, 32},
	.pos2 = {13, 32},
	.pos3 = {12, 33},
	.pos4 = {13, 33},
	.repr = 'R',
	.layer = 0
};

OBJECT_BUILDING ROCK_3 = {
	.pos1 = {11, 5},  // �ϳ��� ��ġ�� �ִ� ������ ����ü�� ���� ����
	.repr = 'R',
	.layer = 0
};

OBJECT_BUILDING ROCK_4 = {
	.pos1 = {6, 48},
	.repr = 'R',
	.layer = 0
};

OBJECT_BUILDING ROCK_5 = {
	.pos1 = {13, 50},
	.repr = 'R',
	.layer = 0
};

typedef struct {
	OBJECT_BUILDING allay_base;       // ����
	OBJECT_BUILDING enemy_base; //����
	OBJECT_BUILDING allay_plate;
	OBJECT_BUILDING enemy_plate;      // ����
	OBJECT_BUILDING dormitory;  // ����
	OBJECT_BUILDING garage;     // â��
} COMMON_BUILDINGS;


/* ================= main() =================== */
int main(void) {
	srand((unsigned int)time(NULL));

	intro();
	init();
	Construction();
	Biome();

	display(resource, map, cursor);

	while (1) {
		KEY key = get_key();

		if (is_arrow_key(key)) {
			// ����Ű ����Ŭ�� üũ �� Ŀ�� �̵�
			static int last_key = -1;
			static int last_time = 0;

			if (key == last_key && sys_clock - last_time < 200) {
				double_click_cursor_move(ktod(key));
			}
			else {
				cursor_move(ktod(key));
			}
			last_key = key;
			last_time = sys_clock;
		}
		else {
			handle_input(key, &cursor, &selection);
		}

		sample_obj_move();
		display(resource, map, cursor);
		Sleep(TICK);
		sys_clock += 10;
	}
}


/* ================= subfunctions =================== */
void intro(void) {
	printf("DUNE 1.5\n");
	Sleep(2000);
	system("cls");
}

void outro(void) {
	printf("exiting...\n");
	exit(0);
}

void double_click_cursor_move(DIRECTION dir) {
	for (int i = 0; i < 3; i++) {
		cursor_move(dir);
	}
}

void Construction(void) {
	// �Ʊ� ���̽�
	map[ally_base.layer][ally_base.pos1.row][ally_base.pos1.column] = ally_base.repr;
	map[ally_base.layer][ally_base.pos2.row][ally_base.pos2.column] = ally_base.repr;
	map[ally_base.layer][ally_base.pos3.row][ally_base.pos3.column] = ally_base.repr;
	map[ally_base.layer][ally_base.pos4.row][ally_base.pos4.column] = ally_base.repr;

	// ���� ���̽�
	map[enemy_base.layer][enemy_base.pos1.row][enemy_base.pos1.column] = enemy_base.repr;
	map[enemy_base.layer][enemy_base.pos2.row][enemy_base.pos2.column] = enemy_base.repr;
	map[enemy_base.layer][enemy_base.pos3.row][enemy_base.pos3.column] = enemy_base.repr;
	map[enemy_base.layer][enemy_base.pos4.row][enemy_base.pos4.column] = enemy_base.repr;

	// �Ʊ� ����
	map[ally_pad.layer][ally_pad.pos1.row][ally_pad.pos1.column] = ally_pad.repr;
	map[ally_pad.layer][ally_pad.pos2.row][ally_pad.pos2.column] = ally_pad.repr;
	map[ally_pad.layer][ally_pad.pos3.row][ally_pad.pos3.column] = ally_pad.repr;
	map[ally_pad.layer][ally_pad.pos4.row][ally_pad.pos4.column] = ally_pad.repr;

	//���� ����
	map[enemy_pad.layer][enemy_pad.pos1.row][enemy_pad.pos1.column] = enemy_pad.repr;
	map[enemy_pad.layer][enemy_pad.pos2.row][enemy_pad.pos2.column] = enemy_pad.repr;
	map[enemy_pad.layer][enemy_pad.pos3.row][enemy_pad.pos3.column] = enemy_pad.repr;
	map[enemy_pad.layer][enemy_pad.pos4.row][enemy_pad.pos4.column] = enemy_pad.repr;

	//�Ʊ� �����̽�
	map[ally_spice.layer][ally_spice.pos1.row][ally_spice.pos1.column] = ally_spice.repr;

	//���� �����̽�
	map[enemy_spice.layer][enemy_spice.pos1.row][enemy_spice.pos1.column] = enemy_spice.repr;


	// �� �����̳� �߰� ������Ʈ�� ������ ������� �߰� ����
}

void Biome(void) {
	// ���� 1 �߰�
	map[ROCK_1.layer][ROCK_1.pos1.row][ROCK_1.pos1.column] = ROCK_1.repr;
	map[ROCK_1.layer][ROCK_1.pos2.row][ROCK_1.pos2.column] = ROCK_1.repr;
	map[ROCK_1.layer][ROCK_1.pos3.row][ROCK_1.pos3.column] = ROCK_1.repr;
	map[ROCK_1.layer][ROCK_1.pos4.row][ROCK_1.pos4.column] = ROCK_1.repr;

	// ���� 2 �߰�
	map[ROCK_2.layer][ROCK_2.pos1.row][ROCK_2.pos1.column] = ROCK_2.repr;
	map[ROCK_2.layer][ROCK_2.pos2.row][ROCK_2.pos2.column] = ROCK_2.repr;
	map[ROCK_2.layer][ROCK_2.pos3.row][ROCK_2.pos3.column] = ROCK_2.repr;
	map[ROCK_2.layer][ROCK_2.pos4.row][ROCK_2.pos4.column] = ROCK_2.repr;

	// ���� 3 �߰�
	map[ROCK_3.layer][ROCK_3.pos1.row][ROCK_3.pos1.column] = ROCK_3.repr;

	// ���� 4 �߰�
	map[ROCK_4.layer][ROCK_4.pos1.row][ROCK_4.pos1.column] = ROCK_4.repr;

	// ���� 5 �߰�
	map[ROCK_5.layer][ROCK_5.pos1.row][ROCK_5.pos1.column] = ROCK_5.repr;
}



void init(void) {
	// layer 0(map[0])�� ���� ����
	for (int j = 0; j < MAP_WIDTH; j++) {
		map[0][0][j] = '#';
		map[0][MAP_HEIGHT - 1][j] = '#';
	}

	for (int i = 1; i < MAP_HEIGHT - 1; i++) {
		map[0][i][0] = '#';
		map[0][i][MAP_WIDTH - 1] = '#';
		for (int j = 1; j < MAP_WIDTH - 1; j++) {
			map[0][i][j] = ' ';
		}
	}

	// layer 1(map[1])�� ��� �α�(-1�� ä��)
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			map[1][i][j] = -1;
		}
	}

	// object sample
	map[1][obj.pos.row][obj.pos.column] = 'o';
}

// (�����ϴٸ�) ������ �������� Ŀ�� �̵�
void cursor_move(DIRECTION direction) {
	// Ŀ���� ���� ��ġ�� ������� ���⿡ ���� �̵�
	POSITION new_position = pmove(cursor.current, direction);

	// �̵��� ��ġ�� ���� ���� ���� �ִ��� Ȯ��
	if (new_position.row >= 0 && new_position.row < MAP_HEIGHT &&
		new_position.column >= 0 && new_position.column < MAP_WIDTH) {
		cursor.previous = cursor.current; // ���� ��ġ ������Ʈ
		cursor.current = new_position; // ���ο� ��ġ�� ������Ʈ
	}
}



void handle_input(KEY key, CURSOR* cursor, SELECTION* selection) {
	int layer = 0; // ���� ���̾�� 0���� ����
	int row = cursor->current.row;
	int col = cursor->current.column;

	if (layer < 0 || layer >= N_LAYER || row < 0 || row >= MAP_HEIGHT || col < 0 || col >= MAP_WIDTH) {
		return;
	}

	switch (key) {
	case k_up:
		cursor_move(d_up);
		break;
	case k_down:
		cursor_move(d_down);
		break;
	case k_left:
		cursor_move(d_left);
		break;
	case k_right:
		cursor_move(d_right);
		break;

	case k_select:
		// ���õ� ������Ʈ ó��
		if (map[layer][row][col] != ' ' && map[layer][row][col] != -1) {
			if (selection->selected_object != NULL) {
				free(selection->selected_object);
			}

			OBJECT_BUILDING* obj = malloc(sizeof(OBJECT_BUILDING));
			if (obj != NULL) {
				obj->repr = map[layer][row][col];
				obj->pos1 = cursor->current;

				selection->selected_object = obj;
				selection->is_selected = true;
			}
		}
		else {
			if (selection->selected_object != NULL) {
				free(selection->selected_object);
			}
			selection->selected_object = NULL;
			selection->is_selected = false;
		}

		display_status(cursor, selection);
		break;

	case k_cancel:
		if (selection->selected_object != NULL) {
			free(selection->selected_object);
			selection->selected_object = NULL;
		}
		selection->is_selected = false;
		clear_status();
		break;

	default:
		break;
	}

	display_status(cursor, selection);
}


void set_cursor_position(int x, int y) {
	COORD coord = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void display_status(const CURSOR* cursor, const SELECTION* selection) {
	// ������ ��� ��ġ(���� x=70, y=0)�� ����
	set_cursor_position(70, 0);

	if (selection->is_selected) {
		if (selection->selected_object) {
			printf("Selected Object: %c   ", selection->selected_object->repr);
		}
		else {
			printf("Selected Terrain: Desert    ");
		}
	}
	else {
		printf("No Object Selected          ");
	}
}

/* ����â �ʱ�ȭ */
void clear_status(void) {
	printf("Status Cleared\n");
}

/* ================= sample object movement =================== */
POSITION sample_obj_next_position(void) {
	// ���� ��ġ�� �������� ���ؼ� �̵� ���� ����	
	POSITION diff = psub(obj.dest, obj.pos);
	DIRECTION dir;

	// ������ ����. ������ �ܼ��� ���� �ڸ��� �պ�
	if (diff.row == 0 && diff.column == 0) {
		if (obj.dest.row == 1 && obj.dest.column == 1) {
			// topleft --> bottomright�� ������ ����
			POSITION new_dest = { MAP_HEIGHT - 2, MAP_WIDTH - 2 };
			obj.dest = new_dest;
		}
		else {
			// bottomright --> topleft�� ������ ����
			POSITION new_dest = { 1, 1 };
			obj.dest = new_dest;
		}
		return obj.pos;
	}

	// ������, ������ �Ÿ��� ���ؼ� �� �� �� ������ �̵�
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}

	// validation check
	// next_pos�� ���� ����� �ʰ�, (������ ������)��ֹ��� �ε����� ������ ���� ��ġ�� �̵�
	// ������ �浹 �� �ƹ��͵� �� �ϴµ�, ���߿��� ��ֹ��� ���ذ��ų� ���� ������ �ϰų�... ���
	POSITION next_pos = pmove(obj.pos, dir);
	if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] < 0) {

		return next_pos;
	}
	else {
		return obj.pos;  // ���ڸ�
	}
}

void sample_obj_move(void) {
	if (sys_clock <= obj.next_move_time) {
		// ���� �ð��� �� ����w
		return;
	}

	// ������Ʈ(�ǹ�, ���� ��)�� layer1(map[1])�� ����ddd
	map[1][obj.pos.row][obj.pos.column] = -1;
	obj.pos = sample_obj_next_position();
	map[1][obj.pos.row][obj.pos.column] = obj.repr;

	obj.next_move_time = sys_clock + obj.move_period;
}