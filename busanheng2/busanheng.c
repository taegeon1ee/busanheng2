#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<windows.h>
#include<stdbool.h>

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
void print_train(int length, int citizen_pos, int zombie_pos, int madongseok_pos) {
	for (int i = 0; i < length; i++) printf("#");
	printf("\n");
	for (int i = 0; i < length; i++) {
		if ((i == 0) || (i == length - 1)) printf("#");
		else if (i == citizen_pos) printf("C");
		else if (i == madongseok_pos) printf("M");
		else if (i == zombie_pos) printf("Z");
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

int main(void) {
	printf("==========GAME START !==========\n");
	srand((unsigned int)time(NULL));

	int length = -1, probability = -1, stamina = -1;
	input_value(&length, LEN_MIN, LEN_MAX, "train length(15~50)>>");
	input_value(&stamina, STM_MIN, STM_MAX, "madongseok stamina(0~5)>>");
	input_value(&probability, PROB_MIN, PROB_MAX, "percentile probability 'p'(10~90)>>");

	int C = length - 6, Z = length - 3, M = length - 2, aggro_C = 1, aggro_M = 1;
	bool pull_success = false;
	print_train(length, C, Z, M);

	for (int turn = 1; ; turn++) {
		int C_previous = C, Z_previous = Z, aggro_C_previous = aggro_C;
		//시민 이동
		if (random_double(0, 100) > probability) {
			if (++aggro_C > AGGRO_MAX) aggro_C = AGGRO_MAX;
			C--;
		}
		else {
			if (--aggro_C < AGGRO_MIN) aggro_C = AGGRO_MIN;
		}
		//좀비 이동
		if (turn % 2 == 1) {
			if (pull_success) pull_success = false;
			else {
				if ((aggro_M > aggro_C) && (Z < M - 1)) Z++;
				if ((aggro_M <= aggro_C) && (Z > C + 1)) Z--;
			}
		}
		//상태 출력1
		print_train(length, C, Z, M);
		if (C != C_previous) {
			if (aggro_C_previous != aggro_C) printf("citizen:%d->%d (aggro: %d -> %d)\n", C_previous, C, aggro_C_previous, aggro_C);
			else printf("citizen:%d->%d (aggro: %d)\n", C_previous, C, aggro_C);
		}
		else {
			if (aggro_C_previous != aggro_C) printf("citizen: stay %d (aggro: %d -> %d)\n", C, aggro_C_previous, aggro_C);
			else printf("citizen: stay %d (aggro: %d)\n", C, aggro_C);
		}
		if (Z != Z_previous) printf("zombie:%d->%d\n", Z_previous, C);
		else printf("zombie: stay %d (cannot move)\n\n", Z);
		//마동석 이동
		int madongseok_move = -1, M_previous = M, aggro_M_previous = aggro_M;
		if (Z == M - 1) input_value(&madongseok_move, MOVE_STAY, MOVE_STAY, "madongseok move(0:stay)>>");
		else input_value(&madongseok_move, MOVE_STAY, MOVE_LEFT, "madongseok move(0:stay, 1:left)>>");
		if (madongseok_move == MOVE_STAY) {
			if (--aggro_M < AGGRO_MIN) aggro_M = AGGRO_MIN;
		}
		else if (madongseok_move == MOVE_LEFT) {
			if (++aggro_M > AGGRO_MAX) aggro_M = AGGRO_MAX;
			M--;
		}
		//상태 출력2
		print_train(length, C, Z, M);
		if (M != M_previous) {
			if (aggro_M_previous != aggro_M) printf("madongseok:%d->%d (aggro: %d -> %d, stamina: %d)\n\n", M_previous, M, aggro_M_previous, aggro_M, stamina);
			else printf("madongseok:%d->%d (aggro: %d, stamina: %d)\n\n", M_previous, M, aggro_M, stamina);
		}
		else {
			if (aggro_M_previous != aggro_M) printf("madongseok: stay %d (aggro: %d -> %d, stamina: %d)\n\n", M, aggro_M_previous, aggro_M, stamina);
			else printf("madongseok: stay %d (aggro: %d, stamina: %d)\n\n", M, aggro_M, stamina);
		}
		//시민 행동
		if (C == 1) {
			printf("==========ESCAPE !==========");
			break;
		}
		else printf("citizen does nothing.\n");
		//좀비 행동
		int stamina_previous = stamina;
		if (Z == C + 1 && Z < M - 1) { //시민과 인접, 마동석과는 멀리
			printf("Zomibe attacked citizen\n");
			printf("GAME OVER! citizen dead...\n");
			break;
		}
		else if (C + 1 < Z && Z == M - 1) { //시민과 멀리, 마동석과 인접
			stamina--;
			printf("Zomibe attacked madongseok (madongseok stamina: %d -> %d)\n", stamina_previous, stamina);
			if (stamina <= STM_MIN) {
				printf("GAME OVER! madongseok dead...\n");
				break;
			}
		}
		else if (Z == C + 1 && Z == M - 1) { //둘 다 인접
			if (aggro_M > aggro_C) {
				stamina--;
				printf("Zomibe attacked madongseok (aggro: %d vs %d, madongseok stamina: %d -> %d)\n", aggro_C, aggro_M, stamina + 1, stamina);
				if (stamina <= STM_MIN) {
					printf("GAME OVER! madongseok dead...\n");
					break;
				}
			}
			else {
				printf("Zomibe attacked citizen (aggro: %d vs %d)\n", aggro_C, aggro_M);
				printf("GAME OVER! citizen dead...\n");
				break;
			}
		}
		else {
			printf("zombie attacked nobody.\n");
		}
		//마동석 행동
		int madongseok_action = -1;
		aggro_M_previous = aggro_M; stamina_previous = stamina;
		if (Z == M - 1) input_value(&madongseok_action, ACTION_REST, ACTION_PULL, "madongseok action(0.rest, 1.provoke, 2.pull)>>");
		else input_value(&madongseok_action, ACTION_REST, ACTION_PROVO, "madongseok action(0.rest, 1.provoke)>>");
		if (madongseok_action == ACTION_REST) {
			if (--aggro_M < AGGRO_MIN) aggro_M = AGGRO_MIN;
			if (++stamina > STM_MAX) stamina = STM_MAX;
			printf("madongseok rests...\n");
		}
		else if (madongseok_action == ACTION_PROVO) {
			aggro_M = AGGRO_MAX;
			printf("madongseok provoked zombie...\n");
		}
		else if (madongseok_action == ACTION_PULL) {
			if ((aggro_M += 2) > AGGRO_MAX) aggro_M = AGGRO_MAX;
			if (--stamina < STM_MIN) stamina = STM_MIN;
			if (random_double(0, 100) > probability) {
				printf("madongseok pulled zombie...Next turn, it can't move\n");
				pull_success = true;
			}
			else {
				printf("madongseok failed to pull zombie\n");
				pull_success = false;
			}
		}
		if (stamina_previous != stamina) printf("madongseok: %d (aggro: %d -> %d, stamina: %d -> %d)\n\n", M, aggro_M_previous, aggro_M, stamina_previous, stamina);
		else  printf("madongseok: %d (aggro: %d -> %d, stamina: %d)\n\n", M, aggro_M_previous, aggro_M, stamina);
	}
	return 0;
}