#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "list.h"
#include "testing.h"

int main() {

    const char* html_filename = "out_br.html";

    FILE* out_html = fopen(html_filename, "w");
    fclose(out_html);
    out_html = fopen(html_filename, "a");

    ListInfo list1 = {};
    INIT_LIST(list1, 10);

    ListDump(&list1, out_html, HTMLFileMode);

    //SizeTestFunc(&list1);
    AddValueAfterPosition(&list1, 111, 0);
    AddValueAfterPosition(&list1, 222, 1);
    AddValueAfterPosition(&list1, 333, 2);
    AddValueAfterPosition(&list1, 444, 3);
    AddValueAfterPosition(&list1, 555, 4);
    AddValueAfterPosition(&list1, 666, 5);
    AddValueAfterPosition(&list1, 777, 6);
    //list1.data[3].prev = 55;
//
    //list1.data[3].next = 5;
    ListDump(&list1, out_html, HTMLFileMode);


    //OrderTestFunc(&list1);
    //ListDump(&list1, out_html, HTMLFileMode);

    ListDtor(&list1);

    fclose(out_html);

    printf("COMMIT_ME_I_WORK!\n");

    return 0;

}


