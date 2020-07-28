#include "thread_consumer.h"

	ConsumerClient::ConsumerClient()
:num(0),m_thread_loop(NULL),m_thread_run(NULL)
{}

ConsumerClient::~ConsumerClient()
{}

void ConsumerClient::loop()
{
	while(true)
	{
		Para *tn=nullptr;
		{
			//看一下是否有线程结束了
			unique_lock <mutex> ul(m_mutex);
			if(NumQue.size() > 0){
				long long n = NumQue[0];
				NumQue.pop_front();
				if(HPMap.find(n) != HPMap.end()){
					tn = HPMap[n];
				}
			}
		}

		if(tn != nullptr) {
			//清理以下结束的线程
			tn->th->join();
			delete tn->th;
			tn->th = nullptr;
			cout<<"thread over num = "<<tn->num<<endl;
		}
		else
		{
			usleep(100);
			{
				//清理一下map
				set<int> sm;
				unique_lock <mutex> ul(m_mutex);
				for(auto& it :HPMap) {
					if(it.second->th == nullptr) 
					{
						sm.insert(it.first);
						delete it.second;
						it.second = nullptr;
					}
				}
				for(auto& s: sm) {
					cout<<"map clear num = "<<s<<endl;
					HPMap.erase(s);
				}
			}
		}
	}
	return;
}

void * thread_loop(void * arg)
{
	if (arg)
	{
		ConsumerClient &item = *(ConsumerClient*) arg;
		item.loop();
	}
	return nullptr;
}

void * thread_run(void * arg)
{
	if (arg)
	{
		ConsumerClient &item = *(ConsumerClient*) arg;
		item.run(arg);
	}
	return nullptr;
}


void * thread_work(void * arg)
{
	if (arg)
	{
		Para* item = (Para*) arg;
		ConsumerClient * te = (ConsumerClient*) item->te;
		int num = item->num;
		item = nullptr;
		te->Work(item);
		te->AddVec(num);
		cout<<"thread end num = "<<num<<endl;
	}
	return nullptr;
}


void ConsumerClient::AddVec(int &num)
{
	unique_lock <mutex> ul(m_mutex);
	NumQue.push_back(num);
}

//启动工作线程
void ConsumerClient::run(void *arg)
{
	int num=0;
	Para * item = nullptr;
	while(true) 
	{
		usleep(20000);
		item = new Para();
		item->num = ++num;
		item->te = arg;
		item->th = new thread(thread_work, item);
		{
			unique_lock <mutex> ul(m_mutex);
			HPMap[item->num] = item;
			cout<<"star num = "<<item->num<<endl;
		}
	}
	return;
}

void ConsumerClient::Work(Para *hp)
{
	//工作
	cout<<"work"<<endl;
	return;
}

void ConsumerClient::start()
{
	m_thread_loop = new thread(thread_loop, this);
	m_thread_run = new thread(thread_run, this);
}
