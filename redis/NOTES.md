## Data structures

Entry: single key-value pair
- char *key
- char *value
- Entry *next (for collision chaining)

HashTable: the main structure
- Entry **buckets (array of pointers)
- int size (number of buckets)

## Collision strategy

Using separate chaining with linked lists.

If two keys hash to the same bucket, they form a linked list.


## Visualization

HashTable:
- size: 100
- buckets:

[0] --> Entry(key="name", value="Linh", next = NULL)
[1] --> NULL
[2] --> Entry(key="city", value="Castelfranco", next = --> Entry(...))
[3] --> NULL
...
[99] --> NULL

## Functions completed:
- [x] hash()
- [ ] create_table()
- [ ] destroy_table()
- [ ] set()
- [ ] get()
- [ ] delete()
- [ ] rehash()
