#include <iostream>
#include <algorithm>
#include <cstdio>
#include <vector>
#include <string>
#include <queue>
#define TIME_QUANTUM 10
using namespace std;

char buffer[200];
char * input;
int process_num, a, b;
struct process {
public:
	int state;		// 0: Ready  1: Waiting  2: Terminated
	int idx;
	int number;
	int arrival_time;
	int sleep_time;
	int waiting_time;
	int turnaround_time;
	int priority;
	vector<int> CPU_burst;
	vector<int> IO_burst;

};
process arr[10];

bool terminated(process * arr) {		// 모든 Process가 끝났는 지 검사하는 함수
	for (int i = 0; i < process_num; i++) {
		if (arr[i].state < 2)
			return false;
	}
	return true;
}

bool operator<(process a, process b) {	// Process 간의 우선순위를 비교하기 위한 연산자 오버로딩
	if (a.priority == b.priority) return a.number > b.number;
	else return a.priority > b.priority;
}

void schedule(int type) {			// 0: FIFO  1: SJF  2: Priority
	process tmp_arr[10];
	process * running = NULL;		// CPU에서 현재 돌아가고 있는 Process
	int now = 0;					// 현재 시간
	int running_time = -1;			// CPU에서 현재 돌아가고 있는 Process가 끝나는 시간
	bool CPU_busy = false;			// CPU가 돌아가고 있는지를 나타내는 bool 변수
	priority_queue<process> ready;	// Ready Queue
	for (int i = 0; i < process_num; i++) {
		tmp_arr[i] = arr[i];
		if (type == 0)				// FIFO -> 우선순위가 도착시간
			tmp_arr[i].priority = tmp_arr[i].arrival_time;
		if (type == 1)				// SJF -> 우선순위가 남은 CPU burst
			tmp_arr[i].priority = tmp_arr[i].CPU_burst[tmp_arr[i].idx];
	}
	while (!terminated(tmp_arr)) {	// 모든 Process가 terminated 될 때 까지 반복
		for (int i = 0; i < process_num; i++) {
			if (tmp_arr[i].state == 2)			// Process의 State가 Terminated이면 아무런 검사를 하지않음
				continue;
			if (tmp_arr[i].arrival_time == now) {	// Process가 도착했을 때 Ready Queue에 넣어줌
				tmp_arr[i].state = 0;
				ready.push(tmp_arr[i]);
			}
			if (tmp_arr[i].state == 1 && tmp_arr[i].sleep_time + tmp_arr[i].IO_burst[tmp_arr[i].idx - 1] == now) {	// Process가 I/O burst를 끝낸 시간에 다시 Ready Queue에 넣어줌
				tmp_arr[i].waiting_time -= tmp_arr[i].IO_burst[tmp_arr[i].idx - 1];
				tmp_arr[i].state = 0;
				if (type == 0)
					tmp_arr[i].priority = now;	// FIFO -> Priority를 다음 CPU burst의 Arrival time으로 업데이트
				if (type == 1)
					tmp_arr[i].priority = tmp_arr[i].CPU_burst[tmp_arr[i].idx];	// SJF ->  Priority를 다음에 할 CPU burst로 업데이트
				ready.push(tmp_arr[i]);
			}
		}
		if (now == running_time) {	// 현재 CPU에 있는 Process가 CPU_Burst를 끝냈을 때
			CPU_busy = false;
			if (running->CPU_burst.size() == running->idx) {	// Process가 마지막 CPU burst를 끝냈으면 종료
				running->state = 2;
				running->turnaround_time = now - running->arrival_time; // Turnaround time 계산
			}
			else {	// 마지막 CPU burst가 아니라면 시간을 기록해두고 waiting 상태로 바꿔둔다
				running->sleep_time = now;
				running->state = 1;
			}
		}
		if (!CPU_busy && !ready.empty()) {	// Ready Queue에 대기중인 Process가 존재하고, CPU가 busy 하지 않으면 Scheduling
			CPU_busy = true;
			running = &tmp_arr[ready.top().number];
			ready.pop();
			cout << now << " " << running->number << "\n";
			running->waiting_time += now - running->sleep_time;			// Waiting time 계산
			running_time = now + running->CPU_burst[running->idx++];	// 현재 CPU burst가 끝나는 시간 계산
		}
		now++;	// 1초 증가
	}
	for (int i = 0; i < process_num; i++)	// 함수 종료시 각 Process의 Waiting time 과 Turnaround time을 출력
		cout << "process " << i << " waiting time : " << tmp_arr[i].waiting_time << ", turnaround time: " << tmp_arr[i].turnaround_time << "\n";
}

void Round_Robin() {
	process tmp_arr[10];
	process * running = NULL;		// CPU에서 현재 돌아가고 있는 Process
	int now = 0;					// 현재 시간
	int running_time = -1;			// CPU에서 현재 돌아가고 있는 Process가 끝나는 시간
	bool CPU_busy = false;			// CPU가 돌아가고 있는지를 나타내는 bool 변수
	queue<process> ready;			// Ready Queue
	for (int i = 0; i < process_num; i++)
		tmp_arr[i] = arr[i];
	while (!terminated(tmp_arr)) {	// 모든 Process가 terminated 될 때 까지 반복
		for (int i = 0; i < process_num; i++) {
			if (tmp_arr[i].state == 2)			// Process의 State가 Terminated이면 아무런 검사를 하지않음
				continue;
			if (tmp_arr[i].arrival_time == now) { // Process가 도착했을 때 Ready Queue에 넣어줌
				tmp_arr[i].state = 0;
				ready.push(tmp_arr[i]);
			}
			if (tmp_arr[i].state == 1 && tmp_arr[i].sleep_time + tmp_arr[i].IO_burst[tmp_arr[i].idx - 1] == now) {	// Process가 I/O burst를 끝낸 시간에 다시 Ready Queue에 넣어줌
				tmp_arr[i].waiting_time -= tmp_arr[i].IO_burst[tmp_arr[i].idx - 1];
				tmp_arr[i].state = 0;
				ready.push(tmp_arr[i]);
			}
		}
		if (now == running_time) {	// 현재 CPU에 있는 Process가 CPU_Burst를 끝냈을 때
			CPU_busy = false;
			if (running->CPU_burst.size() - 1 == running->idx && running->CPU_burst[running->idx] == 0) {	// Process가 마지막 CPU burst를 끝냈으면 종료
				running->state = 2;
				running->turnaround_time = now - running->arrival_time; // Turnaround time 계산
			}
			else {	// 마지막 CPU burst가 아니라면 시간을 기록해두고 waiting 상태로 바꿔둔다
				if (running->CPU_burst[running->idx] > 0) {	// Time Quantum만큼 실행후 CPU burst가 남았다면 큐에 다시 넣어줌
					running->sleep_time = now;
					ready.push(tmp_arr[running->number]);
				}
				else {										// CPU burst가 남지 않았다면 Waiting으로 바꿔줌
					running->sleep_time = now;
					running->state = 1;
					running->idx++;
				}
			}
		}
		if (!CPU_busy && !ready.empty()) {	// Ready Queue에 대기중인 Process가 존재하고, CPU가 busy 하지 않으면 Scheduling
			CPU_busy = true;
			running = &tmp_arr[ready.front().number];
			ready.pop();
			cout << now << " " << running->number << "\n";
			running->waiting_time += now - running->sleep_time;			// Waiting time 계산
			if (running->CPU_burst[running->idx] > TIME_QUANTUM) {		// Time Quantum만큼 실행하고 CPU burst를 남은 만큼 줄여줌
				running_time = now + TIME_QUANTUM;
				running->CPU_burst[running->idx] -= TIME_QUANTUM;
			}
			else {														// 남은 CPU burst가 Time Quantum보다 작거나 같다면 그만큼만 실행
				running_time = now + running->CPU_burst[running->idx];
				running->CPU_burst[running->idx] = 0;
			}
		}

		now++;	// 1초 증가
	}
	for (int i = 0; i < process_num; i++)	// 함수 종료시 각 Process의 Waiting time 과 Turnaround time을 출력
		cout << "process " << i << " waiting time : " << tmp_arr[i].waiting_time << ", turnaround time: " << tmp_arr[i].turnaround_time << "\n";
}

int main() {
	freopen("Text.txt", "r", stdin);
	//********************입력 부분************************
	cin >> process_num;
	for (int i = 0; i < process_num; i++) {
		cin >> a >> b;
		arr[i].idx = 0;
		arr[i].number = i;
		arr[i].sleep_time = arr[i].arrival_time = a;
		arr[i].priority = b;
		arr[i].waiting_time = 0;
		cin.getline(buffer, 200);
		input = strtok(buffer, " ");
		arr[i].CPU_burst.push_back(atoi(input));
		input = strtok(NULL, " ");
		while (input != NULL) {
			arr[i].IO_burst.push_back(atoi(input));
			input = strtok(NULL, " ");
			arr[i].CPU_burst.push_back(atoi(input));
			input = strtok(NULL, " ");
		}
	}
	//********************입력 부분************************

	cout << "=======================FIFO=======================\n";
	schedule(0);
	cout << "==================================================\n\n";
	cout << "=======================SJF========================\n";
	schedule(1);
	cout << "==================================================\n\n";
	cout << "=====================Priority=====================\n";
	schedule(2);
	cout << "==================================================\n\n";
	cout << "========================RR========================\n";
	Round_Robin();
	cout << "==================================================\n\n";
	return 0;
}