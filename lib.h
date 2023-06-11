#ifndef LIB_H
#define LIB_H

// Nothing in here should be specific to any platform
// But we do depend on C standard library (which should be cross platform?)

#include <stdio.h>
#include <stdlib.h>
//#include <assert.h>

#include <x86intrin.h> //@temp

#include <stdint.h>
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

//@ are these 32 and 64 bits for all architectures?
typedef float r32;
typedef double r64;

bool ReadFile(const char *FileName, u8 **Contents, size_t *NumBytes);
bool WriteFile(const char *FileName, u8 *Contents, size_t NumBytes);
bool ReadTextFile(const char *FileName, char **Contents); // returns 0-terminated contents
//bool WriteTextFile(const char *FileName, char **Contents); // expects 0-terminated contents

// Determines the length of an array at compile time.
#define COUNT(a) (sizeof(a) / sizeof(a[0]))

#define MIN(a, b) (a) < (b) ? (a) : (b)
#define MAX(a, b) (a) > (b) ? (a) : (b)

// Using this on a float or a double before casting it to an integer yields a more accurate result.
#define CAST_ROUND(n) ((n) + (((n) > 0) ? 0.5 : -0.5))

#define DEFAULT_INITIALLY_ALLOCATED 1024

template <typename t>
struct array
{
	int Count;
	int Allocated;
	t *Data;
};

template <typename t>
void ArrayInit(array<t> *Array, int InitiallyAllocated = DEFAULT_INITIALLY_ALLOCATED)
{
	Array->Count = 0;
	Array->Allocated = InitiallyAllocated;
	Array->Data = (t *) malloc(Array->Allocated * sizeof(t)); //@
}

template <typename t>
array<t> *ArrayNew()
{
	array<t> *Array = (array<t> *) malloc(sizeof(array<t>));
	ArrayInit(Array);
	return Array;
}

template <typename t>
void ArrayAdd(array<t> *Array, t Element)
{
//	assert(array->count <= array->allocated);
	if (Array->Count == Array->Allocated)
	{
		Array->Allocated *= 2;
		t *P = (t *) malloc(Array->Allocated * sizeof(t)); //@
		for(int I = 0; I < Array->Count; ++I)
		{
			P[I] = Array->Data[I];
		}
		free(Array->Data);
		Array->Data = P;
	}

	Array->Data[Array->Count] = Element;
	Array->Count += 1;
}

template <typename t>
void ArrayAdd(array<t> *a, t e[], u32 n)
{
	for(u32 i = 0; i < n; ++i)
	{
		ArrayAdd(a, e[i]);
	}
}

template <typename t>
void ArrayAddFast(array<t> *Array, t Elements[], u32 NumElements)
{
	int NewNumElements = Array->Count + NumElements;
	if (NewNumElements > Array->Allocated)
	{
		printf("ALLOCATING NEW MEMORY\n");
		int AllocatedTwice = Array->Allocated * 2;
		Array->Allocated = (NewNumElements > AllocatedTwice) ? NewNumElements : AllocatedTwice;

		unsigned long Cycles1 = __rdtsc();
		t *Ptr = (t *) malloc(Array->Allocated * sizeof(t)); //@
		unsigned long Cycles2 = __rdtsc();
		printf("malloc(): %lu cycles\n", Cycles2 - Cycles1);

		unsigned long Cycles3 = __rdtsc();
		for(int i = 0; i < Array->Count; ++i)
		{
			Ptr[i] = Array->Data[i];
		}
		unsigned long Cycles4 = __rdtsc();
		printf("copying: %lu cycles\n", Cycles4 - Cycles3);

		free(Array->Data);
		Array->Data = Ptr;
	}

	int j = Array->Count;
	for(u32 i = 0; i < NumElements; ++i)
	{
		Array->Data[j] = Elements[i];
		++j;
	}
	Array->Count = NewNumElements;
}

template <typename t>
void ArrayRemove(array<t> *Array, int i)
{
	assert(i >= 0 && i < Array->Count);
	for(; i < Array->Count-1; ++i)
	{
		Array->Data[i] = Array->Data[i+1];
	}
	Array->Count -= 1;
}

#endif