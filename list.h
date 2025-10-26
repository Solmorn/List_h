#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>


#ifndef LIST_H
#define LIST_H

typedef int list_type;
typedef int error_code;

static const size_t     MAX_SIZE_VALUE     = 0x00011A6AAD;
static const size_t     MIN_SIZE_VALUE     =           10;
static const list_type  POISON             = 0x00000D1127;
static const size_t     LIST_EXPAND_VALUE  =            2;


enum List_Err_t {
    Ok                        =      0,//
    AllocationError           = 1 << 1,
    CapacityError             = 1 << 2,
    NullptrDataError          = 1 << 4,
    SizeError                 = 1 << 5,//
    PoisonDataError           = 1 << 8,
    PoisonFillingError        = 1 << 9,//
};

#ifdef _DEBUG

struct BirthInfo {
    const char* file;
    const char* func;
    const char* name;
    int         line;
};


#define INIT_LIST(list, capacity) do {                          \
    BirthInfo info_got = {__FILE__, __func__, #list, __LINE__};  \
    ListCtor(&list, capacity, &info_got);                       \
} while(0);

#define ASSERT_OK(list) do{              \
                                        \
    error_code code = 0;                \
                                        \
    if ((code = ListErr(list)) != 0) {    \
        ListDump(list);                \
        printf("ASSERTION FAILED\n\n"); \
        return code;                    \
    }                                   \
                                        \
}while(0);

#else //debug

#define INIT_LIST(list, capacity) do {                          \
    ListCtor(&list, capacity);                                  \
} while(0);

#define ASSERT_OK(list) do{              \
}while(0);

#endif //debug

struct ListElem {

    list_type elem = 0;

    size_t    prev = 0;
    size_t    next = 0;

};

struct ListInfo {

    ListElem*   data           = nullptr;

    list_type   head           =       0;
    list_type   tail           =       0;

    list_type   next_place     =       1;

    size_t      capacity       =       0;

    #ifdef _DEBUG
    BirthInfo* info            = nullptr;
    error_code errors_bit      =       0;
    #endif // _DEBUG

};

#ifdef _DEBUG
error_code ListErr(ListInfo* list);
void ListDump(ListInfo* list);
void FilingHTML(ListInfo* list, const char* filename);
bool ContainsError(error_code code, List_Err_t err);
#endif





error_code ListCtor(ListInfo* list, size_t capacity, BirthInfo* info_got);
//error_code SetHead(ListInfo* list,);
error_code AddValueAfterPosition(ListInfo* list, list_type value, size_t position);
error_code DeletePosition(ListInfo* list, size_t position);
error_code ListDtor(ListInfo* list);

#endif
