#include <malloc.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "array.h"


// Construction / Destruction

Array array_new(long capacity){
    assert(capacity > 0);
    return (Array){malloc(sizeof(long)*capacity), 0, 0, capacity};
}

void array_destroy(Array a){
    free(a.data);
}


// Primitives

long array_empty(Array a){
    return a.back <= a.front;
}

long array_front(Array a){
    return a.data[a.front];
}

long array_back(Array a){
    return a.data[a.back - 1];
}

void array_popFront(Array* a){
    a->front++;
}

void array_popBack(Array* a){
    a->back--;
}

Array array_save(Array a){
    return (Array){a.data, a.front, a.back, a.capacity};
}


// Iteration

void array_foreach(Array a, void fn(long)){
    for(Array b = array_save(a); !array_empty(b); array_popFront(&b)){
        fn(array_front(b));
    }
}

void array_foreachReverse(Array a, void fn(long)){
    for(Array b = array_save(a); !array_empty(b); array_popBack(&b)){
        fn(array_back(b));
    }
}

static void _array_printSingleLongHelper(long i){
    printf(", %ld", i);
}

void array_print(Array a){
    printf("Array:{");
    if(!array_empty(a)){
        printf("%ld", array_front(a));
        array_popFront(&a);
    }
    array_foreach(a, _array_printSingleLongHelper);
    printf("}\n");
}


// Capacity

long array_length(Array a){
    return a.back - a.front;
}

void array_reserve(Array* a, long capacity){
	Array new_a = array_new(capacity);
	// copy over the old array
	//new_a.front = a->front;
	//new_a.back = a->back;
	memcpy(new_a.data, a->data, a->capacity * sizeof(long));
	a->capacity = new_a.capacity;
	// free old array
	array_destroy(*a);
	a->data = new_a.data;
}


// Modifiers

/*
a = new_arr(2)
(a = [], back = 0)

insertBack(1)
insertBack(2)

(a = [1, 2], back = 2)

*/



void array_insertBack(Array* a, long stuff){
	if (a->back >= a->capacity) {
		array_reserve(a, 2 * a->capacity);
		printf("pointer %p, increased capacity by a factor of 2, now at: %d\n", a->data, a->capacity);
	}
	a->data[a->back++] = stuff;
}


