# Mini Key Value storage (Mini Redis)

Hash table, djb2 algorithm.

Collision handling: separate chaining (linked list in buckets)

Rehash when load factor > 0.75

# Architecture

Client 
|
v
TCP socket (port )
|
V
Command parser (SET/GET/DEL)
|
V
Hash Table (in memory)

# How to compile
gcc hash.c -lws2_32 -o redis
gcc client.c -lws2_32 -o client
# How to run
./a.exe (./a.out)

# Test

# Features

SET
GET
DEL
KEYS
EXIST
HELP
QUIT

# Advanced
[x] TCP server                              Client server talks
[x] Add persistence (save/load to file)     File I/O, serialization, pager
[x] Implement RESP protocol		            Binary-safe parsing
[x] Use epoll/select instead of fork		        Event loop, non-blocking I/O
[x] Add data types (list, set, hash)	    Advanced data structures
[] Add expiration (TTL)			        Time-based key eviction, LRU Cache