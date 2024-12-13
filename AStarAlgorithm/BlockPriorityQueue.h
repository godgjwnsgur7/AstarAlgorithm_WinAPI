#pragma once
#include "framework.h"
#include <queue>

class BlockPriorityQueue
{
	vector<Block*> heap;
public:
	BlockPriorityQueue() {}

	int GetCount() { return heap.size(); }

	void Enqueue(Block* block)
	{
		heap.push_back(block);
		int now = heap.size() - 1;

		while (now > 0)
		{
			int next = (now - 1) / 2;

			if (heap[now]->fCost > heap[next]->fCost)
				break;
			else if (heap[next]->fCost == heap[now]->fCost && heap[next]->hCost < heap[now]->hCost)
				break;

			Block* temp = heap[now];
			heap[now] = heap[next];
			heap[next] = temp;
			now = next;
		}
	}

	Block* Dequeue()
	{
		if (heap.size() == 0)
		{
			return nullptr;
		}

		Block* ret = heap[0];
		int lastIndex = heap.size() - 1;
		heap[0] = heap[lastIndex];
		heap.erase(heap.begin() + lastIndex);
		lastIndex -= 1;
		int now = 0;
		
		while (true)
		{
			int left = 2 * now + 1;
			int right = 2 * now + 2;
			int next = now;

			if (left <= lastIndex)
			{
				if (heap[next]->fCost > heap[left]->fCost)
					next = left;
				else if (heap[next]->fCost == heap[left]->fCost && heap[next]->hCost > heap[left]->hCost)
					next = left;
			}

			if (right <= lastIndex)
			{
				if (heap[next]->fCost > heap[right]->fCost)
					next = right;
				else if (heap[next]->fCost == heap[right]->fCost && heap[next]->hCost > heap[right]->hCost)
					next = right;
			}

			if (next == now)
				break;

			Block* temp = heap[now];
			heap[now] = heap[next];
			heap[next] = temp;
			now = next;
		}

		return ret;
	}
};