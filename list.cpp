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

        list->data[index].next = index+1;
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

    ListElem* calloc_ptr = AllocateList(list, capacity_got, 1);
    if (calloc_ptr == nullptr) return AllocationError;

    list->data[0].next = 0;
    list->data[0].prev = 0;
    list->data[0].elem = POISON;

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


    list->data[position].elem = POISON;
    list->data[position].prev = list->current_free_place;
    list->data[position].next = list->current_free_place;
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
        code |= CapacityError;
    }

    size_t current_next = list->data[0].next;
    size_t current_prev = list->data[0].prev;
    size_t size_check = 0;
    while (current_next != 0 && current_prev != 0) {
        //next way check
        if (current_next >= list->capacity ||
            list->data[current_next].next >= list->capacity ||
            current_next != list->data[list->data[current_next].next].prev) {

            code |= NextPrevError;
            list->errors_bit = code;
            return code;
        }

        if (list->data[current_next].elem == POISON && current_next != 0) {
            list->errors_bit = code;
            code |= PoisonDataError;
        }

        current_next = list->data[current_next].next;

        //prev way check
        if (current_prev >= list->capacity ||
            list->data[current_prev].prev >= list->capacity ||
            current_prev != list->data[list->data[current_prev].prev].next) {

            code |= NextPrevError;
            list->errors_bit = code;
            return code;
        }

        if (list->data[current_prev].elem == POISON && current_prev != 0) {
            list->errors_bit = code;
            code |= PoisonDataError;
        }

        current_prev = list->data[current_prev].prev;

        size_check++;

    }

    if (size_check != list->size) {
        code |= SizeError;
    }


    list->errors_bit = code;

    return code;
}

void ListDump(ListInfo* list, FILE* out, DumpingMode mode) {

    assert(list);
    ListErr(list);

    if (mode == HTMLFileMode) fprintf(out, "<pre>\n");


    BirthInfo* info_got = list->info;
    fprintf(out, "=====INIT_INFO=====\nFILE: %s /-----/ FUCK: %s /-----/ LINE: %d /-----/ NAME: %s\n\n",
                                info_got->file, info_got->func, info_got->line, info_got->name);

    fprintf(out, "ERROR_CODE: %d\n", list->errors_bit);
    fprintf(out, "ListDump(%s[%p]) {\n", info_got->name, &list);

    fprintf(out, "    size          = %lu\t%s\n", list->size,      ContainsError(list->errors_bit, SizeError) || ContainsError(list->errors_bit, SizeMoreThanCapacityError) ? "(BAD!)" : "");
    fprintf(out, "    capacity      = %lu\t%s\n", list->capacity,      ContainsError(list->errors_bit, CapacityError) ? "(BAD!)" : "");
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


enum Arrow {
    BadArrow,
    ProblemArrow,
    NoArrow,
    GoodArrow
};

enum Color {
    NeutralElementColor,
    GoodElementColor,
    BadElementColor
};



void MakeIndexedDotFromList(ListInfo* list, const char* filename) {

    FILE* out = fopen(filename, "w");

    size_t order_len = list->capacity;
    Arrow arrows[MAX_SIZE_VALUE][MAX_SIZE_VALUE] = {};
    Color colors[MAX_SIZE_VALUE] = {};

    for (size_t i = 0; i < order_len; i++) {

        for (size_t j = 0; j < order_len; j++) {
            arrows[i][j] = NoArrow;
        }

        colors[i] = NeutralElementColor;
    }


    for (size_t i = 0; i < order_len; i++) {
        ListElem* e = &(list->data[i]);
        if (i == 0 || e->elem != POISON) {
            if (e->next >= list->size) {
            } else if (list->data[e->next].prev == i) {
                if (colors[i] != BadElementColor) colors[i] = GoodElementColor;
                if (arrows[i][e->next] != BadArrow) {
                    if (i == 0) printf("SSSSSSSSS0000000000000SSSS\n");
                    arrows[i][e->next] = GoodArrow;
                }
                printf("%d__%d___%d\n\n", i, e->next, arrows[i][e->prev]);
            } else {
                if (arrows[i][e->next] != BadArrow) {
                    arrows[i][e->next] = ProblemArrow;
                }
                arrows[e->next][list->data[e->next].prev] = BadArrow;
                colors[list->data[e->next].prev] = BadElementColor;
            }
        }
    }


    for (size_t i = 0; i < order_len; i++) {
        ListElem* e = &(list->data[i]);
        if (i == 0 || e->elem != POISON) {
            if (e->prev >= list->size) {
            } else if (list->data[e->prev].next == i) {
                if (colors[i] != BadElementColor) colors[i] = GoodElementColor;
                if (arrows[i][e->prev] != BadArrow) {
                    if (i == 0) printf("SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS\n");
                    arrows[i][e->prev] = GoodArrow;
                }
                printf("%d__%d___%d\n\n", i, e->prev, arrows[i][e->prev]);
            } else {
                if (arrows[i][e->prev] != BadArrow) {
                    arrows[i][e->prev] = ProblemArrow;
                }
                arrows[e->prev][list->data[e->prev].next] = BadArrow;
                colors[list->data[e->prev].next] = BadElementColor;
            }
        }
    }


    //for (size_t i = 0; i < order_len; i++) {
    //    for (size_t j = 0; j < order_len; j++) {
    //        printf("%3lu__%3lu__%d\n", i, j, arrows[i][j]);
    //    }
    //}


    fprintf(out, "digraph G {\n");
    fprintf(out, "    orientation=portrait;\n");
    fprintf(out, "    node [shape=septagon, style=\"filled, penwidth=3\", fillcolor=red, fontsize=12];\n");


    fprintf(out, "    {rank=same; ");
    for (size_t i = 0; i < order_len; i++) {
        fprintf(out, "node%lu; ", i);
    }
    fprintf(out, "}\n");

    for (size_t i = 0; i < order_len; i++) {
        size_t idx = i;
        ListElem* e = &list->data[idx];
        const char* style = "";
        const char* fill_color = "";
        if (colors[i] == NeutralElementColor) fill_color = "lightgray";
        if (colors[i] == GoodElementColor) fill_color = "\"#c4e9c1ff\"";
        if (colors[i] == BadElementColor) fill_color = "red";

        if (idx == list->data[0].next) {
            style = "penwidth=3, color=blue";
        } else if (idx == list->data[0].prev) {
            style = "penwidth=3, color=yellow";
        } else if (idx == 0) {
            style = "penwidth=3, color=black";
        } else if (idx == list->current_free_place) {
            style = "penwidth=3, color=gray";
        }
        fprintf(out,
            "    node%lu [shape=rect, style=\"rounded,filled\", fillcolor=%s, label=\"index: %lu\\nnext: %lu\\nelem: %d\\nprev: %lu\", %s];\n",
            idx, fill_color, idx, e->next, e->elem, e->prev, style);
    }


    for (size_t i = 0; i < order_len; i++) {
        size_t from = i;
        if (i < order_len - 1) fprintf(out, "    node%lu -> node%lu [style=invis, weight=100];\n", from, i + 1);
    }

    for (size_t i = 0; i < order_len; i++) {
        size_t j = list->data[i].next;
            //fprintf(stderr, "=========================%5lu__%5lu\n", i, j);
            if (arrows[i][j] != NoArrow) {
                if (arrows[i][j] == GoodArrow) {
                    if (arrows[i][j] == arrows[j][i]) {
                        printf("%d====%d\n\n", i, j);
                        fprintf(out, "    node%lu -> node%lu [color=green, weight=0, dir = both];\n", i, j);
                    }
                } else if (arrows[i][j] == ProblemArrow) {
                    fprintf(out, "    node%lu -> node%lu [color=red, weight=0, penwidth=3];\n", i, j);
                } else if (arrows[i][j] == BadArrow){
                    fprintf(out, "    node%lu -> node%lu [color=black, weight=0, penwidth=5];\n", i, j);
                }
                arrows[i][j] = NoArrow;
            }

    }

    for (size_t i = 0; i < order_len; i++) {
        size_t j = list->data[i].prev;
            //fprintf(stderr, "+++++++++++++++++++++++++%5lu__%5lu\n", i, j);
            if (arrows[i][j] != NoArrow) {
                if (arrows[i][j] == GoodArrow) {
                    if (arrows[i][j] == arrows[j][i]) {
                        printf("%d====%d\n\n", i, j);
                        fprintf(out, "    node%lu -> node%lu [color=green, weight=0, dir = both];\n", i, j);
                    }
                } else if (arrows[i][j] == ProblemArrow) {
                    fprintf(out, "    node%lu -> node%lu [color=blue, weight=0, penwidth=3];\n", i, j);
                } else if (arrows[i][j] == BadArrow) {
                    fprintf(out, "    node%lu -> node%lu [color=black, weight=0, penwidth=5];\n", i, j);
                }
                arrows[i][j] = NoArrow;
            }

    }


    size_t frees = list->current_free_place;
    while (frees < list->capacity && list->data[frees].prev < list->capacity) {
        if (colors[frees] == NeutralElementColor && colors[list->data[frees].prev] == NeutralElementColor) {
            fprintf(out, "    node%lu -> node%lu [color=lightgray, style=dashed, weight=0];\n", frees, list->data[frees].prev);
        }
        frees = list->data[frees].prev;
    }

    fprintf(out, "    null_elem [shape=ellipse fillcolor=lightgray style=filled label=\"null_elem = %d\"];\n", 0);
    fprintf(out, "    null_elem -> node%d [color=brown];\n", 0);

    fprintf(out, "    free [shape=ellipse fillcolor=\"lightgray\" style=filled label=\"free = %lu\"];\n", list->current_free_place);
    fprintf(out, "    free -> node%lu [color=brown];\n", list->current_free_place);

    fprintf(out, "    head [shape=ellipse fillcolor=\"#a4bae4ff\" style=filled label=\"head = %lu\"];\n", list->data[0].next);
    fprintf(out, "    head -> node%lu [color=brown];\n", list->data[0].next);

    fprintf(out, "    tail [shape=ellipse fillcolor=\"#eded54ff\" style=filled label=\"tail = %lu\"];\n", list->data[0].prev);
    fprintf(out, "    tail -> node%lu [color=brown];\n", list->data[0].prev);

    fprintf(out, "}\n");

    fclose(out);
}

#endif // for debug func
