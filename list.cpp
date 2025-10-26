#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "list.h"

#ifdef _DEBUG
static void FillPoison(ListInfo* list);
#endif

static ListElem* AllocateList(ListInfo* list, size_t capacity_got) {

    ListElem* realloc_ptr = (ListElem*)realloc(list->data, capacity_got * sizeof(list_type));

    if (realloc_ptr == nullptr) return nullptr;

    list->capacity = capacity_got;
    list->data = realloc_ptr;


    #ifdef _DEBUG
    FillPoison(list);
    #endif

    for (size_t index = list->capacity / 2; index < list->capacity; index++) {

        list->data[index].next = 0;
        list->data[index].elem = 0;
        list->data[index].prev = index+1;

    }

    printf("syka");

    return realloc_ptr;

}

error_code ListCtor(ListInfo* list, size_t capacity_got, BirthInfo* info_got = nullptr) {
//
    assert(list);

    if (capacity_got > MAX_SIZE_VALUE) return CapacityError;
    if (capacity_got < MIN_SIZE_VALUE) capacity_got = MIN_SIZE_VALUE;

    #ifdef _DEBUG
    list->info = info_got;
    #endif //debug

    ListElem* calloc_ptr = AllocateList(list, capacity_got);
    if (calloc_ptr == nullptr) return AllocationError;

    for (size_t index = 0; index < list->capacity; index++) {

        list->data[index].next = 0;
        list->data[index].elem = 0;
        list->data[index].prev = index+1;
    }

    ASSERT_OK(list);

    return Ok;

}

error_code AddValueAfterPosition(ListInfo* list, list_type value, size_t position) {

    ASSERT_OK(list);

    if (position == list->capacity) {
        ListElem* realloc_ptr = AllocateList(list, list->capacity * LIST_EXPAND_VALUE);
        if (realloc_ptr == nullptr) return AllocationError;
    }

    size_t adding_pos = list->next_place;

    list->next_place = list->data[adding_pos].prev;

    list->data[adding_pos].prev = position;
    list->data[list->data[position].next].prev = adding_pos;


    list->data[adding_pos].elem = value;

    list->data[adding_pos].next = list->data[position].next;
    list->data[position].next = adding_pos;





    ASSERT_OK(list);

    return Ok;

}

error_code DeletePosition(ListInfo* list, size_t position) {

    ASSERT_OK(list);

    list->data[list->data[position].prev].next = list->data[position].next;
    list->data[list->data[position].next].prev = list->data[position].prev;


    list->data[position].next = 0;
    list->data[position].elem = 0;
    list->data[position].prev = list->next_place;
    list->next_place = position;

    ASSERT_OK(list);

    return Ok;

}


error_code ListDtor(ListInfo* list) {

    ASSERT_OK(list);

    free(list->data);

    return Ok;

}

#ifdef _DEBUG //for debug funcs

error_code ListErr(ListInfo* list) {

    assert(list);

    error_code code = Ok;



    list->errors_bit = code;

    return code;
}

void ListDump(ListInfo* list) {

    assert(list);

    BirthInfo* info_got = list->info;
    printf("=====INIT_INFO=====\nFILE: %s /-----/ FUCK: %s /-----/ LINE: %d /-----/ NAME: %s\n\n",
                                info_got->file, info_got->func, info_got->line, info_got->name);

    printf("ERROR_CODE: %d\n", list->errors_bit);
    printf("ListDump(%s[%p]) {\n", info_got->name, &list);

    printf("    capacity      = %d\t%s\n", list->capacity,      ContainsError(list->errors_bit, SizeError) ? "(BAD!)" : "");
    printf("    poison        = %d\n", POISON);

    printf("    data[%p]\t%s {\n", list->data,              ContainsError(list->errors_bit, NullptrDataError) ||
                                                       ContainsError(list->errors_bit, PoisonDataError)  ||
                                                       ContainsError(list->errors_bit, PoisonFillingError)         ? "(BAD!)" : "");
    if (!(ContainsError(list->errors_bit, NullptrDataError) || ContainsError(list->errors_bit, CapacityError))) {

        for (size_t index = 0; index < list->capacity; index++) {
            const char* is_poison = "";
            const char* is_filled = "*";
            if (index >= list->capacity) is_filled = " ";
            size_t       next  = (list->data)[index].next;
            list_type element  = (list->data)[index].elem;
            size_t       prev  = (list->data)[index].prev;
            if (element == POISON) is_poison = "(poison)";
             printf("        %s [%u] = %5u %5d %5u %s\n", is_filled, index, next, element, prev, is_poison);
        }

        printf("\n    }");
    }

    printf("\n}\n\n");
}

static void FillPoison(ListInfo* list) {

    return;

}

bool ContainsError(error_code code, List_Err_t err) {
    return err & code;
}


void FilingHTML(ListInfo* list, const char* html_filename) {
    const char* dot_filename = "temp_graph.dot";
    const char* svg_filename = "temp_graph.svg";

    FILE* dot_file = fopen(dot_filename, "w");
    if (!dot_file) {
        perror("Ошибка создания dot файла");
        return;
    }

    fprintf(dot_file, "digraph G {\n");
    fprintf(dot_file, "  rankdir=LR;\n");
    fprintf(dot_file, "  node [shape=rect, style=\"rounded,filled\", fillcolor=lightgray, fontsize=12];\n");

    for (size_t i = 0; i < list->capacity; i++) {
        ListElem e = list->data[i];
        fprintf(dot_file,
            "  node%zu [label=\"next: %d\\nelem: %d\\nprev: %d\"];\n",
            i, e.next, e.elem, e.prev);
    }

    for (size_t i = 0; i < list->capacity; i++) {
        ListElem e = list->data[i];
        if (e.next >= 0 && (size_t)e.next < list->capacity) {
            fprintf(dot_file, "  node%zu -> node%d [color=blue];\n", i, e.next);
        }
    }

    for (size_t i = 0; i < list->capacity; i++) {
        ListElem e = list->data[i];
        if (e.prev >= 0 && (size_t)e.prev < list->capacity) {
            fprintf(dot_file, "  node%zu -> node%d [color=red, style=dashed];\n", i, e.prev);
        }
    }

    fprintf(dot_file, "}\n");
    fclose(dot_file);

    char cmd[512];
    snprintf(cmd, sizeof(cmd), "dot -Tsvg %s -o %s", dot_filename, svg_filename);
    if (system(cmd) != 0) {
        fprintf(stderr, "Ошибка вызова dot\n");
        return;
    }

    FILE* svg_file = fopen(svg_filename, "r");
    if (!svg_file) {
        perror("Ошибка открытия svg файла");
        return;
    }

    FILE* html_file = fopen(html_filename, "w");
    if (!html_file) {
        perror("Ошибка создания html файла");
        fclose(svg_file);
        return;
    }

    fprintf(html_file, "<html><head><meta charset=\"UTF-8\"><title>Двусвязный список - граф</title></head><body>\n");
    fprintf(html_file, "<h2>Граф двусвязного списка</h2>\n");

    char buffer[4096];
    while (fgets(buffer, sizeof(buffer), svg_file)) {
        fputs(buffer, html_file);
    }

    fprintf(html_file, "</body></html>\n");

    fclose(svg_file);
    fclose(html_file);

    remove(dot_filename);
    remove(svg_filename);
}


#endif // for debug func
