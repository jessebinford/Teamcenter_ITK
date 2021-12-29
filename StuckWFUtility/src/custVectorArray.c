#include <stdio.h>
#include <stdlib.h>


typedef struct _custVectorArray
{
    size_t size;
    size_t actual_size;
    int *content;
} custVectorArray;


void custVectorArray_create(custVectorArray *d)
{
    d->actual_size = d->size = 0;
    d->content = NULL;
}

void custVectorArray_append(custVectorArray *d, int v)
{
    if (d->size+1 > d->actual_size)
    {
		size_t new_size;
		if (!d->actual_size) 
		{ 
			new_size = 1;
		}
		else
		{
			new_size = d->actual_size * 2;
		}
		int *temp = realloc(d->content, sizeof(int) * new_size);
		if (!temp)
		{
			fprintf(stderr, "Failed to extend array (new_size=%zu)\n", new_size);
			exit(EXIT_FAILURE);
		}
		d->actual_size = new_size;
		d->content = temp;
    }
    d->content[d->size] = v;
    d->size++;
}

const int* custVectorArray_data(custVectorArray *d)
{
    return d->content;
}


void custVectorArray_destroy(custVectorArray *d)
{
    free(d->content);
    d->content = NULL;
    d->size = d->actual_size = 0;
}


size_t custVectorArray_size(custVectorArray *d)
{
    return d->size;
}