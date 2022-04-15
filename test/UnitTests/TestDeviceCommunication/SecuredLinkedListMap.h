// Created by Steven Cybinski
// GitHub: https://github.com/StevenCyb/SecuredLinkedListMap
// Features changed for this project:
// - replaced std::mutex with FreeRTOS mutex
// - added object initiliser list (important for functionality)
// - added clear function to destructor to prevent heap issues
// - delete all nodes in clear function to prevent heap issues
// - when deleting nodes, delete the value too (specific to this project)

#ifndef SecuredLinkedListMap_h
#define SecuredLinkedListMap_h

template<typename T1, typename T2> 
struct SecuredLinkedListMapNode {
	T1 key;
	T2 value;
	SecuredLinkedListMapNode<T1, T2> *next;
};
template<typename T1, typename T2> 
struct SecuredLinkedListMapElement {
	T1 key;
	T2 value;
};

template<typename T1, typename T2> 
class SecuredLinkedListMap {
	private:
		SemaphoreHandle_t mtx;
		int bucketSize;
		SecuredLinkedListMapNode<T1, T2> *bucketRoot;
		virtual int sizeUnsecured();
		virtual void putUnsecured(T1 key, T2 value);
		virtual bool hasUnsecured(T1 key);
		virtual void getAllUnsecured(SecuredLinkedListMapElement<T1, T2>* copies);
		virtual T2 getUnsecured(T1 key);
		virtual void removeUnsecured(T1 key);
		virtual void clearUnsecured();
	public:
		SecuredLinkedListMap(void);
		~SecuredLinkedListMap(void);
		virtual int size();
		virtual void put(T1 key, T2 value);
		virtual bool has(T1 key);
		virtual void getAll(SecuredLinkedListMapElement<T1, T2>* copies);
		virtual T2 get(T1 key);
		virtual void remove(T1 key);
		virtual void clear();
};

template<typename T1, typename T2> 
SecuredLinkedListMap<T1, T2>::SecuredLinkedListMap()
  : mtx(NULL), bucketSize(0), bucketRoot(NULL)
{
	bucketSize = 0;
	mtx = xSemaphoreCreateMutex();
}

template<typename T1, typename T2> 
SecuredLinkedListMap<T1, T2>::~SecuredLinkedListMap() {
	vSemaphoreDelete(mtx);
	clearUnsecured();
}

template<typename T1, typename T2> 
int SecuredLinkedListMap<T1, T2>::size() {
	xSemaphoreTake(mtx, portMAX_DELAY);
	int returnVal = sizeUnsecured();
	xSemaphoreGive(mtx);
	return returnVal;
}
template<typename T1, typename T2> 
int SecuredLinkedListMap<T1, T2>::sizeUnsecured() {
	return bucketSize;
}

template<typename T1, typename T2> 
void SecuredLinkedListMap<T1, T2>::put(T1 key, T2 value) {
	xSemaphoreTake(mtx, portMAX_DELAY);
	putUnsecured(key, value);
	xSemaphoreGive(mtx);
}
template<typename T1, typename T2> 
void SecuredLinkedListMap<T1, T2>::putUnsecured(T1 key, T2 value) {
	if(bucketSize <= 0) {
		SecuredLinkedListMapNode<T1, T2> *cache = new SecuredLinkedListMapNode<T1, T2>();
		cache->key = key;
		cache->value = value;
		bucketRoot = cache;
		bucketSize += 1;
	} else {
		SecuredLinkedListMapNode<T1, T2> *current = bucketRoot;
		if(current->key == key) {
			current->value = value;
			return;
		}
		while(current->next != NULL) {
			current = current->next;
			if(current->key == key) {
				current->value = value;
				return;
			}
		}
		SecuredLinkedListMapNode<T1, T2> *cache = new SecuredLinkedListMapNode<T1, T2>();
		cache->key = key;
		cache->value = value;
		current->next = cache;
		bucketSize += 1;
	}
}

template<typename T1, typename T2> 
bool SecuredLinkedListMap<T1, T2>::has(T1 key) {
	xSemaphoreTake(mtx, portMAX_DELAY);
	bool returnVal = hasUnsecured(key);
	xSemaphoreGive(mtx);
	return returnVal;
}
template<typename T1, typename T2> 
bool SecuredLinkedListMap<T1, T2>::hasUnsecured(T1 key) {
	SecuredLinkedListMapNode<T1, T2> *current = bucketRoot;
	while(current != NULL) {
		if(current->key == key) {
			return true;
		}
		current = current->next;
	}
	return false;
}

template<typename T1, typename T2> 
void SecuredLinkedListMap<T1, T2>::getAll(SecuredLinkedListMapElement<T1, T2>* copies) {
	xSemaphoreTake(mtx, portMAX_DELAY);
	getAllUnsecured(copies);
	xSemaphoreGive(mtx);
}
template<typename T1, typename T2> 
void SecuredLinkedListMap<T1, T2>::getAllUnsecured(SecuredLinkedListMapElement<T1, T2>* copies) {
	int counter = 0;
	SecuredLinkedListMapNode<T1, T2> *current = bucketRoot;
	while(current != NULL) {
		SecuredLinkedListMapElement<T1, T2> copy = SecuredLinkedListMapElement<T1, T2>();
		copy.key = current->key;
		copy.value = current->value;
		copies[counter] = copy;
		current = current->next;
		counter ++;
	}
}

template<typename T1, typename T2> 
T2 SecuredLinkedListMap<T1, T2>::get(T1 key) {
	xSemaphoreTake(mtx, portMAX_DELAY);
	T2 returnVal = getUnsecured(key);
	xSemaphoreGive(mtx);
	return returnVal;
}
template<typename T1, typename T2> 
T2 SecuredLinkedListMap<T1, T2>::getUnsecured(T1 key) {
	SecuredLinkedListMapNode<T1, T2> *current = bucketRoot;
	if(current->key == key) {
		return current->value;
	}
	while(current->next != NULL) {
		current = current->next;
		if(current->key == key) {
			return current->value;
		}
	}
	return T2();
}

template<typename T1, typename T2> 
void SecuredLinkedListMap<T1, T2>::remove(T1 key) {
	xSemaphoreTake(mtx, portMAX_DELAY);
	removeUnsecured(key);
	xSemaphoreGive(mtx);
}
template<typename T1, typename T2> 
void SecuredLinkedListMap<T1, T2>::removeUnsecured(T1 key) {
	SecuredLinkedListMapNode<T1, T2> *current = bucketRoot;
	if(current->key == key) {
		bucketRoot = current->next;
		bucketSize -= 1;
		return;
	}
	while(current->next != NULL) {
		if(current->next->key == key) {
			current->next = current->next->next;
			bucketSize -= 1;
			return;
		}
		current = current->next;
	}
}

template<typename T1, typename T2> 
void SecuredLinkedListMap<T1, T2>::clear() {
	xSemaphoreTake(mtx, portMAX_DELAY);
	clearUnsecured();
	xSemaphoreGive(mtx);
}
template<typename T1, typename T2> 
void SecuredLinkedListMap<T1, T2>::clearUnsecured() {
  SecuredLinkedListMapNode<T1, T2> *tmp = bucketRoot;
	while(bucketRoot != NULL) {
    tmp = bucketRoot;
		bucketRoot = bucketRoot->next;
    delete tmp->value; // specific to this project: delete object on heap
    delete tmp;
    bucketSize--;
	}
}

#endif
