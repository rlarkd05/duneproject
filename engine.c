#include <stdlib.h>
#include <time.h>
#include <stdio.h>
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
void cursor_double_move(DIRECTION dir, int times);
void sample_obj_move(void);
void cursor_select(void);
void StatusWindow(void);
void cursor_select(void);
void system_message(void);
void command_message(void);
POSITION sample_obj_next_position(void);


/* ================= control =================== */
int sys_clock = 0;
CURSOR cursor = { { 1, 1 }, {1, 1} };
int last_key_time = 0;
#define DOUBLE_PRESS_INTERVAL 200

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
	.speed = 300,
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
	cursor_select();
	StatusWindow();
	system_message();
	command_message();
	display(resource, map, cursor);

	while (1) {
		// loop 돌 때마다(즉, TICK==10ms마다) 키 입력 확인
		KEY key = get_key();

		// 키 입력이 있으면 처리
		if (is_arrow_key(key)) {
			DIRECTION dir = ktod(key);
			cursor_move(dir); // 기존 커서 이동 함수 호출
		}
		else {
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

void cursor_double_move(DIRECTION dir, int times) {
	for (int i = 0; i < times; i++) {
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

//상태창
void StatusWindow() {
	POSITION pos;

	// 왼쪽 세로 테두리 출력
	for (int row = 1; row < MAP_HEIGHT + 1; row++) {
		pos.row = row;
		pos.column = MAP_WIDTH + 2; // 맵 오른쪽에 상태창 위치 설정
		gotoxy(pos);
		printf("#");
	}

	// 상단 가로 테두리 출력
	for (int col = MAP_WIDTH + 2; col < MAP_WIDTH + 60; col++) {
		pos.row = 1;
		pos.column = col;
		gotoxy(pos);
		printf("#");
	}

	// 하단 가로 테두리 출력
	for (int col = MAP_WIDTH + 2; col < MAP_WIDTH + 60; col++) {
		pos.row = MAP_HEIGHT;
		pos.column = col;
		gotoxy(pos);
		printf("#");
	}

	// 오른쪽 세로 테두리 출력
	for (int row = 1; row < MAP_HEIGHT + 1; row++) {
		pos.row = row;
		pos.column = MAP_WIDTH + 59; // 오른쪽 벽 위치
		gotoxy(pos);
		printf("#");
	}
}

//시스템 메시지
void system_message() {
	// 메시지 창 높이와 시작 위치 설정
	POSITION pos;
	int message_start_row = MAP_HEIGHT + 2; // 시스템 메시지 창의 시작 행
	int message_width = MAP_WIDTH - 1;          // 시스템 메시지 창 너비
	int message_height = 5;                 // 시스템 메시지 창 높이

	// 왼쪽 세로 테두리 출력
	for (int row = message_start_row; row < message_start_row + message_height; row++) {
		pos.row = row;
		pos.column = 0; // 맨 왼쪽 테두리 위치
		gotoxy(pos);
		printf("#");
	}

	// 상단 가로 테두리 출력
	for (int col = 0; col < message_width + 1; col++) {
		pos.row = message_start_row;
		pos.column = col;
		gotoxy(pos);
		printf("#");
	}

	// 하단 가로 테두리 출력
	for (int col = 0; col < message_width + 1; col++) {
		pos.row = message_start_row + message_height - 1;
		pos.column = col;
		gotoxy(pos);
		printf("#");
	}

	// 오른쪽 세로 테두리 출력
	for (int row = message_start_row; row < message_start_row + message_height; row++) {
		pos.row = row;
		pos.column = message_width;
		gotoxy(pos);
		printf("#");
	}
}

//명령창
void command_message() {
	POSITION pos;
	int command_start_row = MAP_HEIGHT + 2;  // 명령창의 시작 행 (상태창 바로 아래)
	int command_width = 58;                 // 명령창 너비 (상태창 너비와 동일)
	int command_height = 5;                 // 명령창 높이

	// 왼쪽 세로 테두리 출력
	for (int row = command_start_row; row < command_start_row + command_height; row++) {
		pos.row = row;
		pos.column = MAP_WIDTH + 2;         // 상태창 오른쪽과 일치하게 위치 설정
		gotoxy(pos);
		printf("#");
	}

	// 상단 가로 테두리 출력
	for (int col = MAP_WIDTH + 2; col < MAP_WIDTH + 2 + command_width; col++) {
		pos.row = command_start_row;
		pos.column = col;
		gotoxy(pos);
		printf("#");
	}

	// 하단 가로 테두리 출력
	for (int col = MAP_WIDTH + 2; col < MAP_WIDTH + 2 + command_width; col++) {
		pos.row = command_start_row + command_height - 1;
		pos.column = col;
		gotoxy(pos);
		printf("#");
	}

	// 오른쪽 세로 테두리 출력
	for (int row = command_start_row; row < command_start_row + command_height; row++) {
		pos.row = row;
		pos.column = MAP_WIDTH + 2 + command_width - 1;
		gotoxy(pos);
		printf("#");
	}
}

// (가능하다면) 지정한 방향으로 커서 이동
void cursor_move(DIRECTION dir) {
	POSITION curr = cursor.current;
	POSITION new_pos = pmove(curr, dir);

	// 현재 시간에서 연속 입력 여부를 확인
	int time_diff = sys_clock - last_key_time;
	last_key_time = sys_clock; // Update last key time

	// 연속 입력 시 이동 거리 결정
	int move_distance = (time_diff < DOUBLE_PRESS_INTERVAL) ? 2 : 1; // 2칸 이동 또는 1칸 이동

	for (int i = 0; i < move_distance; i++) {
		// validation check (맵의 유효한 영역 내에서만 이동)
		if (1 <= new_pos.row && new_pos.row <= MAP_HEIGHT - 2 &&
			1 <= new_pos.column && new_pos.column <= MAP_WIDTH - 2) {

			// 현재 위치를 이전 위치로 업데이트
			cursor.previous = cursor.current; // Update previous position
			cursor.current = new_pos; // Update current position

			// 새로운 위치로 이동
			new_pos = pmove(new_pos, dir); // Calculate new position
		}
		else {
			break; // If new position is out of bounds, exit loop
		}
	}
}

// 오브젝트 선택 기능
void cursor_select(void) {
	POSITION pos = cursor.current;
	char obj_repr = map[1][pos.row][pos.column]; // 현재 위치의 오브젝트 표현

	if (obj_repr != -1 && obj_repr != ' ') {
		// 오브젝트가 있을 경우 상태 출력
		printf("Selected Object: %c\n", obj_repr);
		// 여기에 추가 상태 정보를 표시할 수 있음
		// 예: 특정 오브젝트의 상태를 가져와 출력
	}
	else {
		printf("Selected Terrain: Sand\n"); // 빈 지형 선택 시
	}
}

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

	obj.next_move_time = sys_clock + obj.speed;
}