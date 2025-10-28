#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "list.h"

int main() {

    ListInfo list1 = {};
    INIT_LIST(list1, 10);

    AddValueAfterPosition(&list1, 303, 0);
    AddValueAfterPosition(&list1, 313, 1);
    AddValueAfterPosition(&list1, 323, 2);
    AddValueAfterPosition(&list1, 333, 2);
    AddValueAfterPosition(&list1, 343, 2);
    AddValueAfterPosition(&list1, 353, 1);
    AddValueAfterPosition(&list1, 363, 3);


    ListDump(&list1);




    printf("ffff");
    ListDtor(&list1);
    printf("ffff");
    return 0;

}


