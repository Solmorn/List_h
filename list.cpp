#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "list.h"

#ifdef _DEBUG
//
static void FillPoison(ListInfo* list);
#endif

static ListElem* AllocateList(ListInfo* list, size_t capacity_got) {

    ListElem* realloc_ptr = (ListElem*)realloc(list->data, capacity_got * sizeof(list_type));

    if (realloc_ptr == nullptr) return nullptr;

    list->capacity = capacity_got;
    list->data = realloc_ptr;

    for (size_t index = 0; index < list->capacity; index++) {
        list->data[index].elem = 0;
    }

    #ifdef _DEBUG
    FillPoison(list);
    #endif

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

    //for (size_t index = 1; index < list->capacity; index++) {
    //    list->data[index].next = POISON;
    //    list->data[index].prev = POISON;
    //}

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

    list->data[adding_pos].elem = value;

    list->data[adding_pos].next = list->data[position].next;
    list->data[position].next = adding_pos;

    list->next_place = list->data[adding_pos].prev;

    list->data[adding_pos].prev = position;
    list->data[list->data[position].next].prev = adding_pos;


    ASSERT_OK(list);

    return Ok;

}

error_code DeletePosition(ListInfo* list, size_t position) {

    ASSERT_OK(list);



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
            list_type element = (list->data)[index].elem;
            if (element == POISON) is_poison = "(poison)";
             printf("        %s [%u] = %d %s\n", is_filled, index, element, is_poison);
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
#endif // for debug func
