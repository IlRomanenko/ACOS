#include <iostream>
#include <cstdio>
#include <pthread.h>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <unistd.h>
#include <semaphore.h>


using namespace std;

const int maxn = 32, thread_count = 4;
const int part = maxn / thread_count;
	

bool **life, **buf, **cur_life;
int offset[] = {-1, 0, 1};

pthread_mutex_t mutx;
sem_t semap_end;

inline void set(int x, int y, bool **from, bool** to)
{
	int cnt = 0, tx, ty;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			tx = x + offset[i];
			if (tx >= maxn)
				tx = 0;
			else if (tx < 0)
				tx = maxn - 1;

			ty = y + offset[j];
			if (ty >= maxn)
				ty = 0;
			else if (ty < 0)
				ty = maxn - 1;

			if (tx == x && ty == y)
				continue;

			if (from[tx][ty])
				cnt++;
		}
	}

	if (from[x][y])
	{
		if (cnt == 2 || cnt == 3)
			to[x][y] = true;
		else
			to[x][y] = false;
	}
	else
	{
		if (cnt == 3)
			to[x][y] = true;
		else
			to[x][y] = false;
	}
} 

struct send_data
{
	int from_row, to_row;
	bool **from, **to;
};

void* recalc(void * data)
{
	send_data* cur_data = (send_data*)data;
	int from_row = cur_data->from_row, to_row = cur_data->to_row;
	bool **from = cur_data->from, **to = cur_data->to;

	for (int i = from_row; i < to_row; i++)
	{
		for (int j = 0; j < maxn; j++)
			set(i, j, from, to);
	}
	sem_post(&semap_end);
	delete cur_data;
}

int main(int argc, char** argv)
{	
	life = new bool*[maxn];
	buf = new bool*[maxn];
	for (int i = 0; i < maxn; i++)
	{
		life[i] = new bool[maxn];
		buf[i] = new bool[maxn];
	}

	srand(time(NULL));

	for (int i = 0; i < maxn; i++)
		for (int j = 0; j < maxn; j++)
		{
			life[i][j] = rand() % 2;
			buf[i][j] = false;
		}

	bool ** first = life, **second = buf;

	pthread_t temp;
	
	for (;;)
	{

		sem_init(&semap_end, 0, -thread_count);

		for (int i = 0; i < thread_count; i++)
		{
			send_data *cur_data = new send_data();
			cur_data->from = first;
			cur_data->to = second;
			cur_data->from_row = i * part;

			if (i == thread_count - 1)
				cur_data->to_row = maxn;
			else
				cur_data->to_row = (i + 1) * part;

			pthread_create(&temp, NULL, recalc, cur_data);
		}

		sem_wait(&semap_end);

		cur_life = first;

		cout << endl << endl << endl;
		cout << endl << endl << endl;
		cout << endl << endl << endl;
		cout << endl << endl << endl;		cout << endl << endl << endl;


		for (int i = 0; i < maxn; i++)
		{
			for (int j = 0; j < maxn; j++)
			{
				if (first[i][j])
					cout << ".";
				else
					cout << " ";
			}
			cout << endl;
		}
		usleep(200 * 1000);
		swap(first, second);
	}

	return 0;
}

