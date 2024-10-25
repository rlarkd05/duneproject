/*
* raw(?) I/O
*/
#include "io.h"

void gotoxy(POSITION pos) {
	COORD coord = { pos.column, pos.row }; // 행, 열 반대로 전달
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void set_color(int color) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void printc(POSITION pos, char ch, int color) {
	if (color >= 0) {
		set_color(color);
	}
	gotoxy(pos);
	printf("%c", ch);
}

KEY get_key(void) {
	if (!_kbhit()) {  // 입력된 키가 있는지 확인
		return k_none;  // 입력이 없을 경우
	}

	int byte = _getch();    // 입력된 키를 전달 받기
	switch (byte) {
	case 'q':
		return k_quit;  // 'q'를 누르면 종료
	case 224:
		byte = _getch();  // MSB 224가 입력되면 1바이트 더 전달 받기
		switch (byte) {
		case 72: return k_up;    // 위 방향키
		case 75: return k_left;  // 왼쪽 방향키
		case 77: return k_right; // 오른쪽 방향키
		case 80: return k_down;  // 아래 방향키
		default: return k_unknown; // 알 수 없는 키
		}
	default:
		// 알 수 없는 키에 대한 처리
		if (byte == ' ') return k_select; // 스페이스바
		if (byte == 27) return k_cancel; // ESC 키
		return k_unknown; // 그 외의 알 수 없는 키
	}
}
