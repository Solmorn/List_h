#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "testing.h"



int SizeTestFunc(ListInfo* list) {
    AddValueAfterPosition(list, 33, 0);
    AddValueAfterPosition(list, 33, 1);
    AddValueAfterPosition(list, 33, 2);
    AddValueBeforePosition(list, 33, 1);
    RemovePositionFromList(list, 3);

    if (list->size == 3) return 1;
    return 0;
}

int OrderTestFunc(ListInfo* list) {
    AddValueAfterPosition(list, 33, 0);
    AddValueAfterPosition(list, 33, 1);
    AddValueAfterPosition(list, 33, 2);
    AddValueBeforePosition(list, 33, 1);
    RemovePositionFromList(list, 3);

    size_t current = list->data[0].next;
    size_t size_cpy = list->size;
    while (current != 0 && size_cpy != 0) {
        size_cpy--;
        current = list->data[current].next;
    }

    if (current != size_cpy) return 0;
    return 1;
}
