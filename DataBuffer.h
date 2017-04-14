#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

class IData{
protected:
public:
	IData();
	virtual ~IData();
	virtual void clear();
};

class DataBuffer{
protected:
	std::queue<void*> buffer;

	std::mutex mtxQueueIOLock;
	std::mutex mtxNewData;
	std::condition_variable cvNewData;
	std::mutex mtxDataPopped;
	std::condition_variable cvDataPopped;

	int maxSize;
	int dropped;

	bool push(void* data);
	bool pop(void*& data);

public:
	DataBuffer();
	virtual ~DataBuffer();
	void setMaxSize(int size);
	void clear();

	int getDropped();
	void clearDropped();
	void waitUntilNew();
	void waitUntilPop();
	bool isFull();
};

