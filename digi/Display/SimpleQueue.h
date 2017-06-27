#ifndef digi_Display_SimpleQueue_h
#define digi_Display_SimpleQueue_h


namespace digi {

/// @addtogroup Display
/// @{

template <typename Type, int size>
class SimpleQueue
{
public:

	SimpleQueue()
		: head(0), tail(0) {}
	
	bool isEmpty()
	{
		return this->tail == this->head;		
	}
	
	bool add(const Type& value)
	{
		int newHead = (this->head + 1) % size;

		// check if full
		if (newHead == this->tail)
			return false;
		
		this->queue[this->head] = value;
		this->head = newHead;
		return true;
	}
	
	bool add(const Type* values, size_t numValues)
	{
		int head = this->head;
		while (numValues > 0)
		{
			int newHead = (head + 1) % size;

			// check if full
			if (newHead == this->tail)
				return false;
			
			this->queue[head] = *values;
			head = newHead;
			
			++values;
			--numValues;
		}
		this->head = head;
		return true;
	}

	Type get(const Type& defaultValue)
	{
		// check if empty
		if (this->tail == this->head)
			return defaultValue;

		Type tmp = this->queue[this->tail];
		this->tail = (this->tail + 1) % size;
		return tmp;
	}

protected:
	int head;
	int tail;
	Type queue[size];
};
	
/// @}

} // namespace digi

#endif
