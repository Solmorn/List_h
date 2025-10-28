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



    FILE* out_html = fopen("out_br.html", "w");

    if (out_html == nullptr) printf("ghghhg");

    printf("SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS\n");

    fprintf(out_html, "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");


    BirthInfo* info_got = list->info;
    fprintf(out_html, "=====INIT_INFO=====\nFILE: %s /-----/ FUCK: %s /-----/ LINE: %d /-----/ NAME: %s\n\n",
                                info_got->file, info_got->func, info_got->line, info_got->name);

    fprintf(out_html, "ERROR_CODE: %d\n", list->errors_bit);
    fprintf(out_html, "ListDump(%s[%p]) {\n", info_got->name, &list);

    fprintf(out_html, "    capacity      = %d\t%s\n", list->capacity,      ContainsError(list->errors_bit, SizeError) ? "(BAD!)" : "");
    fprintf(out_html, "    poison        = %d\n", POISON);

    fprintf(out_html, "    data[%p]\t%s {\n", list->data,              ContainsError(list->errors_bit, NullptrDataError) ||
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
                fprintf(out_html, "        %s [%u] = %5u %5d %5u %s\n", is_filled, index, next, element, prev, is_poison);
        }

        fprintf(out_html, "\n    }");
    }

    fprintf(out_html, "\n}\n\n");


    FilingHTML(list, "out.dot");


    system("dot -Tsvg out.dot -o img.svg");


    //fprintf(out_html, "<img src=\"img.svg\" style=\"max-width: 100%; height: auto;\" />");

    fclose(out_html);
}

static void FillPoison(ListInfo* list) {

    return;

}

bool ContainsError(error_code code, List_Err_t err) {
    return err & code;
}


void FilingHTML(ListInfo* list, const char* filename) {

    FILE* out = fopen(filename, "w");
    //check

    bool* element_included = (bool*)calloc(list->capacity, sizeof(bool));
    if (element_included == nullptr) printf("AAAAAAAAAAAAGHD");
    size_t* order = (size_t*)calloc(list->capacity, sizeof(size_t));
    //check

    order[0] = 0;

    element_included[0] = true;
    printf("SSSSSS44SSSSSSSSSSSSSSSSSSSSSSSSSS\n");
    size_t order_len = 1;

    size_t current = list->data[0].next;

    while (current != 0 && current < list->capacity && !element_included[current]) {
        order[order_len++] = current;
        element_included[current] = true;
        current = list->data[current].next;
    }


    fprintf(out, "digraph G {\n");
    fprintf(out, "  orientation=landscape;\n");
    fprintf(out, "  rotate=180;\n");
    fprintf(out, "  node [shape=rect, style=\"rounded,filled\", fillcolor=lightgray, fontsize=12];\n");


    fprintf(out, "  { rank=same; ");
    for (size_t i = 0; i < order_len; i++) {
        fprintf(out, "node%u; ", order[i]);
    }
    fprintf(out, "}\n");


    for (size_t i = 0; i < order_len; i++) {
        size_t idx = order[i];
        ListElem* e = &list->data[idx];
        const char* style = "";
        if (idx == list->data[0].next) {
            style = "penwidth=3, color=black";
        } else if (idx == list->data[0].prev) {
            style = "penwidth=3, color=yellow";
        }
        fprintf(out,
            "  node%u [label=\"next: %u\\nelem: %d\\nprev: %u\", %s];\n",
            idx, e->next, e->elem, e->prev, style);
    }


    for (size_t i = 0; i < order_len; i++) {
        size_t from = order[i];
        ListElem* e = &list->data[from];
        if (e->next != 0 && e->next < list->capacity && element_included[e->next]) {
            fprintf(out, "  node%u -> node%u [color=blue];\n", from, e->next);
        }
    }


    for (size_t i = 0; i < order_len; i++) {
        size_t from = order[i];
        ListElem* e = &list->data[from];
        if (e->prev != 0 && e->prev < list->capacity && element_included[e->prev]) {
            fprintf(out, "  node%u -> node%u [color=red, style=dashed];\n", from, e->prev);
        }
    }

    fprintf(out, "}\n");

    fclose(out);
    free(element_included);
    free(order);
}


#endif // for debug func
