#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "list.h"

int main() {

    ListInfo list1 = {};
    INIT_LIST(list1, 10);

    AddValueAfterPosition(&list1, 333, 0);
    AddValueAfterPosition(&list1, 336, 1);

    ListDump(&list1);
    printf("ffff");


    return 0;
}

