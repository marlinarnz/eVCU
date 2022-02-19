// Based on a project by Steven Cybinski
// GitHub documentation: https://github.com/StevenCyb/SecuredLinkedList
// Modifications:
// - replaced std::mutex with FreeRTOS mutex
// - added object initiliser list (important for functionality)
// - added clear function to destructor to prevent heap issues
// - delete all nodes in clear function to prevent heap issues

#ifndef SecuredLinkedList_h
#define SecuredLinkedList_h


template<class T>
struct SecuredLinkedListNode {
	T data;
	SecuredLinkedListNode<T> *next;
};

template<class T>
class SecuredLinkedList {
	private:
		SemaphoreHandle_t mtx;
		unsigned int listSize;
		SecuredLinkedListNode<T> *root;
		SecuredLinkedListNode<T> *leaf;
		virtual unsigned int sizeUnsecured();
		virtual void pushUnsecured(T t);
		virtual T popUnsecured();
		virtual void addUnsecured(unsigned int index, T t);
		virtual T getUnsecured(unsigned int index);
		virtual void unshiftUnsecured(T t);
		virtual T shiftUnsecured();
		virtual void clearUnsecured();
	public:
		SecuredLinkedList(void);
		~SecuredLinkedList(void);
		virtual unsigned int size();
		virtual void push(T t);
		virtual T pop();
		virtual void add(unsigned int index, T t);
		virtual T get(unsigned int index);
		virtual void unshift(T t);
		virtual T shift();
		virtual void clear();
};

template<typename T>
SecuredLinkedList<T>::SecuredLinkedList()
  : listSize(0), mtx(NULL), root(NULL), leaf(NULL)
{
	mtx = xSemaphoreCreateMutex();
}

template<typename T>
SecuredLinkedList<T>::~SecuredLinkedList() {
	vSemaphoreDelete(mtx);
  clearUnsecured();
}

template<typename T>
unsigned int SecuredLinkedList<T>::size() {
	xSemaphoreTake(mtx, portMAX_DELAY);
	unsigned int returnVal = sizeUnsecured();
  xSemaphoreGive(mtx);
  return returnVal;
}
template<typename T>
unsigned int SecuredLinkedList<T>::sizeUnsecured() {
	return listSize;
}

template<typename T>
void SecuredLinkedList<T>::push(T t) {
  xSemaphoreTake(mtx, portMAX_DELAY);
	pushUnsecured(t);
  xSemaphoreGive(mtx);
}
template<typename T>
void SecuredLinkedList<T>::pushUnsecured(T t) {
	SecuredLinkedListNode<T> *cache = new SecuredLinkedListNode<T>();
	cache->data = t;
	if(listSize <= 0) {
		root = cache;
		leaf = cache;
		listSize += 1;
	} else if(listSize == 1) {
		root->next = cache;
		leaf = cache;
		listSize += 1;
	} else {
		leaf->next = cache;
		leaf = cache;
		listSize += 1;
	}
}

template<typename T>
T SecuredLinkedList<T>::pop() {
  xSemaphoreTake(mtx, portMAX_DELAY);
	T returnVal = popUnsecured();
  xSemaphoreGive(mtx);
  return returnVal;
}
template<typename T>
T SecuredLinkedList<T>::popUnsecured() {
	if(listSize <= 0) {
		return T();
	} else if(listSize > 1) {
		T cache = leaf->data;
		SecuredLinkedListNode<T> *previous = root;
		while(previous->next->next != NULL) {
			previous = previous->next;
		}
		leaf = previous;
		leaf->next = NULL;
		listSize -= 1;
		return cache;
	} else {
		T cache = root->data;
		root = NULL;
		leaf = NULL;
		listSize -= 1;
		return cache;
	}
}

template<typename T>
void SecuredLinkedList<T>::add(unsigned int index, T t) {
  xSemaphoreTake(mtx, portMAX_DELAY);
	addUnsecured(index, t);
  xSemaphoreGive(mtx);
}
template<typename T>
void SecuredLinkedList<T>::addUnsecured(unsigned int index, T t) {
	if(index <= 0) {
		unshiftUnsecured(t);
	} else if (index >= (listSize - 1)) {
		pushUnsecured(t);
	} else {
		SecuredLinkedListNode<T> *previous = root;
		for(unsigned int i = 1; i < index; i++) {
			previous = previous->next;
		}
		SecuredLinkedListNode<T> *cache = new SecuredLinkedListNode<T>();
		cache->data = t;
		cache->next = previous->next;
		previous->next = cache;
		listSize += 1;
	}
}

template<typename T>
T SecuredLinkedList<T>::get(unsigned int index) {
  xSemaphoreTake(mtx, portMAX_DELAY);
	T returnVal = getUnsecured(index);
  xSemaphoreGive(mtx);
  return returnVal;
}
template<typename T>
T SecuredLinkedList<T>::getUnsecured(unsigned int index) {
	if(index <= 0) {
		return root->data;
	} else {
		SecuredLinkedListNode<T> *previous = root;
		for(unsigned int i = 0; i < index; i++) {
			previous = previous->next;
		}
		return previous->data;
	}
}

template<typename T>
void SecuredLinkedList<T>::unshift(T t) {
  xSemaphoreTake(mtx, portMAX_DELAY);
	unshiftUnsecured(t);
  xSemaphoreGive(mtx);
}
template<typename T>
void SecuredLinkedList<T>::unshiftUnsecured(T t) {
	if(listSize <= 0) {
		pushUnsecured(t);
	} else {
		SecuredLinkedListNode<T> *cache = new SecuredLinkedListNode<T>();
		cache->data = t;
		cache->next = root;
		root = cache;
		listSize += 1;
		if(listSize == 2) {
			leaf = root->next;
		}
	}
}

template<typename T>
T SecuredLinkedList<T>::shift() {
  xSemaphoreTake(mtx, portMAX_DELAY);
	T returnVal = shiftUnsecured();
  xSemaphoreGive(mtx);
  return returnVal;
}
template<typename T>
T SecuredLinkedList<T>::shiftUnsecured() {
	if(listSize <= 0) {
		return T();
	}
	if(listSize > 1) {
		T cache = root->data;
		root = root->next;
		listSize -= 1;
		return cache;
	} else {
		return popUnsecured();
	}
}

template<typename T>
void SecuredLinkedList<T>::clear() {
  xSemaphoreTake(mtx, portMAX_DELAY);
	clearUnsecured();
  xSemaphoreGive(mtx);
}
template<typename T>
void SecuredLinkedList<T>::clearUnsecured() {
	SecuredLinkedListNode<T> *tmp = root;
  while(root!=NULL) {
    tmp = root;
    root = root->next;
    delete tmp; // Delete item
    listSize--; // Decrease counter
  }
  leaf = NULL;
}

#endif
