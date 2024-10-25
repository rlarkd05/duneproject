
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
	.repr = 'P',  // E는 적군 장판을 나타내는 문자
	.layer = 0
};

OBJECT_BUILDING ally_spice = {
	.pos1 = {13, 1},  // 아군 스파이스 위치
	.repr = '5',      // 스파이스를 나타내는 문자
	.layer = 0        // 첫 번째 레이어
};

OBJECT_BUILDING enemy_spice = {
	.pos1 = {4, 58},
	.repr = '5', // 스파이스를 나타내는 문자
	.layer = 0   // 첫 번째 레이어
};

// 바위 구조체 정의
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
	.pos1 = {11, 5},  // 하나의 위치만 있는 바위도 구조체로 정의 가능
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
	OBJECT_BUILDING allay_base;       // 본진
	OBJECT_BUILDING enemy_base; //적진
	OBJECT_BUILDING allay_plate;
	OBJECT_BUILDING enemy_plate;      // 장판
	OBJECT_BUILDING dormitory;  // 숙소
	OBJECT_BUILDING garage;     // 창고
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
			// 방향키 더블클릭 체크 후 커서 이동
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
	// 아군 베이스
	map[ally_base.layer][ally_base.pos1.row][ally_base.pos1.column] = ally_base.repr;
	map[ally_base.layer][ally_base.pos2.row][ally_base.pos2.column] = ally_base.repr;
	map[ally_base.layer][ally_base.pos3.row][ally_base.pos3.column] = ally_base.repr;
	map[ally_base.layer][ally_base.pos4.row][ally_base.pos4.column] = ally_base.repr;

	// 적군 베이스
	map[enemy_base.layer][enemy_base.pos1.row][enemy_base.pos1.column] = enemy_base.repr;
	map[enemy_base.layer][enemy_base.pos2.row][enemy_base.pos2.column] = enemy_base.repr;
	map[enemy_base.layer][enemy_base.pos3.row][enemy_base.pos3.column] = enemy_base.repr;
	map[enemy_base.layer][enemy_base.pos4.row][enemy_base.pos4.column] = enemy_base.repr;

	// 아군 장판
	map[ally_pad.layer][ally_pad.pos1.row][ally_pad.pos1.column] = ally_pad.repr;
	map[ally_pad.layer][ally_pad.pos2.row][ally_pad.pos2.column] = ally_pad.repr;
	map[ally_pad.layer][ally_pad.pos3.row][ally_pad.pos3.column] = ally_pad.repr;
	map[ally_pad.layer][ally_pad.pos4.row][ally_pad.pos4.column] = ally_pad.repr;

	//적군 장판
	map[enemy_pad.layer][enemy_pad.pos1.row][enemy_pad.pos1.column] = enemy_pad.repr;
	map[enemy_pad.layer][enemy_pad.pos2.row][enemy_pad.pos2.column] = enemy_pad.repr;
	map[enemy_pad.layer][enemy_pad.pos3.row][enemy_pad.pos3.column] = enemy_pad.repr;
	map[enemy_pad.layer][enemy_pad.pos4.row][enemy_pad.pos4.column] = enemy_pad.repr;

	//아군 스파이스
	map[ally_spice.layer][ally_spice.pos1.row][ally_spice.pos1.column] = ally_spice.repr;

	//적군 스파이스
	map[enemy_spice.layer][enemy_spice.pos1.row][enemy_spice.pos1.column] = enemy_spice.repr;


	// 적 장판이나 추가 오브젝트도 동일한 방식으로 추가 가능
}

void Biome(void) {
	// 바위 1 추가
	map[ROCK_1.layer][ROCK_1.pos1.row][ROCK_1.pos1.column] = ROCK_1.repr;
	map[ROCK_1.layer][ROCK_1.pos2.row][ROCK_1.pos2.column] = ROCK_1.repr;
	map[ROCK_1.layer][ROCK_1.pos3.row][ROCK_1.pos3.column] = ROCK_1.repr;
	map[ROCK_1.layer][ROCK_1.pos4.row][ROCK_1.pos4.column] = ROCK_1.repr;

	// 바위 2 추가
	map[ROCK_2.layer][ROCK_2.pos1.row][ROCK_2.pos1.column] = ROCK_2.repr;
	map[ROCK_2.layer][ROCK_2.pos2.row][ROCK_2.pos2.column] = ROCK_2.repr;
	map[ROCK_2.layer][ROCK_2.pos3.row][ROCK_2.pos3.column] = ROCK_2.repr;
	map[ROCK_2.layer][ROCK_2.pos4.row][ROCK_2.pos4.column] = ROCK_2.repr;

	// 바위 3 추가
	map[ROCK_3.layer][ROCK_3.pos1.row][ROCK_3.pos1.column] = ROCK_3.repr;

	// 바위 4 추가
	map[ROCK_4.layer][ROCK_4.pos1.row][ROCK_4.pos1.column] = ROCK_4.repr;

	// 바위 5 추가
	map[ROCK_5.layer][ROCK_5.pos1.row][ROCK_5.pos1.column] = ROCK_5.repr;
}



void init(void) {
	// layer 0(map[0])에 지형 생성
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

	// layer 1(map[1])은 비워 두기(-1로 채움)
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			map[1][i][j] = -1;
		}
	}

	// object sample
	map[1][obj.pos.row][obj.pos.column] = 'o';
}

// (가능하다면) 지정한 방향으로 커서 이동
void cursor_move(DIRECTION direction) {
	// 커서의 현재 위치를 기반으로 방향에 따라 이동
	POSITION new_position = pmove(cursor.current, direction);

	// 이동할 위치가 맵의 범위 내에 있는지 확인
	if (new_position.row >= 0 && new_position.row < MAP_HEIGHT &&
		new_position.column >= 0 && new_position.column < MAP_WIDTH) {
		cursor.previous = cursor.current; // 이전 위치 업데이트
		cursor.current = new_position; // 새로운 위치로 업데이트
	}
}



void handle_input(KEY key, CURSOR* cursor, SELECTION* selection) {
	int layer = 0; // 현재 레이어는 0으로 설정
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
		// 선택된 오브젝트 처리
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
	// 오른쪽 상단 위치(가령 x=70, y=0)를 설정
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

/* 상태창 초기화 */
void clear_status(void) {
	printf("Status Cleared\n");
}

/* ================= sample object movement =================== */
POSITION sample_obj_next_position(void) {
	// 현재 위치와 목적지를 비교해서 이동 방향 결정	
	POSITION diff = psub(obj.dest, obj.pos);
	DIRECTION dir;

	// 목적지 도착. 지금은 단순히 원래 자리로 왕복
	if (diff.row == 0 && diff.column == 0) {
		if (obj.dest.row == 1 && obj.dest.column == 1) {
			// topleft --> bottomright로 목적지 설정
			POSITION new_dest = { MAP_HEIGHT - 2, MAP_WIDTH - 2 };
			obj.dest = new_dest;
		}
		else {
			// bottomright --> topleft로 목적지 설정
			POSITION new_dest = { 1, 1 };
			obj.dest = new_dest;
		}
		return obj.pos;
	}

	// 가로축, 세로축 거리를 비교해서 더 먼 쪽 축으로 이동
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}

	// validation check
	// next_pos가 맵을 벗어나지 않고, (지금은 없지만)장애물에 부딪히지 않으면 다음 위치로 이동
	// 지금은 충돌 시 아무것도 안 하는데, 나중에는 장애물을 피해가거나 적과 전투를 하거나... 등등
	POSITION next_pos = pmove(obj.pos, dir);
	if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] < 0) {

		return next_pos;
	}
	else {
		return obj.pos;  // 제자리
	}
}

void sample_obj_move(void) {
	if (sys_clock <= obj.next_move_time) {
		// 아직 시간이 안 됐음w
		return;
	}

	// 오브젝트(건물, 유닛 등)은 layer1(map[1])에 저장ddd
	map[1][obj.pos.row][obj.pos.column] = -1;
	obj.pos = sample_obj_next_position();
	map[1][obj.pos.row][obj.pos.column] = obj.repr;

	obj.next_move_time = sys_clock + obj.move_period;
}