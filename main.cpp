#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "list.h"
#include "testing.h"

int main() {

    printf("dd");

    const char* html_filename = "out_br.html";

    FILE* out_html = fopen(html_filename, "w");
    fclose(out_html);
    out_html = fopen(html_filename, "a");

    ListInfo list1 = {};
    INIT_LIST(list1, 10);


    SizeTestFunc(&list1);
    ListDump(&list1, out_html, HTMLFileMode);

    OrderTestFunc(&list1);
    ListDump(&list1, out_html, HTMLFileMode);

    ListDtor(&list1);
    //ListDtor(&list2);

    fclose(out_html);

    printf("COMMIT_ME_I_WORK!\n");

    return 0;

}


