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

bool terminated(process * arr) {		// ��� Process�� ������ �� �˻��ϴ� �Լ�
	for (int i = 0; i < process_num; i++) {
		if (arr[i].state < 2)
			return false;
	}
	return true;
}

bool operator<(process a, process b) {	// Process ���� �켱������ ���ϱ� ���� ������ �����ε�
	if (a.priority == b.priority) return a.number > b.number;
	else return a.priority > b.priority;
}

void schedule(int type) {			// 0: FIFO  1: SJF  2: Priority
	process tmp_arr[10];
	process * running = NULL;		// CPU���� ���� ���ư��� �ִ� Process
	int now = 0;					// ���� �ð�
	int running_time = -1;			// CPU���� ���� ���ư��� �ִ� Process�� ������ �ð�
	bool CPU_busy = false;			// CPU�� ���ư��� �ִ����� ��Ÿ���� bool ����
	priority_queue<process> ready;	// Ready Queue
	for (int i = 0; i < process_num; i++) {
		tmp_arr[i] = arr[i];
		if (type == 0)				// FIFO -> �켱������ �����ð�
			tmp_arr[i].priority = tmp_arr[i].arrival_time;
		if (type == 1)				// SJF -> �켱������ ���� CPU burst
			tmp_arr[i].priority = tmp_arr[i].CPU_burst[tmp_arr[i].idx];
	}
	while (!terminated(tmp_arr)) {	// ��� Process�� terminated �� �� ���� �ݺ�
		for (int i = 0; i < process_num; i++) {
			if (tmp_arr[i].state == 2)			// Process�� State�� Terminated�̸� �ƹ��� �˻縦 ��������
				continue;
			if (tmp_arr[i].arrival_time == now) {	// Process�� �������� �� Ready Queue�� �־���
				tmp_arr[i].state = 0;
				ready.push(tmp_arr[i]);
			}
			if (tmp_arr[i].state == 1 && tmp_arr[i].sleep_time + tmp_arr[i].IO_burst[tmp_arr[i].idx - 1] == now) {	// Process�� I/O burst�� ���� �ð��� �ٽ� Ready Queue�� �־���
				tmp_arr[i].waiting_time -= tmp_arr[i].IO_burst[tmp_arr[i].idx - 1];
				tmp_arr[i].state = 0;
				if (type == 0)
					tmp_arr[i].priority = now;	// FIFO -> Priority�� ���� CPU burst�� Arrival time���� ������Ʈ
				if (type == 1)
					tmp_arr[i].priority = tmp_arr[i].CPU_burst[tmp_arr[i].idx];	// SJF ->  Priority�� ������ �� CPU burst�� ������Ʈ
				ready.push(tmp_arr[i]);
			}
		}
		if (now == running_time) {	// ���� CPU�� �ִ� Process�� CPU_Burst�� ������ ��
			CPU_busy = false;
			if (running->CPU_burst.size() == running->idx) {	// Process�� ������ CPU burst�� �������� ����
				running->state = 2;
				running->turnaround_time = now - running->arrival_time; // Turnaround time ���
			}
			else {	// ������ CPU burst�� �ƴ϶�� �ð��� ����صΰ� waiting ���·� �ٲ�д�
				running->sleep_time = now;
				running->state = 1;
			}
		}
		if (!CPU_busy && !ready.empty()) {	// Ready Queue�� ������� Process�� �����ϰ�, CPU�� busy ���� ������ Scheduling
			CPU_busy = true;
			running = &tmp_arr[ready.top().number];
			ready.pop();
			cout << now << " " << running->number << "\n";
			running->waiting_time += now - running->sleep_time;			// Waiting time ���
			running_time = now + running->CPU_burst[running->idx++];	// ���� CPU burst�� ������ �ð� ���
		}
		now++;	// 1�� ����
	}
	for (int i = 0; i < process_num; i++)	// �Լ� ����� �� Process�� Waiting time �� Turnaround time�� ���
		cout << "process " << i << " waiting time : " << tmp_arr[i].waiting_time << ", turnaround time: " << tmp_arr[i].turnaround_time << "\n";
}

void Round_Robin() {
	process tmp_arr[10];
	process * running = NULL;		// CPU���� ���� ���ư��� �ִ� Process
	int now = 0;					// ���� �ð�
	int running_time = -1;			// CPU���� ���� ���ư��� �ִ� Process�� ������ �ð�
	bool CPU_busy = false;			// CPU�� ���ư��� �ִ����� ��Ÿ���� bool ����
	queue<process> ready;			// Ready Queue
	for (int i = 0; i < process_num; i++)
		tmp_arr[i] = arr[i];
	while (!terminated(tmp_arr)) {	// ��� Process�� terminated �� �� ���� �ݺ�
		for (int i = 0; i < process_num; i++) {
			if (tmp_arr[i].state == 2)			// Process�� State�� Terminated�̸� �ƹ��� �˻縦 ��������
				continue;
			if (tmp_arr[i].arrival_time == now) { // Process�� �������� �� Ready Queue�� �־���
				tmp_arr[i].state = 0;
				ready.push(tmp_arr[i]);
			}
			if (tmp_arr[i].state == 1 && tmp_arr[i].sleep_time + tmp_arr[i].IO_burst[tmp_arr[i].idx - 1] == now) {	// Process�� I/O burst�� ���� �ð��� �ٽ� Ready Queue�� �־���
				tmp_arr[i].waiting_time -= tmp_arr[i].IO_burst[tmp_arr[i].idx - 1];
				tmp_arr[i].state = 0;
				ready.push(tmp_arr[i]);
			}
		}
		if (now == running_time) {	// ���� CPU�� �ִ� Process�� CPU_Burst�� ������ ��
			CPU_busy = false;
			if (running->CPU_burst.size() - 1 == running->idx && running->CPU_burst[running->idx] == 0) {	// Process�� ������ CPU burst�� �������� ����
				running->state = 2;
				running->turnaround_time = now - running->arrival_time; // Turnaround time ���
			}
			else {	// ������ CPU burst�� �ƴ϶�� �ð��� ����صΰ� waiting ���·� �ٲ�д�
				if (running->CPU_burst[running->idx] > 0) {	// Time Quantum��ŭ ������ CPU burst�� ���Ҵٸ� ť�� �ٽ� �־���
					running->sleep_time = now;
					ready.push(tmp_arr[running->number]);
				}
				else {										// CPU burst�� ���� �ʾҴٸ� Waiting���� �ٲ���
					running->sleep_time = now;
					running->state = 1;
					running->idx++;
				}
			}
		}
		if (!CPU_busy && !ready.empty()) {	// Ready Queue�� ������� Process�� �����ϰ�, CPU�� busy ���� ������ Scheduling
			CPU_busy = true;
			running = &tmp_arr[ready.front().number];
			ready.pop();
			cout << now << " " << running->number << "\n";
			running->waiting_time += now - running->sleep_time;			// Waiting time ���
			if (running->CPU_burst[running->idx] > TIME_QUANTUM) {		// Time Quantum��ŭ �����ϰ� CPU burst�� ���� ��ŭ �ٿ���
				running_time = now + TIME_QUANTUM;
				running->CPU_burst[running->idx] -= TIME_QUANTUM;
			}
			else {														// ���� CPU burst�� Time Quantum���� �۰ų� ���ٸ� �׸�ŭ�� ����
				running_time = now + running->CPU_burst[running->idx];
				running->CPU_burst[running->idx] = 0;
			}
		}

		now++;	// 1�� ����
	}
	for (int i = 0; i < process_num; i++)	// �Լ� ����� �� Process�� Waiting time �� Turnaround time�� ���
		cout << "process " << i << " waiting time : " << tmp_arr[i].waiting_time << ", turnaround time: " << tmp_arr[i].turnaround_time << "\n";
}

int main() {
	freopen("Text.txt", "r", stdin);
	//********************�Է� �κ�************************
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
	//********************�Է� �κ�************************

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