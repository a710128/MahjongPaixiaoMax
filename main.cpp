#include <cstdio>
#include <cstring>
#include <algorithm>
#include <ctime>
#include <thread>
#include "sampler.h"

const int OFFSET_M = 0;
const int OFFSET_P = 9;
const int OFFSET_S = 18;
const int OFFSET_Z = 27;


void thread_task(double result[], const int hai[], const int rest[], int seed, int times, int max_round) {
	Sampler *smp = new Sampler(hai, rest, seed);
	smp->sample(result, times, max_round);
	delete smp;
}


int main() {
	static char s[256];
	static int hai[9 * 3 + 7];
	static int rest[9 * 3 + 7];
	static int stack[20];

	for (int i = 0; i < 9 * 3 + 7; ++ i) {
		rest[i] = 4;
		hai[i] = 0;
	}

	printf("Hai: ");
	scanf("%s", s);
	//printf("Num process: ");
	//int num_process;
	//scanf("%d", &num_process);
	int num_process = 5;

	int len = strlen(s);
	stack[0] = 0;
	for (int i = 0; i < len; ++ i) {
		if ('0' <= s[i] && s[i] <= '9') {
			if (s[i] == '0') stack[++ stack[0]] = 5;
			else stack[++ stack[0]] = s[i] - '0';
		}
		else if (s[i] == 'm' || s[i] == 'p' || s[i] == 's' || s[i] == 'z'){
			int base = OFFSET_M;
			if (s[i] == 'p') base = OFFSET_P;
			else if (s[i] == 's') base = OFFSET_S;
			else if (s[i] == 'z') base = OFFSET_Z;
			while (stack[0]) hai[stack[stack[0] --] + base - 1] ++;
		}
	}
	for (int i = 0; i < 9 * 3 + 7; ++ i) rest[i] -= hai[i];
	
	int iter = 0, seed = int(time(NULL));

	double **result = new double*[num_process];
	double **subresult = new double*[num_process];
	std::thread **thd = new std::thread*[num_process];

	for (int i = 0; i < num_process; ++ i) {
		result[i] = new double[34];
		subresult[i] = new double[34];
	}
	for (int i = 0; i < 34; ++ i) {
		for (int j = 0; j < num_process; ++ j)
			result[j][i] = 0;
	}

	int gap = 18 / num_process;

	double epc[34];

	while (true) {
		
		for (int i = 0; i < num_process; ++ i) {
			thd[i] = new std::thread(thread_task, subresult[i], hai, rest, seed + iter * num_process + i,
				 10000, (i + 1) * gap);
		}
		for (int i = 0; i < num_process; ++ i) {
			thd[i]->join();
		}
		for (int i = 0; i < num_process; ++ i) {
			for (int j = 0; j < 34; ++ j) {
				result[i][j] += subresult[i][j];
			}
			delete thd[i];
		}
		iter ++;

		printf("Iter: %d\n", iter);
		printf("Round");
		for (int i = 0; i < 34; ++ i) {
			if (hai[i] > 0) {
				printf("\t%d%c", (i % 9) + 1, "mpsz"[i / 9]);
			}
		}
		printf("\n");
		for (int i = 0; i < num_process; ++ i) {
			printf("%d", (i + 1) * gap);
			for (int j = 0; j < 34; ++ j) {
				if (hai[j] > 0) {
					printf("\t%.2lf%%", 100.0 * result[i][j] / iter);
				}
			}
			printf("\n");
		}
		for (int i = 0; i < 34; ++ i) epc[i] = 0;
		double cpos = gap / 2.0;
		for (int i = 0; i < num_process; ++ i) {
			for (int j = 0; j < 34; ++ j) {
				double prob = result[i][j] / iter;
				if (i > 0) prob = prob - (result[i - 1][j] / iter);
				epc[j] += prob * cpos;
			}
			cpos = cpos + gap;
		}
		cpos = (cpos + (gap / 2.0) + 30) / 2.0;
		for (int i = 0; i < 34; ++ i) {
			double prob = 1 - (result[num_process - 1][i] / iter);
			epc[i] += prob * cpos;
		}

		int order[34];
		for (int i = 0; i < 34; ++ i) {
			order[i] = i;
		}
		/* argsort */
		for (int i = 0; i < 34; ++ i) {
			for (int j = i; j > 0; j --) {
				if ( epc[order[j - 1]] >  epc[order[j]]) {
					std::swap(order[j - 1], order[j]);
				}
				else break;
			}
		}
		
		for (int i = 0; i < 34; ++ i) {
			if (hai[order[i]] > 0) {
				printf("%d%c: %lf\n", (order[i] % 9) + 1, "mpsz"[order[i] / 9], 
					epc[order[i]]);
			}
		}
		printf("\n\n");
	}

	for (int i = 0; i < num_process; ++ i) {
		delete[] subresult[i];
		delete[] result[i];
	}
	delete[] subresult;
	delete[] result;
	delete[] thd;

	

	

	
	return 0;
}