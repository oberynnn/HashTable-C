#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#define PRIME_1 151
#define PRIME_2 163
#define INITIAL_BASE_SIZE 50

typedef struct {
    char* key;
    char* value;
} item;

static item DELETED_ITEM = {NULL, NULL};

typedef struct {
    int size;
    int count;
    int base_size;
    item** items;
} hash_table;

static item* create_new_item(const char* key, const char* value);
static hash_table* create_new_table(void);
static void delete_item(item* i);
static void delete_table(hash_table* table);
static int hash(const char* string, const int a, const int n);
static int double_hashing(const char* string, const int num, const int attempt);
void insert(hash_table* table, const char* key, const char* value);
char* search(hash_table* table, const char* key);
void delete(hash_table* table, const char* key);
int is_prime(const int x);
int next_prime(int x);
static hash_table* new_size(const int base_size);
hash_table* new_sized(void);
static void resize_table(hash_table* table, const int base_size);
static void table_resize_up(hash_table* table);
static void table_resize_down(hash_table* table);

static item* create_new_item(const char* key, const char* value) {
    item* new;

    new = malloc(sizeof(item));
    new->key = strdup(key);
    new->value = strdup(value);

    return new;
}

static hash_table* create_new_table(void) {
    hash_table* new;

    new = malloc(sizeof(hash_table));
    new->size = 10;
    new->count = 0;
    new->base_size = 10;
    new->items = calloc((size_t)new->size, sizeof(item*));
    return new;
}

static void delete_item(item* i) {
    free(i->key);
    free(i->value);
    free(i);
}

static void delete_table(hash_table* table) {
    item* table_item;
    int i;

    for (i = 0; i < table->size; i++) {
        table_item = table->items[i];
        if (table_item != NULL) {
            delete_item(table_item);
        }
    }
    free(table->items);
    free(table);
}

static int hash(const char* string, const int a, const int n) {
    long hash_result;
    int length;
    int i;

    hash_result = 0;
    length = strlen(string);
    for (i = 0; i < length; i++) {
        hash_result += (long)pow(a, length - (i + 1)) * string[i];
        hash_result %= n;
    }
    hash_result = (int)hash_result;
    return hash_result;
}

static int double_hashing(const char* string, const int num, const int attempt) {
    int a;
    int b;
    int result;

    a = hash(string, PRIME_1, num);
    b = hash(string, PRIME_2, num);
    result = (a + (attempt * (b + 1))) % num;

    return result;
}

void insert(hash_table* table, const char* key, const char* value) {
    item* hash_item;
    item* curr_item;
    int idx;
    int i;
    int load;

    load = table->count * 100 / table->size;
    if (load > 70) {
        table_resize_up(table);
    }
    hash_item = create_new_item(key, value);
    idx = hash(hash_item->key, table->size, 0);
    curr_item = table->items[idx];
    i = 1;
    while (curr_item != NULL) {
        if (curr_item != &DELETED_ITEM) {
            if (strcmp(key, curr_item->key) == 0) {
                delete_item(curr_item);
                table->items[idx] = hash_item;
                return;
            }
        }
        idx = hash(hash_item->key, table->size, i);
        curr_item = table->items[idx];
        i++;
    }
    table->items[idx] = hash_item;
    table->count++;
}

char* search(hash_table* table, const char* key) {
    int idx;
    item* hash_item;
    int i;

    idx = hash(key, table->size, 0);
    hash_item = table->items[idx];
    i = 1;
    while (hash_item != NULL) {
        if (hash_item != &DELETED_ITEM) {
            if (strcmp(key, hash_item->key) == 0) {
            return hash_item->value;
            }
            idx = hash(key, table->size, i);
            hash_item = table->items[idx];
            i++;
        }
    }
    return NULL;
}

void delete(hash_table* table, const char* key) {
    int idx;
    item* hash_item;
    int i;
    int load;

    load = table->count * 100 / table->size;
    if (load < 10) {
        table_resize_down(table);
    }
    idx = hash(key, table->size, 0);
    hash_item = table->items[idx];
    i = 1;
    while (hash_item != NULL) {
        if (hash_item != &DELETED_ITEM) {
            if (strcmp(key, hash_item->key) == 0) {
                delete_item(hash_item);
                table->items[idx] = &DELETED_ITEM;
            }
        }
        idx = hash(key, table->size, i);
        hash_item = table->items[idx];
        i++;
    }
    table->count--;
}

int is_prime(const int x) {
    int i;

    if (x < 2) {
        return -1;
    }
    else if (x < 4) {
        return 1;
    }
    else if ((x % 2) == 0) {
        return 0;
    }
    for (i = 3; i <= floor(sqrt((double)x)); i += 2) {
        if ((x % i) == 0) {
            return 0;
        }
    }
    return 1;
}

int next_prime(int x) {
    while (is_prime(x) != 1) {
        x++;
    }
    return x;
}

static hash_table* new_size(const int base_size) {
    hash_table* table;

    table = malloc(sizeof(table));
    table->size = next_prime(base_size);
    table->base_size = base_size;
    table->count = 0;
    table->items = calloc(table->size, sizeof(item*));

    return table;
}

hash_table* new_sized(void) {
    return new_size(INITIAL_BASE_SIZE);
}

static void resize_table(hash_table* table, const int base_size) {
    hash_table* new;
    int i;
    int temp_size;
    item* new_item;
    item** temp_items;

    if (base_size < INITIAL_BASE_SIZE) {
        return;
    }
    new = new_size(base_size);
    for (i = 0; i < new->size; i++) {
        new_item = new->items[i];
        if (new_item != NULL && new_item != &DELETED_ITEM) {
            insert(new, new_item->key, new_item->value);
        }
    }
    table->base_size = new->base_size;
    table->count = new->count;
    temp_size = table->size;
    table->size = new->size;
    new->size = temp_size;
    temp_items = table->items;
    table->items = new->items;
    new->items = temp_items;
    delete_table(new);
}

static void table_resize_up(hash_table* table) {
    int new_size;

    new_size = table->base_size * 2;
    resize_table(table, new_size);
}

static void table_resize_down(hash_table* table) {
    int new_size;

    new_size = table->base_size / 2;
    resize_table(table, new_size);
}