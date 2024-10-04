#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef int stack_elem_t;
const size_t INIT_STACK_SIZE = 2;
const int L_CANARY = 0xDEADF00D;
const int R_CANARY = 0xBADC0FFE;
const stack_elem_t TOXIC = -11111;
const size_t CANARY_MEMORY = 2;
const size_t CHANGE_CAPACITY_COEFFICIENT = 2;

void stack_ctor(struct Stack_t* stk);
void work_with_stack(struct Stack_t* stk, stack_elem_t* pop_element);
void up_cap(struct Stack_t* stk);
void stack_push(struct Stack_t* stk, stack_elem_t push); 
void stack_pop(struct Stack_t* stk, stack_elem_t* pop);  
void stack_dtor(struct Stack_t* stk); 
void dump(struct Stack_t* stk, FILE* in);
void low_cap(struct Stack_t* stk);
unsigned long calculation_hash(const stack_elem_t* data, size_t size);
void verification(struct Stack_t* stk);

struct Stack_t
{
    int l_stack_canary;
    size_t size;
    size_t capacity;
    stack_elem_t* data;
    unsigned long hash;
    FILE* input_log;
    int r_stack_canary;
};

int main()
{
    Stack_t stk  = {};
    stack_ctor(&stk);
    stack_elem_t pop_element = TOXIC;
    work_with_stack(&stk, &pop_element);
    stack_dtor(&stk);
    return 0;
}
void work_with_stack(struct Stack_t* stk, stack_elem_t* pop_element)
{
    printf("0 - finish the programm\n 1 - print stack\n 2 - push the element\n 3 - pop the element\n 4 - check info\n");
    int programm_options = -1;
    scanf("%d", &programm_options);
    while (programm_options != 0)
    {
        verification(stk);
        switch (programm_options)
        {
        case 1:
            printf("currernt stack\n");
            for (size_t i = 0; i < stk->size; i++)
            {
                printf("%d ", stk->data[i]);
            }
            printf("\n");
            break;
        case 2:
            stack_elem_t push;
            printf("Write number to add to the stack\n");
            scanf("%d", &push);
            stack_push(stk, push); 
            break;  
        case 3:
            stack_pop(stk, pop_element);
            printf("pop element = %d\n", *pop_element);
            break;
        case 4:
            dump(stk, stdout);
            verification(stk);
            break;
        default:
        printf("wrong options");
            break;
        }
        printf("Choose programm options\n");
        scanf("%d", &programm_options);
    }
}

void stack_ctor(struct Stack_t* stk)
{
    stk->size = 0;
    stk->capacity = INIT_STACK_SIZE;
    stk->l_stack_canary = L_CANARY;
    stk->r_stack_canary = R_CANARY;
    stk->data = (stack_elem_t*) calloc(stk->capacity + CANARY_MEMORY, sizeof(stack_elem_t));
    stk->data++;
    //TODO:Check return value 
    stk->data[-1] = L_CANARY;
    stk->data[stk->capacity] = R_CANARY;
    for (size_t i = 0; i < stk->capacity; i++)
    {
        stk->data[i] = TOXIC;
    }
    stk->hash = calculation_hash(stk->data, stk->size);
    verification(stk);
}
void up_cap(struct Stack_t* stk)
{
    stk->capacity *= CHANGE_CAPACITY_COEFFICIENT;
    stk->data--;
    stk->data = (stack_elem_t*) realloc(stk->data, stk->capacity + CANARY_MEMORY); 
    stk->data++;
    stk->data[stk->capacity] = R_CANARY;
    for (size_t i = stk->size; i < stk->capacity; i++) 
    {
        stk->data[i] = TOXIC;
    }
    dump(stk, stdout);
    verification(stk);

}

void stack_push(struct Stack_t* stk, stack_elem_t push)
{
    if (stk->capacity <= stk->size)
    {
        up_cap(stk);
    }
    stk->size++;
    stk->data[stk->size - 1] = push;
    printf("sha pzdc    1\n");
    dump(stk, stdout);
    stk->hash = calculation_hash(stk->data, stk->size);
    printf("sha pzdc      2\n");
    dump(stk, stdout);
    verification(stk);
}

void stack_pop(struct Stack_t* stk, stack_elem_t* pop)
{
    verification(stk);
    if (stk->capacity > CHANGE_CAPACITY_COEFFICIENT * stk->size + CHANGE_CAPACITY_COEFFICIENT)
    { 
        low_cap(stk);
    }
    *pop = stk->data[stk->size - 1]; 
    stk->data[stk->size - 1] = TOXIC; 
    stk->size--;
    stk->hash = calculation_hash(stk->data, stk->size);
    verification(stk);
}

void low_cap(struct Stack_t* stk)
{
    stk->capacity /= CHANGE_CAPACITY_COEFFICIENT;
    stk->data = (stack_elem_t*) realloc(stk->data - 1, stk->capacity);
    stk->data++; 
    //check return
    stk->data[stk->capacity] = R_CANARY;
    verification(stk);    
}

void dump(struct Stack_t* stk, FILE* in)
{
    fprintf(in, "pointer data = %p\n", stk->data);
    fprintf(in, "size stack  = %d\n", stk->size); 
    fprintf(in, "capacity stack  = %d\n", stk->capacity);
    fprintf(in, "hash = %lu\n",calculation_hash(stk->data, stk->size));
    for (size_t i = 0; i <= stk->capacity + 1; i++)
    {
        stk->data[i - 1] == TOXIC ? fprintf(in, "data[%d] = %d (TOXIC) || pointer data[%d] = %p\n", i - 1, stk->data[i - 1], i - 1 , &stk->data[i - 1]) : fprintf(in, "data[%d] = %d || pointer data[%d] = %p\n", i - 1, stk->data[i - 1], i - 1, &stk->data[i - 1]);
    }
}

void stack_dtor(struct Stack_t* stk)
{
    free(stk->data);
    
    //TODO: kill canary capacity size alll
}

void verification(struct Stack_t* stk)
{
    size_t error_number = 0;
    stk->input_log = fopen("log_stack.txt", "a");
    if (stk->l_stack_canary != L_CANARY) {
        error_number = 1;
        fprintf(stk->input_log, "Error: stk->l_stack_canary != L_CANARY\n");
    }
    if(stk->r_stack_canary != R_CANARY) {
        error_number = 1;
        fprintf(stk->input_log, "Error: stk->r_stack_canary != R_CANARY\n");
    }
    if (stk->data[-1] != L_CANARY) {
        error_number = 1;
        fprintf(stk->input_log, "Error: stk->l_data_canary != L_CANARY\n");
    }
    if(stk->data[stk->capacity] != R_CANARY) {
        error_number = 1;
        fprintf(stk->input_log, "Error: stk->r_data_canary != R_CANARY %d\n", __LINE__);
    }
    if(stk->data == NULL) {
        error_number = 1;
        fprintf(stk->input_log, "Error: stk->data == 0\n");
    }
    if(stk->size > stk->capacity) {
        error_number = 1;
        fprintf(stk->input_log, "Error: stk->size > stk->capacity\n");
    }
    if(calculation_hash(stk->data, stk->size) != stk->hash) {
        error_number = 1;
        fprintf(stk->input_log, "Error: hash changed\n");
        
    }

    if (error_number == 1)
    {
        printf("ERROR, check log_stack.txt\n");
        dump(stk, stk->input_log);
        fprintf(stk->input_log,"%s %s\n\n\n",__TIME__, __DATE__);
        fclose(stk->input_log);
        exit(0);
    }
    fclose(stk->input_log);
}

unsigned long calculation_hash(const stack_elem_t* data, size_t size)
{
    unsigned long hash = 5381;
    for (size_t i = 0; i < size ; i++)
    {
        hash = ((hash << 5) + hash) + data[i];
    }
    return hash;
}





