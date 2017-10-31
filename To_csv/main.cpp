#include <cstdio>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <functional>
#include <sstream>
#include <thread>
#include <memory>
#include "quote-1017-lxl.h"


using namespace std;
using namespace chronos;

struct FILEHEAD {
	char maintype[32];
	int  main_shmid;
	int  data_shmid;
	int  bytes_per_record;
	int  recnum_per_block;
	int  updatetm;
	int  recnum;
	char flags[4]; // 固定为shm //
	int  version;  // 目前为101 //
};


std::string date;

void Snapshot2str(Snapshot *ptr, string &str)
{
	stringstream ss;
	ss << ptr->ukey << ","
		<< ptr->trday << ","
		<< ptr->timeus << ","
		<< ptr->recvus << ","
		<< ptr->status << ","
		<< ptr->pre_close << ","
		<< ptr->high << ","
		<< ptr->low << ","
		<< ptr->open << ","
		<< ptr->last << ","
		<< ptr->match_num << ","
		<< ptr->volume << ","
		<< ptr->turnover << ",";

	for (int i = 0; i < 10; ++i)
		ss << ptr->info[i] << ",";
	for (int i = 0; i < 10; ++i)
		ss << ptr->ask_price[i] << ",";
	for (int i = 0; i < 10; ++i)
		ss << ptr->ask_volume[i] << ",";
	for (int i = 0; i < 10; ++i)
		ss << ptr->bid_price[i] << ",";
	for (int i = 0; i < 10; ++i)
		ss << ptr->bid_volume[i] << ",";
	ss << ptr->ask_orders_num << ","
		<< ptr->bid_orders_num << ",";
	for (int i = 0; i < 50; ++i)
		ss << ptr->ask_queue[i] << ",";
	for (int i = 0; i < 49; ++i)
		ss << ptr->bid_queue[i] << ",";
	ss << ptr->bid_queue[49] << endl;
	str = ss.str();
}
void Order2str(Order *ptr, string &str)
{
	stringstream ss;
	ss << ptr->ukey << ","
		<< ptr->trday << ","
		<< ptr->timeus << ","
		<< ptr->recvus << ","
		<< ptr->index << ","
		<< ptr->price << ","
		<< ptr->volume << ","
		<< ptr->order_type << endl;

	str = ss.str();
}
void Trans2str(Transaction* ptr, string &str)
{
	stringstream ss;
	ss << ptr->ukey << ","
		<< ptr->trday << ","
		<< ptr->timeus << ","
		<< ptr->recvus << ","
		<< ptr->index << ","
		<< ptr->price << ","
		<< ptr->volume << ","
		<< ptr->ask_order << ","
		<< ptr->bid_order << ","
		<< ptr->trade_type << endl;
	str = ss.str();
}
void Orderque2str(OrderQueue *ptr, string &str)
{
	stringstream ss;
	ss << ptr->ukey << ","
		<< ptr->trday << ","
		<< ptr->timeus << ","
		<< ptr->recvus << ","
		<< ptr->side << ","
		<< ptr->price << ","
		<< ptr->orders_num << ",";
	for (int i = 0; i < 49; ++i)
		ss << ptr->queue[i] << ",";
	ss << ptr->queue[49] << endl;
	str = ss.str();
}

template<typename T>
void ReadFile(string FileName, function<void(T*, string&)>TransToCsv)
{
	T temp = { 0 };
	FILEHEAD FileHead = { 0 };

	ifstream fin(FileName);
	ofstream fout(FileName + ".csv");
	cout << "starting Trans " << FileName << ".csv" << endl;
	fin.read((char*)&FileHead, sizeof(FileHead));
	int pCount = FileHead.recnum;
	int size = FileHead.bytes_per_record;
	while (pCount--)
	{
		string str;
		fin.read((char*)&temp, size);
		TransToCsv(&temp, str);
		fout << str;
	}
	fout.close();
	fin.close();
	cout << "生成数据成功" << FileName << ".csv" << endl;
}

bool Test()
{
	ifstream fin;
	string Path = "/UKData/TDF/fqy/QUOTE/" + date + "/";
	string FileName;

	vector<thread> ReadThread;
	FileName = Path + date + "_Snapshot";
	auto f1 = bind(ReadFile<Snapshot>, FileName, Snapshot2str);
	ReadThread.emplace_back(f1);

	FileName = Path + date + "_Order";
	auto f2 = bind(ReadFile<Order>, FileName, Order2str);
	ReadThread.emplace_back(f2);

	FileName = Path + date + "_OrderQueue";
	auto f3 = bind(ReadFile<OrderQueue>, FileName, Orderque2str);
	ReadThread.emplace_back(f3);

	FileName = Path + date + "_Transaction";
	ReadFile<Transaction>(FileName, Trans2str);
	auto f4 = bind(ReadFile<Transaction>, FileName, Trans2str);
	ReadThread.emplace_back(f4);

	for (auto &it : ReadThread)
	{
		it.join();
	}
	return true;
}

int main(int argc,char** argv)
{
	std::string date = argv[1];
	Test();
    printf("hello from To_csv!\n");
    return 0;
}