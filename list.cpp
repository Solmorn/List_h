#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "list.h"

#ifdef _DEBUG
//
static void FillPoison(ListInfo* list);
#endif

static list_type* AllocateList(ListInfo* list, size_t capacity_got) {

    list_type* realloc_ptr = (list_type*)realloc(list->data, capacity_got * sizeof(list_type));

    if (realloc_ptr == nullptr) return nullptr;

    list->capacity = capacity_got;
    list->data = realloc_ptr;

    //#ifdef _DEBUG
    //FillPoison(list);
    //#endif

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

    list_type* calloc_ptr = AllocateList(list, capacity_got);
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
        list_type* realloc_ptr = AllocateList(list, list->capacity * LIST_EXPAND_VALUE);
        if (realloc_ptr == nullptr) return AllocationError;
    }

    size_t adding_pos = list->next_place;
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

    if (list->size == 0) return PopSizeStackError;

    *element = list->data[(list->size) - 1];
    list->data[--(list->size)] = POISON;

    #ifdef HASH_ON
    list->hash_value = CalculateDataHash(list);
    #endif

    ASSERT_OK(list);

    return Ok;

}


error_code ListDtor(ListInfo* list) {

    ASSERT_OK(list);
    printf("ffff1\n");
    printf("%p\n", (list->data_canary_p).data_canary1);
    #ifdef CANARY_ON
    free((list->data_canary_p).data_canary1);
    #else
    free(list->data);
    #endif //canary
    fprintf(stderr,"ffff2\n");
    return Ok;

}

#ifdef _DEBUG //for debug funcs

error_code ListErr(ListInfo* list) {

    assert(list);

    error_code code = 0;

    #ifdef CANARY_ON
    if (list->stack_canary1 != STACK_CANARY || list->stack_canary2 != STACK_CANARY) {
                                code |= StackCanaryError;
        list->errors_bit = code;
        return code;
    }
    #endif

    bool check_data_allowed   = true;
    bool check_poison_allowed = true;

    if (list->size     > MAX_SIZE_VALUE) {
                                code |= SizeError;
        check_data_allowed   = false;
    }

    if (list->capacity > MAX_SIZE_VALUE) {
                                code |= CapacityError;
        check_poison_allowed = false;
    }

    if (list->capacity < list->size) {
                                code |= SizeMoreThanCapacityError;
        check_poison_allowed = false;
    }

    if (list->data == nullptr) {
                                code |= NullptrDataError;
        list->errors_bit = code;
        return code;
    }

    for (size_t index = 0; check_data_allowed && index < list->size; index++) {
        if ((list->data)[index] == POISON) {
                                code |= PoisonDataError;
            break;
        }
    }

    for (size_t index = list->size; check_poison_allowed && index < list->capacity; index++) {
        if ((list->data)[index] != POISON) {
                                code |= PoisonFillingError;
            break;
        }
    }

    #ifdef CANARY_ON
    if (*((list->data_canary_p).data_canary1) != DATA_CANARY || *((list->data_canary_p).data_canary2) != DATA_CANARY) {
                                code |= DataCanaryError;
    }
    #endif

    #ifdef HASH_ON
    if (check_data_allowed && (list->hash_value != CalculateDataHash(list))) {
                                code |= HashError;
    }
    #endif

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

    printf("    size          = %lu\t%s\n", list->size,      ContainsError(list->errors_bit, SizeError) ||
                                                            ContainsError(list->errors_bit, SizeMoreThanCapacityError)  ? "(BAD!)" : "");
    printf("    poison        = %d\n", POISON);

    printf("    data[%p]\t%s", list->data,              ContainsError(list->errors_bit, NullptrDataError) ||
                                                       ContainsError(list->errors_bit, PoisonDataError)  ||
                                                       ContainsError(list->errors_bit, PoisonFillingError)         ? "(BAD!)" : "");
    if (!(ContainsError(list->errors_bit, NullptrDataError) || ContainsError(list->errors_bit, CapacityError))) {

        #ifdef CANARY_ON
        printf(" {\n         [canary] = %x\t%s\n", *((list->data_canary_p).data_canary1),
                                                       ContainsError(list->errors_bit, DataCanaryError)            ? "(BAD!)" : "");
        #endif

        for (size_t index = 0; index < list->capacity; index++) {
            const char* is_poison = "";
            const char* is_filled = "*";
            if (index >= list->size) is_filled = " ";
            list_type element = (list->data)[index];
            if (element == POISON) is_poison = "(poison)";
             printf("        %s [%lu] = %d %s\n", is_filled, index, element, is_poison);
        }
        #ifdef CANARY_ON
        printf("         [canary] = %x\t%s", *((list->data_canary_p).data_canary2),
                                                      ContainsError(list->errors_bit, DataCanaryError)             ? "(BAD!)" : "");
        #endif
        printf("\n    }");
    }

    #ifdef CANARY_ON
    printf("\n    stack_canary2 = %x\t%s\n", list->stack_canary2, ContainsError(list->errors_bit, StackCanaryError)     ? "(BAD!)" : "");
    #endif
    printf("\n}\n\n");
}

static void FillPoison(ListInfo* list) {

    for (size_t index = list->size; index < list->capacity; index++) {
        (list->data)[index] = POISON;
    }

}

bool ContainsError(error_code code, List_Err_t err) {
    return err & code;
}
#endif // for debug func
