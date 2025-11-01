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

    size_t test_number = 4;

    char why_dump[CHAR_STRING_SIZE] = "";
    char test_changes[CHAR_STRING_SIZE] = "";

    sprintf(why_dump, "TEST %lu START CONDITION DUMP", test_number);
    ListDump(&list1, out_html, HTMLFileMode, why_dump);


    switch (test_number) {
        case 1:
            sprintf(test_changes, "next of data[5] now 2 (was 6)");
            AddValueAfterPosition(&list1, 111, 0, out_html);
            AddValueAfterPosition(&list1, 222, 1, out_html);
            list1.data[5].next = 2;
            break;

        case 2:
            sprintf(test_changes, "elem of data[5] now is not a POISON");
            AddValueAfterPosition(&list1, 111, 0, out_html);
            AddValueAfterPosition(&list1, 222, 1, out_html);
            AddValueAfterPosition(&list1, 333, 2, out_html);
            AddValueAfterPosition(&list1, 444, 3, out_html);
            list1.data[7].elem = 5;
            break;

        case 3:
            sprintf(test_changes, "next of data[2] now is not existing node");
            AddValueAfterPosition(&list1, 111, 0, out_html);
            AddValueAfterPosition(&list1, 222, 1, out_html);
            AddValueAfterPosition(&list1, 333, 2, out_html);
            AddValueAfterPosition(&list1, 444, 3, out_html);
            list1.data[2].next = 33;
            break;

        case 4:
            sprintf(test_changes, "next of data[3] now 1 (was 4)");
            AddValueAfterPosition(&list1, 111, 0, out_html);
            AddValueAfterPosition(&list1, 222, 1, out_html);
            AddValueAfterPosition(&list1, 333, 2, out_html);
            AddValueAfterPosition(&list1, 444, 3, out_html);
            list1.data[3].next = 1;
            break;
    }

    sprintf(why_dump, "TEST %lu END CONDITION DUMP ||| %s", test_number, test_changes);
    ListDump(&list1, out_html, HTMLFileMode, why_dump);

    ListDtor(&list1);

    fclose(out_html);

    printf("COMMIT_ME_I_WORK!\n");

    return 0;

}


