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



    /*
    //test1
    AddValueAfterPosition(&list1, 111, 0);
    AddValueAfterPosition(&list1, 222, 1);
    list1.data[5].next = 2;
    ListDump(&list1, out_html, HTMLFileMode);
    */

    /*test2
    AddValueAfterPosition(&list1, 111, 0);
    AddValueAfterPosition(&list1, 222, 1);
    AddValueAfterPosition(&list1, 333, 2);
    AddValueAfterPosition(&list1, 444, 3);
    list1.data[7].elem = 5;
    */

    /*test2
    AddValueAfterPosition(&list1, 111, 0);
    AddValueAfterPosition(&list1, 222, 1);
    AddValueAfterPosition(&list1, 333, 2);
    AddValueAfterPosition(&list1, 444, 3);
    list1.data[2].next = 33;
    */


    AddValueAfterPosition(&list1, 111, 0);
    AddValueAfterPosition(&list1, 222, 1);
    AddValueAfterPosition(&list1, 333, 2);
    AddValueAfterPosition(&list1, 444, 3);
    list1.data[2].next = 0;

    ListDump(&list1, out_html, HTMLFileMode);
    ListDtor(&list1);

    fclose(out_html);

    printf("COMMIT_ME_I_WORK!\n");

    return 0;

}


