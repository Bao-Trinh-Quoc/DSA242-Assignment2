#ifndef HEAP_H
#define HEAP_H
#include <memory.h>
#include "heap/IHeap.h"
#include <sstream>
#include <iostream>
#include "list/XArrayList.h"
using namespace std;
/*
 * function pointer: int (*comparator)(T& lhs, T& rhs)
 *      compares objects of type T given in lhs and rhs.
 *      return: sign of (lhs - rhs)
 *              -1: of lhs < rhs
 *              0 : lhs == rhs
 *              +1: ls > rhs
 * 
 * function pointer: void (*deleteUserData)(Heap<T>* pHeap)
 *      remove user's data in case that T is a pointer type
 *      Users should pass &Heap<T>::free for "deleteUserData"
 * 
 */
template<class T>
class Heap: public IHeap<T>{
public:
    class Iterator; //forward declaration
    
protected:
    T *elements;    //a dynamic array to contain user's data
    int capacity;   //size of the dynamic array
    int count;      //current count of elements stored in this heap
    int (*comparator)(T& lhs, T& rhs);      //see above
    void (*deleteUserData)(Heap<T>* pHeap); //see above
    
public:
    Heap(   int (*comparator)(T& , T&)=0, 
            void (*deleteUserData)(Heap<T>*)=0 );
    
    Heap(const Heap<T>& heap); //copy constructor 
    Heap<T>& operator=(const Heap<T>& heap); //assignment operator
    
    ~Heap();
    
    //Inherit from IHeap: BEGIN
    void push(T item);
    T pop();
    const T peek();
    void remove(T item, void (*removeItemData)(T)=0);
    bool contains(T item);
    int size();
    void heapify(T array[], int size);
    void clear();
    bool empty();
    string toString(string (*item2str)(T&)=0 );
    //Inherit from IHeap: END
    
    void println(string (*item2str)(T&)=0 ){
        cout << toString(item2str) << endl;
    }
    
    Iterator begin(){
        return Iterator(this, true);
    }
    Iterator end(){
        return Iterator(this, false);
    }
    
public:
    /* if T is pointer type:
     *     pass the address of method "free" to Heap<T>'s constructor:
     *     to:  remove the user's data (if needed)
     * Example:
     *  Heap<Point*> heap(&Heap<Point*>::free);
     *  => Destructor will call free via function pointer "deleteUserData"
     */
    static void free(Heap<T> *pHeap){
        for(int idx=0; idx < pHeap->count; idx++) delete pHeap->elements[idx];
    }
    
    
private:
    bool aLTb(T& a, T& b){
        return compare(a, b) < 0;
    }
    int compare(T& a, T& b){
        if(comparator != 0) return comparator(a, b);
        else{
            if (a < b) return -1;
            else if(a > b) return 1;
            else return 0;
        }
    }
    
    void ensureCapacity(int minCapacity); 
    void swap(int a, int b);
    void reheapUp(int position);
    void reheapDown(int position);
    int getItem(T item);
    
    void removeInternalData();
    void copyFrom(const Heap<T>& heap);
public:
    void heapsort(XArrayList<T>& arrayList) {
        // Clear the heap first
        clear();
        
        T* tempArray = new T[arrayList.size()];
        int originalSize = arrayList.size();

        for (int i = 0; i < arrayList.size(); i++) {
            push(arrayList.get(i));
        }
        // Pop elements one by one and store them in tempArray
        for (int i = 0; i < arrayList.size(); i++) {
            tempArray[i] = pop();
            
            XArrayList<T> currentState;
            // Add sorted elements (from tempArray)
            for (int j = 0; j <= i; j++) {
                currentState.add(tempArray[j]);
            }
            // Add remaining unsorted elements (from original arrayList)
            for (int j = i+1; j < arrayList.size(); j++) {
                currentState.add(arrayList.get(j));
            }
            
            currentState.println();
        }
        
        arrayList.clear();
        for (int i = 0; i < originalSize; i++) {
            arrayList.add(tempArray[i]);
        }
        
        delete[] tempArray;
    }
    
//////////////////////////////////////////////////////////////////////
////////////////////////  INNER CLASSES DEFNITION ////////////////////
//////////////////////////////////////////////////////////////////////
    
public:
    
    //Iterator: BEGIN
    class Iterator{
    private:
        Heap<T>* heap;
        int cursor;
    public:
        Iterator(Heap<T>* heap=0, bool begin=0){
            this->heap = heap;
            if(begin && (heap !=0)) cursor = 0;
            if(!begin && (heap !=0)) cursor = heap->size();
        }
        Iterator& operator=(const Iterator& iterator){
            this->heap = iterator.heap;
            this->cursor = iterator.cursor;
            return *this;
        }
        
        T& operator*(){
            return this->heap->elements[cursor];
        }
        bool operator!=(const Iterator& iterator){
            return this->cursor != iterator.cursor;
        }
        // Prefix ++ overload 
        Iterator& operator++(){
            cursor++;
            return *this; 
        }
        // Postfix ++ overload 
        Iterator operator++(int){
            Iterator iterator = *this; 
            ++*this; 
            return iterator; 
        }
        void remove(void (*removeItemData)(T)=0){
            this->heap->remove(this->heap->elements[cursor], removeItemData);
        }
    };
    //Iterator: END
};


//////////////////////////////////////////////////////////////////////
////////////////////////     METHOD DEFNITION      ///////////////////
//////////////////////////////////////////////////////////////////////

template<class T>
Heap<T>::Heap(
        int (*comparator)(T&, T&), 
        void (*deleteUserData)(Heap<T>* ) ){
    this->capacity = 10;
    this->count = 0;
    this->elements = new T[capacity];
    this->comparator = comparator;
    this->deleteUserData = deleteUserData;
}
template<class T>
Heap<T>::Heap(const Heap<T>& heap){
    copyFrom(heap);
}

template<class T>
Heap<T>& Heap<T>::operator=(const Heap<T>& heap){
    if (this != &heap) {
        removeInternalData();
        copyFrom(heap);
     }
  
    return *this;
}


template<class T>
Heap<T>::~Heap(){
    removeInternalData();
}

template<class T>
void Heap<T>::push(T item){ //item  = 25
    // ensureCapacity(count + 1); //[18, 15, 13, 25 , , ]
    // elements[count] = item;
    // count += 1; //count = 
    // reheapUp(count - 1); // [18, 15, 13, 25 , , ]

    // My code
    ensureCapacity(count + 1);  // ensure enough space
    elements[count] = item;
    reheapUp(count);  // reheap up
    this->count++;
}
/*
      18
     /  \
    15   13
    /
  25 
 =>
      25
     /  \
    18   13
    /
  15 
=> array: [25, 18, 13, 15, , ]
           0   1    2   3
 
 */
template<class T>
T Heap<T>::pop(){
    if (this->count == 0) 
        throw std::underflow_error("Calling to peek with the empty heap.");
    
    T item = this->elements[0];
    this->elements[0] = this->elements[this->count - 1]; // last element to root
    this->count--;
    reheapDown(0);  // reheapify the root
    return item;
}

/*
      15
     /  \
    18   13
 => ReheapDown
      18
     /  \
    15   13
=> Array: [18, 15, 13, , , ]
 */

template<class T>
const T Heap<T>::peek(){
    if(count == 0) 
        throw std::underflow_error("Calling to peek with the empty heap.");
    return this->elements[0];
}


template<class T>
void Heap<T>::remove(T item, void (*removeItemData)(T)){
    // int foundIdx = this->getItem(item);
    
    // //CASE 1: not found
    // if(foundIdx == -1) return;

    // //CASE 2: found at foundIdx
    // elements[foundIdx] = elements[count - 1]; 
    // count -= 1;
    // reheapDown(foundIdx);
    // if(removeItemData != NULL) removeItemData(item); //free item's memory
    
    // My code
    int pos = getItem(item);
    if (pos == -1) {
       return; // item not found
    }
 
    if (removeItemData != nullptr) {
       removeItemData(this->elements[pos]);  // remove user's data if needed
    }
 
    // move last element to the position of the item to be removed
    this->elements[pos] = this->elements[this->count - 1];
    this->count--;

    // Check if we need to reheap up or down
    if (pos > 0) {
        int parent = (pos - 1) / 2;
        if (aLTb(this->elements[pos], this->elements[parent])) {
            reheapUp(pos);  // new element is smaller than parent
        } else {
            reheapDown(pos); // new element is larger than parent
        }
    }
    else {
        // If we are at the root, we need to reheap down
        reheapDown(pos);
    }
}

/*
//April 03, 2023
template<class T>
void Heap<T>::remove_bck(T item, void (*removeItemData)(T)){
    int foundIdx = this->getItem(item);
    
    //CASE 1: not found
    if(foundIdx == -1) return;

    //CASE 2: found at 0 => use pop
    if(foundIdx == 0){
        T removed = this->pop();
        if(removeItemData != 0) removeItemData(removed);
        return;
    }
    
    //CASE 3: found at anywhere else
    //Remove item's data
    if(removeItemData != 0) removeItemData(elements[foundIdx]);
    
    //Remove item: shift left
    int copyCount = (count - 1) - (foundIdx + 1) + 1;
    memcpy(&elements[foundIdx], &elements[foundIdx + 1], copyCount*sizeof(T));
    count -= 1;

    //Determine valid heap [0-> (new size -1)]
    int startOldData = foundIdx;
    int lastOldData = count - 1;
    count = foundIdx;

    //reheapify from startOldData
    for(int idx=startOldData; idx <= lastOldData; idx++)
        push(elements[idx]);
}
*/

template<class T>
bool Heap<T>::contains(T item){
    // bool found = false;
    // for(int idx=0; idx < count; idx++){
    //     if(compare(elements[idx], item) == 0){
    //         found = true;
    //         break;
    //     }
    // }
    // return found;
    return getItem(item) != -1;
}

template<class T>
int Heap<T>::size(){
    return this->count;
}

template<class T>
void Heap<T>::heapify(T array[], int size){
    clear();

    ensureCapacity(size);
    for (int i = 0; i < size; i++) {
        elements[i] = array[i];
    }
    this->count = size;

    for (int i = (size / 2) - 1; i >= 0; i--) {
        reheapDown(i);
    }
}

template<class T>
void Heap<T>::clear(){
    removeInternalData();
    
    this->capacity = 10;
    this->count = 0;
    this->elements = new T[capacity];
}

template<class T>
bool Heap<T>::empty(){
    return this->count == 0;
}

template<class T>
string Heap<T>::toString(string (*item2str)(T&)){
    stringstream os;
    if(item2str != 0){
        os << "[";
        for(int idx=0; idx < count -1; idx++)
            os << item2str(elements[idx]) << ",";
        if(count > 0) os << item2str(elements[count - 1]);
        os << "]";
    }
    else{
        os << "[";
        for(int idx=0; idx < count -1; idx++)
            os << elements[idx] << ",";
        if(count > 0) os << elements[count - 1];
        os << "]";
    }
    return os.str();
}


//////////////////////////////////////////////////////////////////////
//////////////////////// (private) METHOD DEFNITION //////////////////
//////////////////////////////////////////////////////////////////////

template<class T>
void Heap<T>::ensureCapacity(int minCapacity){
    if(minCapacity >= capacity){
        //re-allocate 
        int old_capacity = capacity;
        // make sure the capacity is at least minCapacity
        // capacity = old_capacity + (old_capacity >> 2);
        capacity = max(minCapacity, old_capacity + (old_capacity >> 2));
        try{
            T* new_data = new T[capacity];
            //OLD: memcpy(new_data, elements, capacity*sizeof(T));
            memcpy(new_data, elements, old_capacity*sizeof(T));
            delete []elements;
            elements = new_data;
        }
        catch(std::bad_alloc e){
            e.what();
        }
    }
}

template<class T>
void Heap<T>::swap(int a, int b){
    T temp = this->elements[a];
    this->elements[a] = this->elements[b];
    this->elements[b] = temp;
}

template<class T>
void Heap<T>::reheapUp(int position){
    // if(position <= 0) return;
    // int parent = (position-1)/2;
    // if(aLTb(this->elements[position], this->elements[parent])){
    //     this->swap(position, parent);
    //     reheapUp(parent);
    // }

    // My code
    int parent = (position - 1) / 2;
    while (position > 0 && aLTb(elements[position], elements[parent])) {
       swap(position, parent);
       position = parent;
       parent = (position - 1) / 2;
    }
}

template<class T>
void Heap<T>::reheapDown(int position){
    // int leftChild = position*2 + 1;
    // int rightChild = position*2 + 2;
    // int lastPosition = this->count - 1;

    // if(leftChild <= lastPosition){
    //     int smallChild = leftChild; //by default => leftChild valid but rightChild invalid
    //     if(rightChild <= lastPosition){
    //         if(aLTb(this->elements[leftChild], this->elements[rightChild])) 
    //             smallChild = leftChild;
    //         else smallChild = rightChild;
    //     }

    //     if(aLTb(this->elements[smallChild], this->elements[position])){
    //         this->swap(smallChild, position);
    //         reheapDown(smallChild);
    //     }
    // }

    // My code
    int left = position * 2 + 1;  // left child
    int right = position * 2 + 2; // right child
    int smaller = left;
    while (left < count) {
        if (right < count && aLTb(elements[right], elements[left])) {
            smaller = right;
        }
        if (aLTb(elements[smaller], elements[position])) {
            swap(position, smaller);
            position = smaller;
            left = 2 * position + 1;
            right = 2 * position + 2;
            smaller = left;
        }
        else {
            break;
        }
    }
}

template<class T>
int Heap<T>::getItem(T item){
    int foundIdx = -1;
    for(int idx=0; idx < this->count; idx++){
        if(compare(elements[idx], item) == 0){
            foundIdx = idx;
            break;
        }
    }
    return foundIdx;
}

template<class T>
void Heap<T>::removeInternalData(){
    if(this->deleteUserData != 0) deleteUserData(this); //clear users's data if they want
    delete []elements;
}

template<class T>
void Heap<T>::copyFrom(const Heap<T>& heap){
    capacity = heap.capacity;
    count = heap.count;
    elements = new T[capacity];
    this->comparator = heap.comparator;
    this->deleteUserData = heap.deleteUserData;
    
    //Copy items from heap:
    // for(int idx=0; idx < heap.size(); idx++){
    //     this->elements[idx] = heap.elements[idx];
    // }

    // Copy items from heap:
    for(int idx=0; idx < heap.count; idx++){
        // original shallow copy
        // this->elements[idx] = heap.elements[idx]; 

        // deep copy
        // this->elements[idx] = new int(*heap.elements[idx]);

        // Check if we're dealing with Heap<int*> specifically
        if constexpr (std::is_same<T, int*>::value) {
        // Deep copy for int* to avoid double-free issues
        this->elements[idx] = new int(*heap.elements[idx]);
        } else {
        // Simple copy for all other types
        this->elements[idx] = heap.elements[idx];
        }
    }
}

#endif
