---
title: Data Type Optimizations
date: 2023-07-01T11:39:13+0800
categories:
- Redis
tags: 
- Redis
- Data Type Optimizations
- Jedis
- Redis Essentials
---

In Redis, all data types can use different encodings to save memory or improve performance. For instance,
a String that has only digits(for example, 123456) uses less memory that a string of letters(for example,
abcde) because they use different encoding. Data types will use different encodings based on thresholds
defined in the Redis server configuration.

When Redis is downloaded, it comes with a fil called _redis.conf_. This file is well documented and has all
the Redis configuration directives, although some of them are commented out. Usually, the default values in
this file are sufficient for most applications. The Redis configurations can also be specified via the
command-line option or the **CONFIG** command; the most common approach is to use a configuration file.
## Data type optimizations

In Redis, all data types can use different encodings to save memory or improve performance. For instance,
a String that has only digits(for example, 123456) uses less memory that a string of letters(for example,
abcde) because they use different encoding. Data types will use different encodings based on thresholds
defined in the Redis server configuration.

When Redis is downloaded, it comes with a fil called _redis.conf_. This file is well documented and has all
the Redis configuration directives, although some of them are commented out. Usually, the default values in
this file are sufficient for most applications. The Redis configurations can also be specified via the
command-line option or the **CONFIG** command; the most common approach is to use a configuration file.
### String

The following are the available encoding for Strings:
* _int_: This is used when the string is represented by a 64-bit signed integer
* _embstr_: This is used for strings with fewer than 40 bytes
* _raw_: This is used for strings with more than 40 bytes

These encodings are not configurable.

```java
jedis.set("intKey", "123456");
jedis.set("embstrKey", "abcedf");
jedis.set("rawKey", "Neque porro quisquam est qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit...");
String intEnc = jedis.objectEncoding("intKey"); // int
String embstrEnc = jedis.objectEncoding("embstrKey"); // embstr
String rawEnc = jedis.objectEncoding("rawKey"); // raw
```
### List

These are the available encodings for Lists:
* **ziplist**: This is used when the List size has fewer elements than the configuration
   **list-max-ziplist-entries** and each List element has few bytes than the configuration
   **list-max-ziplist-value**
* **linkedlist**: This is used when the previous limits are exceeded.

```java
jedis.lpush("aList", "a", "b");
String ziplist = jedis.objectEncoding("aList"); // "quicklist"

jedis.lpush("aList", "Neque porro quisquam est qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit...");
String linkedlist = jedis.objectEncoding("aList"); // "quicklist"
```

> `quicklists` was introduced in [Redis 3.2](https://raw.githubusercontent.com/antirez/redis/3.2/00-RELEASENOTES):
> 
> > New encoding for the List type: Quicklists. Very important memory savings and storage space in RDB gains (up to 10x sometimes).
{: .prompt-info }

### Set

The following are available encodings for Sets:
* **intset**: This is used when all elements of a Set are integers and the Set cardinality is
  smaller than the configuration **set-max-intset-entries**
* **hashtable: This is used when any element of a Set is not an integer or the Set cardinality
  exceeds the configuration **set-max-intset-entries**


```java
jedis.sadd("aSet", "1", "2");
String intset = jedis.objectEncoding("aSet"); // "intset"

jedis.sadd("aSet", "a");
String hashtable = jedis.objectEncoding("aSet"); // "hashtable"
```
### Hash

The following are the available encodings for Hashes:

* **ziplist**: Used when the number of fields in the Hash does not exceed the configuration
   **hash-max-ziplist-entries** and each field name and value of the Hash is less than the configuration
   **hash-max-ziplist-value**(in bytes).
* **hashtable**: Used when a Hash size or any of its values exceed the configurations **hash-max-ziplist-entries** and **hash-max-ziplist-value** respectively.

```java
jedis.hset("aHash", Map.of("a", "1", "b", "2"));
String ziplist = jedis.objectEncoding("aHash"); // "listpack"

jedis.hset("aHash", Map.of("c", "Neque porro quisquam est qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit..."));
String hashtable = jedis.objectEncoding("aHash"); // "hashtable"
```

> More about `listpack` can be found [here](https://gist.github.com/antirez/66ffab20190ece8a7485bd9accfbc175).
{: .prompt-info }

### Sorted Set
The following are the available encodings:

* **ziplist**: Used when a Sorted Set has fewer entries than the configuration **set-max-ziplist-entries**
   and each ot its values are smaller than **zset-max-ziplist-value**(in bytes)
* **skiplist and hashtable**: These are used when the Sorted Set number of entries or size of any
  its values exceed the configurations **set-max-ziplist-entries** and **zset-max-ziplist-value**

```java
jedis.zadd("aSortedSet", 1, "a");
String ziplist = jedis.objectEncoding("aSortedSet"); // "listpack"

jedis.zadd("aSortedSet", 2, "Neque porro quisquam est qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit...");
String hashtable = jedis.objectEncoding("aSortedSet"); // "skiplist"
```


## Measuring Memory Usage

Let's try to measure the memory usage using Hash as an example.
Let's clear all keys on Redis and check memory information.

```java
jedis.flushAll();
String beforeDefault = jedis.info("memory"); // # Memory, used_memory:2284016, used_memory_human:2.18M, used_memory_rss:3694592, used_memory_rss_human:3.52M, used_memory_peak:2326464, used_memory_peak_human:2.22M, used_memory_peak_perc:98.18%, used_memory_overhead:1079328, used_memory_startup:1078560, used_memory_dataset:1204688, used_memory_dataset_perc:99.94%, allocator_allocated:2249104, allocator_active:3659776, allocator_resident:3659776, total_system_memory:17179869184, total_system_memory_human:16.00G, used_memory_lua:34816, used_memory_vm_eval:34816, used_memory_lua_human:34.00K, used_memory_scripts_eval:552, number_of_cached_scripts:3, number_of_functions:0, number_of_libraries:0, used_memory_vm_functions:32768, used_memory_vm_total:67584, used_memory_vm_total_human:66.00K, used_memory_functions:216, used_memory_scripts:768, used_memory_scripts_human:768B, maxmemory:0, maxmemory_human:0B, maxmemory_policy:noeviction, allocator_frag_ratio:1.63, allocator_frag_bytes:1410672, allocator_rss_ratio:1.00, allocator_rss_bytes:0, rss_overhead_ratio:1.01, rss_overhead_bytes:34816, mem_fragmentation_ratio:1.64, mem_fragmentation_bytes:1445488, mem_not_counted_for_evict:0, mem_replication_backlog:0, mem_total_replication_buffers:0, mem_clients_slaves:0, mem_clients_normal:0, mem_cluster_links:0, mem_aof_buffer:0, mem_allocator:libc, active_defrag_running:0, lazyfree_pending_objects:0, lazyfreed_objects:0

for (int i = 0; i < 500; i++) {
    jedis.hset("hashKey", "field-name-" + i, "field-value-" + i);
}

String afterDefault = jedis.info("memory"); // # Memory, used_memory:2326672, used_memory_human:2.22M, used_memory_rss:3694592, used_memory_rss_human:3.52M, used_memory_peak:2326672, used_memory_peak_human:2.22M, used_memory_peak_perc:100.00%, used_memory_overhead:1113432, used_memory_startup:1078560, used_memory_dataset:1213240, used_memory_dataset_perc:97.21%, allocator_allocated:2283776, allocator_active:3659776, allocator_resident:3659776, total_system_memory:17179869184, total_system_memory_human:16.00G, used_memory_lua:34816, used_memory_vm_eval:34816, used_memory_lua_human:34.00K, used_memory_scripts_eval:552, number_of_cached_scripts:3, number_of_functions:0, number_of_libraries:0, used_memory_vm_functions:32768, used_memory_vm_total:67584, used_memory_vm_total_human:66.00K, used_memory_functions:216, used_memory_scripts:768, used_memory_scripts_human:768B, maxmemory:0, maxmemory_human:0B, maxmemory_policy:noeviction, allocator_frag_ratio:1.60, allocator_frag_bytes:1376000, allocator_rss_ratio:1.00, allocator_rss_bytes:0, rss_overhead_ratio:1.01, rss_overhead_bytes:34816, mem_fragmentation_ratio:1.62, mem_fragmentation_bytes:1410816, mem_not_counted_for_evict:0, mem_replication_backlog:0, mem_total_replication_buffers:0, mem_clients_slaves:0, mem_clients_normal:34032, mem_cluster_links:0, mem_aof_buffer:0, mem_allocator:libc, active_defrag_running:0, lazyfree_pending_objects:0, lazyfreed_objects:0
String encodingDefault = jedis.objectEncoding("hashKey"); // "hashtable"
```
The total memory used(`used_memory`) was approximately **16kB**.

Let's update **hash-max-ziplist-entries** configuration to a smaller number and insert the same dataset.
```java
jedis.flushAll();
jedis.configSet("hash-max-ziplist-entries", "3");

String before = jedis.info("memory");  // # Memory, used_memory:2284208, used_memory_human:2.18M, used_memory_rss:3694592, used_memory_rss_human:3.52M, used_memory_peak:2326672, used_memory_peak_human:2.22M, used_memory_peak_perc:98.17%, used_memory_overhead:1113360, used_memory_startup:1078560, used_memory_dataset:1170848, used_memory_dataset_perc:97.11%, allocator_allocated:2283776, allocator_active:3659776, allocator_resident:3659776, total_system_memory:17179869184, total_system_memory_human:16.00G, used_memory_lua:34816, used_memory_vm_eval:34816, used_memory_lua_human:34.00K, used_memory_scripts_eval:552, number_of_cached_scripts:3, number_of_functions:0, number_of_libraries:0, used_memory_vm_functions:32768, used_memory_vm_total:67584, used_memory_vm_total_human:66.00K, used_memory_functions:216, used_memory_scripts:768, used_memory_scripts_human:768B, maxmemory:0, maxmemory_human:0B, maxmemory_policy:noeviction, allocator_frag_ratio:1.60, allocator_frag_bytes:1376000, allocator_rss_ratio:1.00, allocator_rss_bytes:0, rss_overhead_ratio:1.01, rss_overhead_bytes:34816, mem_fragmentation_ratio:1.62, mem_fragmentation_bytes:1410816, mem_not_counted_for_evict:0, mem_replication_backlog:0, mem_total_replication_buffers:0, mem_clients_slaves:0, mem_clients_normal:34032, mem_cluster_links:0, mem_aof_buffer:0, mem_allocator:libc, active_defrag_running:0, lazyfree_pending_objects:0, lazyfreed_objects:0

for (int i = 0; i < 500; i++) {
    jedis.hset("hashKey", "field-name-" + i, "field-value-" + i);
}

String after = jedis.info("memory");  // # Memory, used_memory:2326864, used_memory_human:2.22M, used_memory_rss:3694592, used_memory_rss_human:3.52M, used_memory_peak:2326864, used_memory_peak_human:2.22M, used_memory_peak_perc:100.00%, used_memory_overhead:1113432, used_memory_startup:1078560, used_memory_dataset:1213432, used_memory_dataset_perc:97.21%, allocator_allocated:2283776, allocator_active:3659776, allocator_resident:3659776, total_system_memory:17179869184, total_system_memory_human:16.00G, used_memory_lua:34816, used_memory_vm_eval:34816, used_memory_lua_human:34.00K, used_memory_scripts_eval:552, number_of_cached_scripts:3, number_of_functions:0, number_of_libraries:0, used_memory_vm_functions:32768, used_memory_vm_total:67584, used_memory_vm_total_human:66.00K, used_memory_functions:216, used_memory_scripts:768, used_memory_scripts_human:768B, maxmemory:0, maxmemory_human:0B, maxmemory_policy:noeviction, allocator_frag_ratio:1.60, allocator_frag_bytes:1376000, allocator_rss_ratio:1.00, allocator_rss_bytes:0, rss_overhead_ratio:1.01, rss_overhead_bytes:34816, mem_fragmentation_ratio:1.62, mem_fragmentation_bytes:1410816, mem_not_counted_for_evict:0, mem_replication_backlog:0, mem_total_replication_buffers:0, mem_clients_slaves:0, mem_clients_normal:34032, mem_cluster_links:0, mem_aof_buffer:0, mem_allocator:libc, active_defrag_running:0, lazyfree_pending_objects:0, lazyfreed_objects:0
String encoding = jedis.objectEncoding("hashKey"); // "hashtable"
```
The total memory used(`used_memory`) was approximately **42kB**. Almost 3 times more than the default one.

Forcing a Hash to be a ziplist has a trade-off -  the more elements a Hash has, the slower the performance.
A ziplist is a dually linked list designed to be memory-efficient, and lookups are performed in Linear
time(O(n), where n is the number of the field in a Hash). On the other hand, a hashtable's lookup runs in
constant time(O(1)), no matter how many elements exist.


