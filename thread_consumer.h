#include <thread>
#include <mutex>
#include <iostream>
#include <string.h>
#include <deque>
#include <set>
#include <map>
#include <unistd.h>
using namespace std;

struct Para
{
	//线程号
	thread* th;

	//this指针（ConsumerClient对象）
	void* te;

	//计数number（线程标识）
	long long num;

	//业务id
	int id;
};

class ConsumerClient
{
	public:
		//构造、析构
		ConsumerClient();
		~ConsumerClient();

	public:
		//开始工作
		void start();

		//内存清理线程 
		void loop();

		//启动工作的线程
		void run(void* arg);

		//工作线程
		void Work(Para *);

		//回写接口
		void AddVec(int& num);

	private:
		mutex m_mutex;

		//计数器
		long long num;

		//存储请求参数和请求编号的map
		map<long long, Para*> HPMap;

		//线程结束回写标志
		deque<long long> NumQue;

		//启工作和资源释放的线程
		thread *m_thread_loop;
		thread *m_thread_run;
};
