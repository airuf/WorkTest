#include <stdlib.h>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>
#include <vector>

#include "WifiDetect.h"

using namespace std;
mutex gMutex;


class ClassA
{
public:
	void func1()
	{
		lock_guard<recursive_mutex> lock(m_Mutex);
	}
	void func2()
	{
		lock_guard<recursive_mutex> lock(m_Mutex);
	}

private:
	recursive_mutex m_Mutex;
};


void OutputValue(int n)
{
	lock_guard<mutex> mLock(gMutex);
	//gMutex.lock();
	cout << "Number";
	for (int i = 0; i < n; ++i)
	{
		//delay 5 ms
		this_thread::sleep_for(chrono::duration<int, std::milli>(5));
		cout << " " << i;
	}
	cout << endl;
	//gMutex.unlock();
}


struct bank_account
{
	explicit bank_account(string name, int money)
	{
		sName = name;
		iMoney = money;
	}

	string sName;
	int iMoney;
	mutex m_mutex;

};

void transfer(bank_account &from, bank_account &to, int amount)
{
	//dont't actually take the locks yets
	unique_lock<mutex> lock1(from.m_mutex, defer_lock);
	unique_lock<mutex> lock2(to.m_mutex, defer_lock);

	//lock bith unique_locks without deadlcok
	lock(lock1, lock2);

	from.iMoney -= amount;
	to.iMoney += amount;

	cout << " Transfer " << amount << " from " << from.sName << " to " << to.sName << endl;

}


int main(int argc, char **argv)
{
	if (0)//thread test
	{
		cout << "Normal function call " << endl;
		OutputValue(3);
		OutputValue(4);

		cout << "\nCall function with thread " << endl;
		thread mThread1(OutputValue, 3);
		thread mThread2(OutputValue, 4);

		mThread1.join();
		mThread2.join();
		cout << endl;


		bank_account Account1("User1", 100);
		bank_account Account2("User2", 50);

		thread t1([&]() {transfer(Account1, Account2, 10); });
		thread t2([&]() {transfer(Account2, Account1, 5); });

		t1.join();
		t2.join();
	}

	if (0)//vector test
	{

		vector<unsigned char> m_saveData, m_recvData;
		std::vector<unsigned char>::iterator it;
		m_recvData.assign(7, 101);
		m_recvData.push_back('2');
		m_recvData.push_back('3');
		m_saveData.insert(m_saveData.end(),m_recvData.begin(), m_recvData.end());
		m_saveData.push_back('1');
		cout << m_saveData.size();
		cout << m_saveData.data();
		for (it = m_saveData.begin(); it < m_saveData.end(); it++)
		{
			std::cout << ' ' << *it;
		}
		cout << endl;
		for (it = m_recvData.begin(); it < m_recvData.end(); it++)
		{
			std::cout << ' ' << *it ;
		}
		cout << endl;
	}

	if (1)
	{
		WifiTest();
	}
	

	system("pause");
	return 0;
}




