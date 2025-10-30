#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "list.h"

#ifdef _DEBUG
static void FillPoison(ListInfo* list, size_t capacity_old);
#endif

static ListElem* AllocateList(ListInfo* list, size_t capacity_got, size_t capacity_old) {

    ListElem* realloc_ptr = (ListElem*)realloc(list->data, capacity_got * sizeof(ListElem));

    if (realloc_ptr == nullptr) return nullptr;

    list->capacity = capacity_got;
    list->data = realloc_ptr;


    #ifdef _DEBUG
    FillPoison(list, capacity_old);
    #endif

    for (size_t index = capacity_old; index < list->capacity; index++) {

        list->data[index].next = 0;
        list->data[index].prev = index+1;

    }


    return realloc_ptr;

}

error_code ListCtor(ListInfo* list, size_t capacity_got, BirthInfo* info_got = nullptr) {

    assert(list);

    if (capacity_got > MAX_SIZE_VALUE) return CapacityError;
    if (capacity_got < MIN_SIZE_VALUE) capacity_got = MIN_SIZE_VALUE;

    #ifdef _DEBUG
    list->info = info_got;
    #endif //debug

    ListElem* calloc_ptr = AllocateList(list, capacity_got, 0);
    if (calloc_ptr == nullptr) return AllocationError;

    ASSERT_OK(list);

    return Ok;

}

error_code AddValueAfterPosition(ListInfo* list, list_type value, size_t position) {

    ASSERT_OK(list);

    if (list->current_free_place == list->capacity - 1) {
        ListElem* realloc_ptr = AllocateList(list, list->capacity * LIST_EXPAND_VALUE, list->capacity);
        if (realloc_ptr == nullptr) return AllocationError;
    }

    size_t adding_pos = list->current_free_place;

    list->current_free_place = list->data[adding_pos].prev;

    list->data[adding_pos].prev = position;
    list->data[list->data[position].next].prev = adding_pos;


    list->data[adding_pos].elem = value;

    list->data[adding_pos].next = list->data[position].next;
    list->data[position].next = adding_pos;


    list->size++;


    ASSERT_OK(list);

    return Ok;

}

error_code AddValueBeforePosition(ListInfo* list, list_type value, size_t position) {

    ASSERT_OK(list);

    AddValueAfterPosition(list, value, list->data[position].prev);

    ASSERT_OK(list);

    return Ok;
}

error_code RemovePositionFromList(ListInfo* list, size_t position) {

    ASSERT_OK(list);

    list->data[list->data[position].prev].next = list->data[position].next;
    list->data[list->data[position].next].prev = list->data[position].prev;


    list->data[position].next = 0;
    list->data[position].elem = POISON;
    list->data[position].prev = list->current_free_place;
    list->current_free_place = position;

    list->size--;

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

    if (list->data == nullptr) {
        code |= NullptrDataError;
        list->errors_bit = code;
        return code;
    }

    if (list->size > list->capacity) {
        code |= SizeMoreThanCapacityError;
    }

    if (list->capacity > MAX_SIZE_VALUE) {
        code |= SizeError;
    }

    size_t current = list->data[0].next;
    while (current != 0) {

        if (current >= list->capacity || current != list->data[list->data[current].next].prev) {
            code |= NextPrevError;
            return code;
        }

        if (list->data[current].elem == POISON && current != 0) {
            list->errors_bit = code;
            code |= PoisonDataError;
        }

        current = list->data[current].next;

    }


    list->errors_bit = code;

    return code;
}

void ListDump(ListInfo* list, FILE* out, DumpingMode mode) {

    assert(list);

    if (mode == HTMLFileMode) fprintf(out, "<pre>\n");


    BirthInfo* info_got = list->info;
    fprintf(out, "=====INIT_INFO=====\nFILE: %s /-----/ FUCK: %s /-----/ LINE: %d /-----/ NAME: %s\n\n",
                                info_got->file, info_got->func, info_got->line, info_got->name);

    fprintf(out, "ERROR_CODE: %d\n", list->errors_bit);
    fprintf(out, "ListDump(%s[%p]) {\n", info_got->name, &list);

    fprintf(out, "    size          = %lu\t%s\n", list->size,      ContainsError(list->errors_bit, SizeError) ? "(BAD!)" : "");
    fprintf(out, "    capacity      = %lu\t%s\n", list->capacity,      ContainsError(list->errors_bit, SizeMoreThanCapacityError) ? "(BAD!)" : "");
    fprintf(out, "    poison        = %d\n", POISON);

    fprintf(out, "    data[%p]\t%s {\n", list->data,              ContainsError(list->errors_bit, NullptrDataError) ||
                                                       ContainsError(list->errors_bit, PoisonDataError)  ||
                                                       ContainsError(list->errors_bit, PoisonFillingError)         ? "(BAD!)" : "");
    if (!(ContainsError(list->errors_bit, NullptrDataError) || ContainsError(list->errors_bit, CapacityError))) {

        for (size_t index = 0; index < list->capacity; index++) {
            const char* is_used = "";
            const char* is_filled = "*";
            if (list->data[index].elem == POISON) is_filled = " ";
            size_t       next  = (list->data)[index].next;
            list_type element  = (list->data)[index].elem;
            size_t       prev  = (list->data)[index].prev;
            if (element == POISON) is_used = "(NOT_ELEMENT)";
                fprintf(out, "        %s [%3lu] = %15lu | %15d | %15lu | %s\n", is_filled, index, next, element, prev, is_used);
        }

        fprintf(out, "\n    }");
    }

    fprintf(out, "\n}\n\n");

    if (mode == HTMLFileMode) {

        static size_t number_of_images = 0;

        char command[CHAR_STRING_SIZE] = "";
        char img_file[CHAR_STRING_SIZE] = "";


        const char* temp_dot_code = "temp_dot_code.dot";
        MakeLogicalDotFromList(list, temp_dot_code);

        sprintf(img_file, "images/img%lu.svg", number_of_images);

        sprintf(command, "dot -Tsvg %s -o %s", temp_dot_code, img_file);
        system(command);

        fprintf(out, "<img src=\"%s\" style=\"max-width: 100%; height: auto;\" />\n", img_file);

        number_of_images++;

        MakeIndexedDotFromList(list, temp_dot_code);

        sprintf(img_file, "images/img%lu.svg", number_of_images);

        sprintf(command, "dot -Tsvg %s -o %s", temp_dot_code, img_file);
        system(command);

        fprintf(out, "<img src=\"%s\" style=\"max-width: 100%; height: auto;\" />\n", img_file);

        number_of_images++;

    }

}

static void FillPoison(ListInfo* list, size_t capacity_old) {

    for (size_t index = capacity_old; index < list->capacity; index++) {

        list->data[index].elem = POISON;

    }

}

bool ContainsError(error_code code, List_Err_t err) {
    return err & code;
}


void MakeLogicalDotFromList(ListInfo* list, const char* filename) {

    FILE* out = fopen(filename, "w");
    //check

    bool* element_included = (bool*)calloc(list->capacity, sizeof(bool));
    size_t* order = (size_t*)calloc(list->capacity, sizeof(size_t));
    //check

    order[0] = 0;
    element_included[0] = true;
    size_t order_len = 1;

    size_t current = list->data[0].next;

    while (current < list->capacity && !element_included[current]) {
        order[order_len++] = current;
        element_included[current] = true;
        current = list->data[current].next;
    }

    fprintf(out, "digraph G {\n");
    fprintf(out, "    orientation=portrait;\n");
    fprintf(out, "    node [shape=rect, style=\"rounded,filled\", fillcolor=lightgray, fontsize=12];\n");


    fprintf(out, "    {rank=same; ");
    for (size_t i = 0; i < order_len; i++) {
        fprintf(out, "node%lu; ", order[i]);
    }
    fprintf(out, "}\n");


    for (size_t i = 0; i < order_len; i++) {
        size_t idx = order[i];
        ListElem* e = &list->data[idx];
        const char* style = "";
        if (idx == list->data[0].next) {
            style = "penwidth=3, color=blue";
        } else if (idx == list->data[0].prev) {
            style = "penwidth=3, color=yellow";
        } else if (idx == 0) {
            style = "penwidth=3, color=black";
        }
        fprintf(out,
            "    node%lu [label=\"index: %lu\\nnext: %lu\\nelem: %d\\nprev: %lu\", %s];\n",
            idx, idx, e->next, e->elem, e->prev, style);
    }

    for (size_t i = 0; i < order_len; i++) {
        size_t from = order[i];
        ListElem* e = &list->data[from];
        if (e->next < list->capacity && element_included[e->next]) {
            fprintf(out, "    node%lu -> node%lu [color=blue];\n", from, e->next);
        }
    }


    for (size_t i = 0; i < order_len; i++) {
        size_t from = order[i];
        ListElem* e = &list->data[from];
        if (e->prev < list->capacity && element_included[e->prev]) {
            fprintf(out, "    node%lu -> node%lu [color=red, style=dashed];\n", from, e->prev);
        }
    }

    fprintf(out, "    null_elem [shape=ellipse fillcolor=\"#797979ff\" style=filled label=\"null_elem = %d\"];\n", 0);
    fprintf(out, "    null_elem -> node%d [color=brown];\n", 0);

    fprintf(out, "    head [shape=ellipse fillcolor=\"#3d77e3ff\" style=filled label=\"head = %lu\"];\n", list->data[0].next);
    fprintf(out, "    head -> node%lu [color=brown];\n", list->data[0].next);

    fprintf(out, "    tail [shape=ellipse fillcolor=\"#eded54ff\" style=filled label=\"tail = %lu\"];\n", list->data[0].prev);
    fprintf(out, "    tail -> node%lu [color=brown];\n", list->data[0].prev);

    fprintf(out, "}\n");

    fclose(out);
    free(element_included);
    free(order);
}


void MakeIndexedDotFromList(ListInfo* list, const char* filename) {

    FILE* out = fopen(filename, "w");

    size_t order_len = list->capacity;

    fprintf(out, "digraph G {\n");
    fprintf(out, "    orientation=portrait;\n");
    fprintf(out, "    node [shape=rect, style=\"rounded,filled\", fillcolor=lightgray, fontsize=12];\n");


    fprintf(out, "    {rank=same; ");
    for (size_t i = 0; i < order_len; i++) {
        fprintf(out, "node%lu; ", i);
    }
    fprintf(out, "}\n");

    for (size_t i = 0; i < order_len; i++) {
        size_t idx = i;
        ListElem* e = &list->data[idx];
        const char* style = "";
        if (idx == list->data[0].next) {
            style = "penwidth=3, color=blue";
        } else if (idx == list->data[0].prev) {
            style = "penwidth=3, color=yellow";
        } else if (idx == 0) {
            style = "penwidth=3, color=black";
        }
        fprintf(out,
            "    node%lu [label=\"index: %lu\\nnext: %lu\\nelem: %d\\nprev: %lu\", %s];\n",
            idx, idx, e->next, e->elem, e->prev, style);
    }


    for (size_t i = 0; i < order_len; i++) {
        size_t from = i;
        ListElem* e = &list->data[from];
        if (i < order_len - 1) fprintf(out, "    node%lu -> node%lu [style=invis, weight=100];\n", from, i + 1);
        if (e->next < list->capacity && (i == 0 || list->data[i].elem != POISON)) {
            fprintf(out, "    node%lu -> node%lu [color=blue, weight=1];\n", from, e->next);
        }
    }

    for (size_t i = 0; i < order_len; i++) {
        size_t from = i;
        ListElem* e = &list->data[from];
        if (e->prev < list->capacity && (i == 0 || list->data[i].elem != POISON)) {
            fprintf(out, "    node%lu -> node%lu [color=red, style=dashed, weight=0];\n", from, e->prev);
        }
    }

    size_t frees = list->current_free_place;
    while (frees < list->capacity && list->data[frees].prev < list->capacity) {
        fprintf(out, "    node%lu -> node%lu [color=lightgray, style=dashed, weight=0];\n", frees, list->data[frees].prev);
        frees = list->data[frees].prev;
    }

    fprintf(out, "    null_elem [shape=ellipse fillcolor=\"#797979ff\" style=filled label=\"null_elem = %d\"];\n", 0);
    fprintf(out, "    null_elem -> node%d [color=brown];\n", 0);

    fprintf(out, "    free [shape=ellipse fillcolor=\"#c22498ff\" style=filled label=\"free = %lu\"];\n", list->current_free_place);
    fprintf(out, "    free -> node%lu [color=brown];\n", list->current_free_place);

    fprintf(out, "    head [shape=ellipse fillcolor=\"#3d77e3ff\" style=filled label=\"head = %lu\"];\n", list->data[0].next);
    fprintf(out, "    head -> node%lu [color=brown];\n", list->data[0].next);

    fprintf(out, "    tail [shape=ellipse fillcolor=\"#eded54ff\" style=filled label=\"tail = %lu\"];\n", list->data[0].prev);
    fprintf(out, "    tail -> node%lu [color=brown];\n", list->data[0].prev);

    fprintf(out, "}\n");

    fclose(out);
}

#endif // for debug func
