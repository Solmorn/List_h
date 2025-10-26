#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "list.h"

int main() {

    ListInfo list1 = {};
    INIT_LIST(list1, 10);
    ListDump(&list1);

    AddValueAfterPosition(&list1, 303, 0);
    ListDump(&list1);
    AddValueAfterPosition(&list1, 313, 1);
    ListDump(&list1);
    AddValueAfterPosition(&list1, 323, 2);
    ListDump(&list1);

    DeletePosition(&list1, 3);
    ListDump(&list1);
    DeletePosition(&list1, 2);
    ListDump(&list1);

    AddValueAfterPosition(&list1, 313, 1);
    ListDump(&list1);
    AddValueAfterPosition(&list1, 323, 2);
    ListDump(&list1);

    FilingHTML(&list1, "out.html");

    printf("ffff");
    ListDtor(&list1);
    printf("ffff");
    return 0;

}


