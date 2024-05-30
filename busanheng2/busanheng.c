#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<windows.h>
#include<stdbool.h>
// 3-4
//파라미터
#define LEN_MIN 15
#define LEN_MAX 50
#define STM_MIN 0
#define STM_MAX 5
#define PROB_MIN 10
#define PROB_MAX 90
#define AGGRO_MIN 0
#define AGGRO_MAX 5

//마동석 이동 방향
#define MOVE_LEFT 1
#define MOVE_STAY 0

//좀비의 공격 대상
#define ATK_NONE 0
#define ATK_CITIZEN 1
#define ATK_DONGSEOK 2

//마동석 행동
#define ACTION_REST 0
#define ACTION_PROVO 1
#define ACTION_PULL 2

void input_value(int* variable, int lower_bound, int upper_bound, char* message) {
	while ((*variable < lower_bound) || (*variable > upper_bound)) {
		printf("%s", message);
		scanf_s("%d", variable);
	}
}
void print_train(int length, int* train) {
	for (int i = 0; i < length; i++) printf("#");
	printf("\n");
	for (int i = 0; i < length; i++) {
		if ((i == 0) || (i == length - 1)) printf("#");
		else if (train[i] == 1) printf("C");
		else if (train[i] == 2) printf("M");
		else if ((train[i] == 3) || (train[i] == 5)) printf("Z");
		else if (train[i] == 4) printf("V");
		else printf(" ");
	}
	printf("\n");
	for (int i = 0; i < length; i++) printf("#");
	printf("\n\n");
}
double random_double(double min, double max) {
	double range = (max - min);
	double div = RAND_MAX / range;
	return min + (rand() / div);
}
int count_citizen(int length, int* train) {
	int citizen_count = 0;
	for (int j = 0; j < length; j++) {
		if (train[j] == 1) citizen_count++;
	}
	return citizen_count;
}
int main(void) {
	printf("==========GAME START !==========\n");
	srand((unsigned int)time(NULL));

	int length = -1, probability = -1, stamina = -1;
	input_value(&length, LEN_MIN, LEN_MAX, "train length(15~50)>>");
	input_value(&stamina, STM_MIN, STM_MAX, "madongseok stamina(0~5)>>");
	input_value(&probability, PROB_MIN, PROB_MAX, "percentile probability 'p'(10~90)>>");

	for (int stage = 1; stage <= 4; stage++) {
		printf("==========STAGE %d==========\n\n", stage);
		int len_C = 1;
		int train[50] = { 0 };
		if (stage == 2) train[length - 5] = 4; //4=빌런
		if (stage > 2) {
			len_C = length / 4 + rand() % (length / 2 - length / 4 + 1);   // 랜덤으로 시민 만듦
			train[length - 5] = 1; //1=시민
		}
		else {
			train[length - 6] = 1; //1=시민
		}
		train[length - 2] = 2; //2=마동석
		train[length - 3] = 3; //3=좀비
		//2 ~ length-6
		for (int i = 0; i < len_C - 1; i++) {  //시민들 위치 랜덤 설정
			while (true) {
				int random_pos = 2 + rand() % (length - 6 - 2 + 1);
				if (train[random_pos] == 0) {
					train[random_pos] = 1;
					break;
				}
			}
		}
		int aggro[50] = { 0 };
		for (int i = 0; i < length; i++) {
			if ((train[i] == 1) || (train[i] == 2) || (train[i] == 4)) {
				aggro[i] = 1;
			}
		}
		bool pull_success = false;
		print_train(length, train);

		//3스테이지 변수
		int citizen_offset = 0;
		for (int turn = 1; ; turn++) {
			char log[256][256] = { 0 };
			int log_index = 0;
			int madongseok_pos = 0;
			//2스테이지 변수
			bool citizen_moved = false;
			bool vilian_moved = false;
			//3스테이지 변수
			int citizen_index = 1;
			int right_most_citizen_pos = -1;
			for (int i = 0; i < length; i++) {
				if (train[i] == 2) madongseok_pos = i;
			}
			for (int i = 0; i < length; i++) {
				//시민 이동
				if (train[i] == 1) {
					if (((random_double(0, 100) > probability) && (i > 0) && (train[i - 1] == 0)) || vilian_moved) {
						right_most_citizen_pos = i - 1;
						citizen_moved = true;
						aggro[i]++;
						sprintf_s(log[log_index], 256, "citizen%d:%d->%d (aggro: %d -> %d)\n", citizen_index + citizen_offset, i, i - 1, aggro[i] - 1, aggro[i]);
						if (aggro[i] > AGGRO_MAX) {
							aggro[i] = AGGRO_MAX;
							sprintf_s(log[log_index], 256, "citizen%d:%d->%d (aggro: %d)\n", citizen_index + citizen_offset, i, i - 1, aggro[i]);
						}
						train[i - 1] = 1;
						train[i] = 0;
						aggro[i - 1] = aggro[i];
						aggro[i] = 0;
					}
					else {
						right_most_citizen_pos = i;
						aggro[i]--;
						sprintf_s(log[log_index], 256, "citizen%d: stay %d (aggro: %d -> %d)\n", citizen_index + citizen_offset, i, aggro[i] + 1, aggro[i]);
						if (aggro[i] < AGGRO_MIN) {
							aggro[i] = AGGRO_MIN;
							sprintf_s(log[log_index], 256, "citizen%d: stay %d (aggro: %d)\n", citizen_index + citizen_offset, i, aggro[i]);
						}
					}
					citizen_index++;
					log_index++;
				}
				else if (train[i] == 4) { //빌런 이동
					if (((right_most_citizen_pos < 0) && (random_double(0, 100) > probability)) || ((right_most_citizen_pos >= 0) && citizen_moved)) {
						vilian_moved = true;
						aggro[i]++;
						sprintf_s(log[log_index], 256, "vilian:%d->%d (aggro: %d -> %d)\n", i, i - 1, aggro[i] - 1, aggro[i]);
						if (aggro[i] > AGGRO_MAX) {
							aggro[i] = AGGRO_MAX;
							sprintf_s(log[log_index], 256, "vilian:%d->%d (aggro: %d)\n", i, i - 1, aggro[i]);
						}
						train[i - 1] = 4;
						train[i] = 0;
						aggro[i - 1] = aggro[i];
						aggro[i] = 0;
					}
					else {
						aggro[i]--;
						sprintf_s(log[log_index], 256, "vilian: stay %d (aggro: %d -> %d)\n", i, aggro[i] + 1, aggro[i]);
						if (aggro[i] < AGGRO_MIN) {
							aggro[i] = AGGRO_MIN;
							sprintf_s(log[log_index], 256, "vilian: stay %d (aggro: %d)\n", i, aggro[i]);
						}
					}
					log_index++;
				}
				else if ((train[i] == 3) || (train[i] == 5)) {//좀비, 강화좀비 이동
					if ((turn % 2 == 1) || (train[i] == 5)) {
						if ((train[i] == 3) && pull_success) {
							pull_success = false;
							sprintf_s(log[log_index], 256, "zombie: stay %d (cannot move due to pulling)\n\n", i);
						}
						else {
							if ((aggro[madongseok_pos] > aggro[right_most_citizen_pos]) && (train[i + 1] == 0)) {
								train[i + 1] = train[i];
								train[i] = 0;
								sprintf_s(log[log_index], 256, "zombie:%d->%d\n", i, i + 1);
								continue;
							}
							if ((aggro[madongseok_pos] <= aggro[right_most_citizen_pos]) && (train[i - 1] == 0)) {
								train[i - 1] = train[i];
								train[i] = 0;
								sprintf_s(log[log_index], 256, "zombie:%d->%d\n", i, i - 1);
								continue;
							}
							sprintf_s(log[log_index], 256, "zombie: stay %d (cannot move. there is someone)\n\n", i);
						}
					}
					else {
						sprintf_s(log[log_index], 256, "zombie: stay %d (cannot move at this turn)\n\n", i);
					}
					log_index++;
				}
			}
			//상태 출력1
			print_train(length, train);
			for (int i = 0; i < log_index; i++) {
				printf(log[i]);
			}
			log_index = 0;
			//마동석 이동
			int madongseok_move = -1;
			if (train[madongseok_pos - 1] == 0) input_value(&madongseok_move, MOVE_STAY, MOVE_LEFT, "madongseok move(0:stay, 1:left)>>");
			else input_value(&madongseok_move, MOVE_STAY, MOVE_STAY, "madongseok move(0:stay)>>");
			if (madongseok_move == MOVE_STAY) {
				aggro[madongseok_pos]--;
				if (aggro[madongseok_pos] < AGGRO_MIN) {
					aggro[madongseok_pos] = AGGRO_MIN;
					sprintf_s(log[log_index++], 256, "madongseok: stay %d (aggro: %d, stamina: %d)\n\n", madongseok_pos, aggro[madongseok_pos], stamina);
				}
				else {
					sprintf_s(log[log_index++], 256, "madongseok: stay %d (aggro: %d -> %d, stamina: %d)\n\n", madongseok_pos, aggro[madongseok_pos] + 1, aggro[madongseok_pos], stamina);
				}
			}
			else if (madongseok_move == MOVE_LEFT) {
				aggro[madongseok_pos]++;
				if (aggro[madongseok_pos] > AGGRO_MAX) {
					aggro[madongseok_pos] = AGGRO_MAX;
					sprintf_s(log[log_index++], 256, "madongseok: %d -> %d (aggro: %d, stamina: %d)\n\n", madongseok_pos, madongseok_pos - 1, aggro[madongseok_pos], stamina);
				}
				else {
					sprintf_s(log[log_index++], 256, "madongseok: %d -> %d (aggro: %d -> %d, stamina: %d)\n\n", madongseok_pos, madongseok_pos - 1, aggro[madongseok_pos] - 1, aggro[madongseok_pos], stamina);
				}
				train[madongseok_pos - 1] = 2;
				train[madongseok_pos] = 0;
				aggro[madongseok_pos - 1] = aggro[madongseok_pos];
				aggro[madongseok_pos] = 0;
				madongseok_pos--;
			}
			//상태 출력2
			print_train(length, train);
			for (int i = 0; i < log_index; i++) {
				printf(log[i]);
			}
			log_index = 0;
			citizen_index = 1;
			bool all_escaped = false;
			bool game_over = false;
			for (int i = 0; i < length; i++) {
				//시민 행동
				if (train[i] == 1) {
					if (i == 1) {
						sprintf_s(log[log_index++], 256, "citizen%d escaped!\n", citizen_offset + citizen_index);
						train[i] = 0;
						if (count_citizen(length, train) == 0) {
							sprintf_s(log[log_index++], 256, "All citizens escaped! Going to the next stage...\n\n");
							all_escaped = true;
							break;
						}
						citizen_offset++;
					}
					else {
						sprintf_s(log[log_index++], 256, "citizen%d does nothing.\n", citizen_offset + citizen_index);
						citizen_index++;
					}
				}
				else if (train[i] == 4) { //빌런 행동
					if (i == 1) {
						sprintf_s(log[log_index++], 256, "vilian escaped!\n");
						train[i] = 0;
					}
					else if ((train[i - 1] == 1) && (random_double(0, 100) < 30)) {
						int temp = aggro[i - 1];
						train[i - 1] = 4;
						train[i] = 1;
						aggro[i - 1] = aggro[i];
						aggro[i] = temp;
						sprintf_s(log[log_index++], 256, "vilian 발암 성공! 시민과 위치 교환\n");
					}
					else {
						sprintf_s(log[log_index++], 256, "vilian does nothing.\n");
					}
				}
				else if ((train[i] == 3) || (train[i] == 5)) { //좀비 행동
					if (i == right_most_citizen_pos + 1 && i < madongseok_pos - 1) { //시민과 인접, 마동석과는 멀리
						sprintf_s(log[log_index++], 256, "Zombie attacked citizen\n");
						if (stage == 4) {
							train[right_most_citizen_pos] = 5;
						}
						else {
							train[right_most_citizen_pos] = 0;
						}
						aggro[right_most_citizen_pos] = 0;
						for (int i = 0; i < length; i++) {
							if (train[i] == 1) right_most_citizen_pos = i;
						}
						if (count_citizen(length, train) == 0) {
							sprintf_s(log[log_index++], 256, "GAME OVER! All citizen died...\n");
							game_over = true;
							break;
						}
					}
					else if (right_most_citizen_pos + 1 < i && i == madongseok_pos - 1) { //시민과 멀리, 마동석과 인접
						stamina--;
						sprintf_s(log[log_index++], 256, "Zombie attacked madongseok (madongseok stamina: %d -> %d)\n", stamina + 1, stamina);
						if (stamina <= STM_MIN) {
							sprintf_s(log[log_index++], 256, "GAME OVER! madongseok dead...\n");
							game_over = true;
							break;
						}
					}
					else if (i == right_most_citizen_pos + 1 && i == madongseok_pos - 1) { //둘 다 인접
						if (aggro[madongseok_pos] > aggro[right_most_citizen_pos]) {
							stamina--;
							sprintf_s(log[log_index++], 256, "Zombie attacked madongseok (aggro: %d vs %d, madongseok stamina: %d -> %d)\n", aggro[right_most_citizen_pos], aggro[madongseok_pos], stamina + 1, stamina);
							if (stamina <= STM_MIN) {
								sprintf_s(log[log_index++], 256, "GAME OVER! madongseok died...\n");
								log_index++;
								game_over = true;
								break;
							}
						}
						else {
							sprintf_s(log[log_index++], 256, "Zombie attacked citizen (aggro: %d vs %d)\n", aggro[right_most_citizen_pos], aggro[madongseok_pos]);
							if (stage == 4) {
								train[right_most_citizen_pos] = 5;
							}
							else {
								train[right_most_citizen_pos] = 0;
							}
							aggro[right_most_citizen_pos] = 0;
							for (int i = 0; i < length; i++) {
								if (train[i] == 1) right_most_citizen_pos = i;
							}
							if (count_citizen(length, train) == 0) {
								sprintf_s(log[log_index++], 256, "GAME OVER! All citizen died...\n");
								game_over = true;
								break;
							}
						}
					}
					else {
						sprintf_s(log[log_index++], 256, "zombie attacked nobody.\n");
					}
				}
			}
			for (int i = 0; i < log_index; i++) {
				printf(log[i]);
			}
			if (all_escaped) break;
			if (game_over) {
				goto game_over;
			}
			log_index = 0;
			//마동석 행동
			int madongseok_action = -1;
			if ((train[madongseok_pos - 1] == 0) || (train[madongseok_pos - 1] == 5)) input_value(&madongseok_action, ACTION_REST, ACTION_PULL, "madongseok action(0.rest, 1.provoke, 2.pull)>>");
			else input_value(&madongseok_action, ACTION_REST, ACTION_PROVO, "madongseok action(0.rest, 1.provoke)>>");
			int aggro_previous = aggro[madongseok_pos], stamina_previous = stamina;
			if (madongseok_action == ACTION_REST) {
				sprintf_s(log[log_index++], 256, "madongseok rests...\n");
				aggro[madongseok_pos]--;
				stamina++;
			}
			else if (madongseok_action == ACTION_PROVO) {
				sprintf_s(log[log_index++], 256, "madongseok provoked zombie...\n");
				aggro[madongseok_pos] = AGGRO_MAX;
			}
			else if (madongseok_action == ACTION_PULL) {
				aggro[madongseok_pos] += 2;
				stamina--;
				if (random_double(0, 100) > probability) {
					sprintf_s(log[log_index++], 256, "madongseok pulled zombie...Next turn, it can't move\n");
					pull_success = true;
				}
				else {
					sprintf_s(log[log_index++], 256, "madongseok failed to pull zombie\n");
					pull_success = false;
				}
			}
			if (aggro[madongseok_pos] < AGGRO_MIN) aggro[madongseok_pos] = AGGRO_MIN;
			if (stamina > STM_MAX) stamina = STM_MAX;
			if ((aggro[madongseok_pos] == aggro_previous) && (stamina != stamina_previous)) {
				sprintf_s(log[log_index++], 256, "madongseok: %d (aggro: %d, stamina: %d -> %d)\n\n", madongseok_pos, aggro[madongseok_pos], stamina_previous, stamina);
			}
			else if ((aggro[madongseok_pos] == aggro_previous) && (stamina == stamina_previous)) {
				sprintf_s(log[log_index++], 256, "madongseok: %d (aggro: %d, stamina: %d)\n\n", madongseok_pos, aggro[madongseok_pos], stamina);
			}
			else if ((aggro[madongseok_pos] != aggro_previous) && (stamina != stamina_previous)) {
				sprintf_s(log[log_index++], 256, "madongseok: %d (aggro: %d -> %d, stamina: %d -> %d)\n\n", madongseok_pos, aggro_previous, aggro[madongseok_pos], stamina_previous, stamina);
			}
			else if ((aggro[madongseok_pos] != aggro_previous) && (stamina == stamina_previous)) {
				sprintf_s(log[log_index++], 256, "madongseok: %d (aggro: %d -> %d, stamina: %d)\n\n", madongseok_pos, aggro_previous, aggro[madongseok_pos], stamina);
			}
			for (int i = 0; i < log_index; i++) {
				printf(log[i]);
			}
			log_index = 0;
		}
	}
	printf("=======================================================\n");
	printf("=====You have cleared ALL stages !Congratulation !=====\n");
	printf("=======================================================\n\n");
	printf("==========ESCAPE !==========");
game_over:
	return 0;
}