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
void sample_obj_move(void);
POSITION sample_obj_next_position(void);


/* ================= control =================== */
int sys_clock = 0;		// system-wide clock(ms)
CURSOR cursor = { { 1, 1 }, {1, 1} };
int last_key_time = 0;    // ������ Ű �Է� �ð�
#define DOUBLE_PRESS_INTERVAL 200  // ���� �Է� �ð� ����(ms)

/* ================= game data =================== */
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH] = { 0 };

RESOURCE resource = {
	.spice = 0,
	.spice_max = 0,
	.population = 0,
	.population_max = 0
};

OBJECT_SAMPLE obj = {
	.pos = {1, 1},
	.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
	.repr = 'o',
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



/* ================= main() =================== */
int main(void) {
	srand((unsigned int)time(NULL));

	intro();
	init();
	Construction();
	Biome();

	display(resource, map, cursor);

	while (1) {
		// loop �� ������(��, TICK==10ms����) Ű �Է� Ȯ��
		KEY key = get_key();

		// Ű �Է��� ������ ó��
		if (is_arrow_key(key)) {
			cursor_move(ktod(key));
		}
		else {
			// ����Ű ���� �Է�
			switch (key) {
			case k_quit: outro();
			case k_none:
			case k_undef:
			default: break;
			}
		}

		// ���� ������Ʈ ����
		sample_obj_move();

		// ȭ�� ���
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
void cursor_move(DIRECTION dir) {
	POSITION curr = cursor.current;
	POSITION new_pos = pmove(curr, dir);

	// ���� �ð����� ���� �Է� ���θ� Ȯ��
	int time_diff = sys_clock - last_key_time;
	int move_distance = (time_diff < DOUBLE_PRESS_INTERVAL) ? 3 : 1; // ���� �Է� �� 2ĭ �̵�

	for (int i = 0; i < move_distance; i++) {
		// validation check (���� ��ȿ�� ���� �������� �̵�)
		if (1 <= new_pos.row && new_pos.row <= MAP_HEIGHT - 2 &&
			1 <= new_pos.column && new_pos.column <= MAP_WIDTH - 2) {

			cursor.previous = cursor.current;
			cursor.current = new_pos;
			new_pos = pmove(new_pos, dir); // ���� �̵��� ���� ��ġ ����
		}
		else {
			break; // �� ��踦 �Ѿ�� �ݺ� ����
		}
	}
	// ������ �Է� �ð��� ���� �ð����� ������Ʈ
	last_key_time = sys_clock;
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
		// ���� �ð��� �� ����
		return;
	}

	// ������Ʈ(�ǹ�, ���� ��)�� layer1(map[1])�� ����
	map[1][obj.pos.row][obj.pos.column] = -1;
	obj.pos = sample_obj_next_position();
	map[1][obj.pos.row][obj.pos.column] = obj.repr;

	obj.next_move_time = sys_clock + obj.move_period;
}