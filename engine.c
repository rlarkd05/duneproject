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
int last_key_time = 0;    // 마지막 키 입력 시간
#define DOUBLE_PRESS_INTERVAL 200  // 연속 입력 시간 간격(ms)

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



/* ================= main() =================== */
int main(void) {
	srand((unsigned int)time(NULL));

	intro();
	init();
	Construction();
	Biome();

	display(resource, map, cursor);

	while (1) {
		// loop 돌 때마다(즉, TICK==10ms마다) 키 입력 확인
		KEY key = get_key();

		// 키 입력이 있으면 처리
		if (is_arrow_key(key)) {
			cursor_move(ktod(key));
		}
		else {
			// 방향키 외의 입력
			switch (key) {
			case k_quit: outro();
			case k_none:
			case k_undef:
			default: break;
			}
		}

		// 샘플 오브젝트 동작
		sample_obj_move();

		// 화면 출력
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
void cursor_move(DIRECTION dir) {
	POSITION curr = cursor.current;
	POSITION new_pos = pmove(curr, dir);

	// 현재 시간에서 연속 입력 여부를 확인
	int time_diff = sys_clock - last_key_time;
	int move_distance = (time_diff < DOUBLE_PRESS_INTERVAL) ? 3 : 1; // 연속 입력 시 2칸 이동

	for (int i = 0; i < move_distance; i++) {
		// validation check (맵의 유효한 영역 내에서만 이동)
		if (1 <= new_pos.row && new_pos.row <= MAP_HEIGHT - 2 &&
			1 <= new_pos.column && new_pos.column <= MAP_WIDTH - 2) {

			cursor.previous = cursor.current;
			cursor.current = new_pos;
			new_pos = pmove(new_pos, dir); // 다음 이동을 위한 위치 갱신
		}
		else {
			break; // 맵 경계를 넘어가면 반복 종료
		}
	}
	// 마지막 입력 시간을 현재 시간으로 업데이트
	last_key_time = sys_clock;
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
		// 아직 시간이 안 됐음
		return;
	}

	// 오브젝트(건물, 유닛 등)은 layer1(map[1])에 저장
	map[1][obj.pos.row][obj.pos.column] = -1;
	obj.pos = sample_obj_next_position();
	map[1][obj.pos.row][obj.pos.column] = obj.repr;

	obj.next_move_time = sys_clock + obj.move_period;
}