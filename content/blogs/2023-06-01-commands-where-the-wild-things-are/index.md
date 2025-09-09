---
draft: true
title: Commands(Where the Wild Things Are)
date: 2023-06-01T07:28:44+0800
categories:
- Redis
tags: 
- Redis
- Time Series
- Jedis
- Redis Essentials
---

This chapter gives an overview of many different Redis commands and features,
from techniques to reduce network latency to extending Redis with Lua scripting.
## Pub/Sub

Pub/Sub stands for Publish-Subscribe, which is a pattern where message are not sent directly to specific receivers.
Publisher send messages to channels, and subscribers receive these messages if they are listening to a given channel.

Here are some examples of Pub/Sub applications:
* News and weather dashboards
* Chat applications
* Push notifications, such as subway delay alerts
* Remove code execution
The following example implement a remove command execution system, where a command is sent to a channel and the server
that is subscribed to that channel executes that command.

The command **PUBLISH** sends a message to the Redis channel, and it returns the number of clients that received that message.
**A message gets lost if there are no clients subscribed to the channel when it comes in.

The command **SUBSCRIBE** subscribes a client to one or many channels. The command **UNSUBSCRIBE** unsubscribes a client from on
or many channels. The command **PSUBSCRIBE** or **PUNSUBSCRIBE** work the same way, but they accept glob-style patterns as channel names.


> Once a Redis client executes the command **SUBSCRIBE** OR **PSUBSCRIBE**, it enters the subscribe mode and stops
> accepting commands, except for the commands **SUBSCRIBE**, **PSUBSCRIBE**, **UNSUBSCRIBE**, and **PUNSUBSCRIBE**.
{: .prompt-tip }

Let's try with an example.
First, we define 2 functions on the server side, `sum` and `mul`:

```java
public int sum(int a, int b) {
    return a + b;
}

public int mul(int a, int b) {
    return a * b;
}
```
Implement subscribe logic on server end.

```java
jedis.subscribe(new JedisPubSub() {
    @Override
    public void onSubscribe(String channel, int subscribedChannels) {
        logger.info("onSubscribe({}, {})", channel, subscribedChannels);
    }

    @Override
    public void onUnsubscribe(String channel, int subscribedChannels) {
        logger.info("onUnsubscribe({}, {})", channel, subscribedChannels);
    }

    @Override
    public void onMessage(String channel, String message) {
        String[] parts = message.split(" ");
        String command = parts[0];
        switch (command) {
            case "sum": {
                String op1 = parts[1];
                String op2 = parts[2];
                int a = Integer.parseInt(op1);
                int b = Integer.parseInt(op2);
                int sum = sum(a, b);
                logger.info("Execute command 'sum' with {} and {}, result is {}", a, b, sum);
                break;
            }
            case "mul": {
                String op1 = parts[1];
                String op2 = parts[2];
                int a = Integer.parseInt(op1);
                int b = Integer.parseInt(op2);
                int mul = mul(a, b);
                logger.info("Execute command 'mul' with {} and {}, result is {}", a, b, mul);
                break;
            }
            case "exit": {
                logger.info("Done! Exiting....");
                unsubscribe();
                break;
            }
            default: {
                logger.info("Unknown command: {}", command);
                break;
            }
        }
    }
}, "channel");
```
Once the subscribe is up and running, we can publish message to the channel.

```java
long subCmdResult = jedis.publish("channel", "sum 2 3"); // 1
long mulCmdResult = jedis.publish("channel", "mul 44 6"); // 1
long exitCmdResult = jedis.publish("channel", "exit"); // 1
List<String> channels = jedis.pubsubChannels(); // [ "channel" ]
```
We could see below log on the server.

```
onSubscribe(channel, 1)
Execute command 'sum' with 2 and 3, result is 5
Execute command 'mul' with 44 and 6, result is 264
Done! Exiting....
onUnsubscribe(channel, 0)
```


## Transactions

A transaction in Redis is a sequence of commands executed in **order and atomically**.
The command **MULTI** marks the beginning of a transaction, and the command **EXEC** marks its end.
**Redis does not serve any other client in the middle of a transaction.**

All commands in a transaction are queued in the client and are only sent to the server when the **EXEC** command is executed.
It is possible to prevent a transaction from being executed by using the **DISCARD** command instead of **EXEC**.
Usually, Redis clients prevent a transaction from being sent to Redis if it contains command syntax errors.

Unlike in traditional SQL databases, transactions in Redis **are not rolled back** if they produce failure.
Redis executes commands in order, and if any of them fail, it proceeds to the next command. Another downside of Redis
transaction is that **it is not possible to make any decisions inside the transaction**, since all commands are queued.
For example, the following code simulates a bank transaction.
Here, money is transferred fro a source account to a destination account inside a Redis transaction.

If the source account has enough funds, the transaction is executed. Otherwise, it is discarded.

Create function `transfer`:

```java
public List<Object> transfer(Jedis client, String from, String to, long value) {
    long balance = Long.parseLong(client.get(from));
    Transaction multi = client.multi();
    multi.decrBy(from, value);
    multi.incrBy(to, value);

    if (balance >= value) {
        return multi.exec();
    }

    multi.discard();
    return null;
}

```
Top up accounts:

```java
jedis.mset("max: checkings", "100", "hugo:checkings", "100");
List<String> balances = jedis.mget("max: checkings", "hugo:checkings"); // [ "100", "100" ]
```
and make the transfer:

```java
List<Long> balance = transfer(jedis, "max: checkings", "hugo:checkings", 40); // [ "60", "140" ]
```

It is possible to make the execution of a transaction conditional using the **WATCH** command,
which implements a **optimistic lock** on a group of keys. The **WATCH** command marks keys as being
watched so that **EXEC** executes the transaction only if the keys are being watched were not changed.
Otherwise, it returns a null reply and the operation needs to be repeated; this is the reason it is called
an optimistic lock. The **UNWATCH** removes keys from the watch list.
Let's first define a `zpop` function:

```java
/**
 * Get and remove the first item in the sorted set
 */
public String zpop(Jedis client, String key, Function<String, Void> callback) {
    String watch = client.watch(key);
    List<String> zrange = client.zrange(key, 0, 0);
    Transaction multi = client.multi();
    multi.zrem(key, zrange.toArray(new String[]{}));
    List<Object> exec = multi.exec();
    if (exec != null) {
        callback.apply(zrange.get(0));
    } else {
        zpop(client, key, callback);
    }
    return watch;
}
```
And run the script:

```java
jedis.zadd("presidents", 1732, "George Washington");
jedis.zadd("presidents", 1809, "Abraham Lincoln");
jedis.zadd("presidents", 1858, "Theodore Roosevelt");

String watch = zpop(jedis, "presidents", (value) -> {
    logger.info("The first present in the group is: {}", value);
    return null;
}); // OK
```
And you would see the log printed:

```
The first present in the group is: George Washington
```


## Pipeline

In Redis, a pipeline is a ways to send multiple commands together to the Redis server without waiting for individual replies.
The replies are read all at once by the client.

The time taken for Redis client to send a command and obtain a reply from the Redis server is called **Round Trip Time(RTT)**.
When multiple commands are sent, there are multiple RTTs. Pipelines can decrease the number of RTTs because commands are grouped,
so a pipeline with 10 commands will have only one RTT. This can **improve the network's performance significantly**.


Redis commands sent in a pipeline **must be independent**. They run sequentially in the server(**the order is reserved**),
but they **do not run as a transaction**. Even though pipelines are neither transactional nor atomic(this means that different
Redis commands may occur between the ones in the pipeline), they are still useful because they can save a lot of network time, preventing the
network from becoming a bottleneck as it often does with heavy load applications.

Try this script.

```java
Pipeline pipeline = jedis.pipelined();

List<Response<String>> responses = new ArrayList<>();
for (int i = 0; i < 100; i++) {
    String key = getKey(String.valueOf(i));
    responses.add(pipeline.set(key, String.valueOf(i)));
}
pipeline.sync();
List<String> results = responses.stream().map(Response::get).toList(); // [ "OK", "OK", "OK", "OK", "OK", "OK", "OK", "OK", "OK", "OK" ]
```

> When sending many commands, it might be a good idea to use multiple pipelines rather than one big pipeline.
{: .prompt-tip }

Commands inside a transaction **may not** be sent as a pipeline by default. This will depend on the Redis client you are using.
It is a good idea to send transactions in a pipeline to avoid an extra round trip.


## Scripting

Redis 2.6 introduced the scripting feature, and the language that was chosen to extend Redis was Lua. Before
Redis 2.6, there was only one way to extend Redis - changing its source code, which was written in C. Lua was
chosen because it is very small and simple, and its C API is very easy to integrate with other libraries.

Lua scripts ara **atomically executed**, which means that the Redis server is blocked during scrip execution.
Because of this, Redis has a default timout of 5 seconds to run any script, although this value can be changed
through the configuration **lua-time-limit**.
### Redis meets Lua

A Redis client must send Lua script as strings to Redis server.
Redis can evaluate any valid Lua code, and a few libraries are available(for example, bitop, cjson, math, and string).
There are also two functions that execute Redis commands: `redis.call` and `redis.pcall`

The function `redis.call` requires the command name and all its parameters, and it returns the result of executed command.
If there are errors, `redis.call` aborts the scripts.
The function `redis.pcall` is similar to `redis.call`, but in the event of an error, it returns the error as a Lua table and continues
the script executions.

Every script can return a value through the keyword `return`, and if there is explicit return, the value `nil` is returned.

It is possible to pass Redis key names and parameters to a Lua script, and they will be available inside the Lua script
through the variables `KEYS` and `ARGS`, respectively.
There are two commands for running Lua scripts: **EVAL** and **EVALSHA**. The following code uses Lua to run the command **GET**
and retrieve a key value.
```java
jedis.set("myKey", "myValue");

var luaScript = "return redis.call('GET', KEYS[1])";
Object result = jedis.eval(luaScript, 1, "myKey"); // "myValue"
```
It is possible to save network bandwidth usage by using the command **SCRIPT LOAD** and **EVALSHA**
when executing the same script multiple times. The command **SCRIPT LOAD** caches a Lua script and returns
an identifier(which is the SHA1 hash of the script). The command **EVALSHA** executes a Lua script based on
an identifier. With **EVALSHA**, only a small identifier is transferred over the network, rather than the full Lua code snippet.

```java
jedis.set("myKey", "myValue");

var luaScript = """
    redis.call('GET', KEYS[1])
    return "Lua script using EVALSHA"
    """;
String scriptSHA = jedis.scriptLoad(luaScript);
Object result = jedis.evalsha(scriptSHA, 1, "myKey"); // "Lua script using EVALSHA"
 ```

> It order to make scripts play nicely with Redis replication, you should write scripts that do not change
> Redis keys in non-deterministic ways(that is, do not use random values). Well-written scripts behaves the
> same way when the are re-executed with the same data.
{: .prompt-info }


### Transaction with Lua

In previous _Transaction_ section, we presented an implementation of a `zpop` function using **WATCH** / **MULTI** / **EXEC**.
That implementation was based on an optimistic lock, which means that the entire operation had to be retried if a client changed
the Sorted Set before the  **MULTI** / **EXEC** was executed.

The same `zpop` function be implemented as a Lua script, and it will be simpler and atomic, which means retries
will not be necessary.

```java
public Object zpop(Jedis client, String key) {
    var luaScript = """
        local elements = redis.call('ZRANGE', KEYS[1], 0, 0)
        redis.call('ZREM', KEYS[1], elements[1])
        return elements[1]
        """;
    return client.eval(luaScript, 1, key);
}
```
and run the new `zpop` function.

```java
jedis.zadd("presidents", 1732, "George Washington");
jedis.zadd("presidents", 1809, "Abraham Lincoln");
jedis.zadd("presidents", 1858, "Theodore Roosevelt");

Object result = zpop(jedis, "presidents"); // "George Washington"
 ```


## Miscellaneous Commands

This section covers the most important Redis commands that we have not previously explained.
### INFO

The **INFO** command returns all Redis server statistic, including information about the Redis version,
operating system, connected clients, memory usage, persistence, replication, and keyspace. By default, the **INFO**
command shows all available sections: memory, persistence, CPU, command, cluster, clients, and replication.
You can also restrict the output by specifying the section name as a parameter.

```java
String info = jedis.info(); // [ "# Server", "redis_version:7.0.11", "redis_git_sha1:00000000", "redis_git_dirty:0", "redis_build_id:337e79498f5fefea", "redis_mode:standalone", "os:Darwin 22.5.0 x86_64", "arch_bits:64", "monotonic_clock:POSIX clock_gettime", "multiplexing_api:kqueue", "atomicvar_api:c11-builtin", "gcc_version:4.2.1", "process_id:18627", "process_supervised:no", "run_id:8ce60e3fae4f779f07e45e05307b037bfc0220b4", "tcp_port:6379", "server_time_usec:1688182949716401", "uptime_in_seconds:149149", "uptime_in_days:1", "hz:10", "configured_hz:10", "lru_clock:10461349", "executable:/Users/yan/redis-server", "config_file:", "io_threads_active:0", "", "# Clients", "connected_clients:1", "cluster_connections:0", "maxclients:10000", "client_recent_max_input_buffer:16896", "client_recent_max_output_buffer:0", "blocked_clients:0", "tracking_clients:0", "clients_in_timeout_table:0", "", "# Memory", "used_memory:2259104", "used_memory_human:2.15M", "used_memory_rss:2891776", "used_memory_rss_human:2.76M", "used_memory_peak:2295312", "used_memory_peak_human:2.19M", "used_memory_peak_perc:98.42%", "used_memory_overhead:1079328", "used_memory_startup:1078560", "used_memory_dataset:1179776", "used_memory_dataset_perc:99.93%", "allocator_allocated:2241760", "allocator_active:2857984", "allocator_resident:2857984", "total_system_memory:17179869184", "total_system_memory_human:16.00G", "used_memory_lua:34816", "used_memory_vm_eval:34816", "used_memory_lua_human:34.00K", "used_memory_scripts_eval:552", "number_of_cached_scripts:3", "number_of_functions:0", "number_of_libraries:0", "used_memory_vm_functions:32768", "used_memory_vm_total:67584", "used_memory_vm_total_human:66.00K", "used_memory_functions:216", "used_memory_scripts:768", "used_memory_scripts_human:768B", "maxmemory:0", "maxmemory_human:0B", "maxmemory_policy:noeviction", "allocator_frag_ratio:1.27", "allocator_frag_bytes:616224", "allocator_rss_ratio:1.00", "allocator_rss_bytes:0", "rss_overhead_ratio:1.01", "rss_overhead_bytes:33792", "mem_fragmentation_ratio:1.29", "mem_fragmentation_bytes:650016", "mem_not_counted_for_evict:0", "mem_replication_backlog:0", "mem_total_replication_buffers:0", "mem_clients_slaves:0", "mem_clients_normal:0", "mem_cluster_links:0", "mem_aof_buffer:0", "mem_allocator:libc", "active_defrag_running:0", "lazyfree_pending_objects:0", "lazyfreed_objects:0", "", "# Persistence", "loading:0", "async_loading:0", "current_cow_peak:0", "current_cow_size:0", "current_cow_size_age:0", "current_fork_perc:0.00", "current_save_keys_processed:0", "current_save_keys_total:0", "rdb_changes_since_last_save:62", "rdb_bgsave_in_progress:0", "rdb_last_save_time:1688182944", "rdb_last_bgsave_status:ok", "rdb_last_bgsave_time_sec:0", "rdb_current_bgsave_time_sec:-1", "rdb_saves:4", "rdb_last_cow_size:0", "rdb_last_load_keys_expired:0", "rdb_last_load_keys_loaded:0", "aof_enabled:0", "aof_rewrite_in_progress:0", "aof_rewrite_scheduled:0", "aof_last_rewrite_time_sec:-1", "aof_current_rewrite_time_sec:-1", "aof_last_bgrewrite_status:ok", "aof_rewrites:0", "aof_rewrites_consecutive_failures:0", "aof_last_write_status:ok", "aof_last_cow_size:0", "module_fork_in_progress:0", "module_fork_last_cow_size:0", "", "# Stats", "total_connections_received:1327", "total_commands_processed:4015", "instantaneous_ops_per_sec:0", "total_net_input_bytes:286358", "total_net_output_bytes:1383121", "total_net_repl_input_bytes:0", "total_net_repl_output_bytes:0", "instantaneous_input_kbps:0.08", "instantaneous_output_kbps:0.09", "instantaneous_input_repl_kbps:0.00", "instantaneous_output_repl_kbps:0.00", "rejected_connections:0", "sync_full:0", "sync_partial_ok:0", "sync_partial_err:0", "expired_keys:1", "expired_stale_perc:0.00", "expired_time_cap_reached_count:0", "expire_cycle_cpu_milliseconds:402", "evicted_keys:0", "evicted_clients:0", "total_eviction_exceeded_time:0", "current_eviction_exceeded_time:0", "keyspace_hits:436", "keyspace_misses:46", "pubsub_channels:0", "pubsub_patterns:0", "pubsubshard_channels:0", "latest_fork_usec:2629", "total_forks:4", "migrate_cached_sockets:0", "slave_expires_tracked_keys:0", "active_defrag_hits:0", "active_defrag_misses:0", "active_defrag_key_hits:0", "active_defrag_key_misses:0", "total_active_defrag_time:0", "current_active_defrag_time:0", "tracking_total_keys:0", "tracking_total_items:0", "tracking_total_prefixes:0", "unexpected_error_replies:0", "total_error_replies:2616", "dump_payload_sanitizations:0", "total_reads_processed:6010", "total_writes_processed:4814", "io_threaded_reads_processed:0", "io_threaded_writes_processed:0", "reply_buffer_shrinks:245", "reply_buffer_expands:174", "", "# Replication", "role:master", "connected_slaves:0", "master_failover_state:no-failover", "master_replid:012cc66a59cfa565b4ec92c20017c00e465908f6", "master_replid2:0000000000000000000000000000000000000000", "master_repl_offset:0", "second_repl_offset:-1", "repl_backlog_active:0", "repl_backlog_size:1048576", "repl_backlog_first_byte_offset:0", "repl_backlog_histlen:0", "", "# CPU", "used_cpu_sys:13.962421", "used_cpu_user:8.832239", "used_cpu_sys_children:0.008685", "used_cpu_user_children:0.001927", "", "# Modules", "", "# Errorstats", "errorstat_ERR:count=2616", "", "# Cluster", "cluster_enabled:0", "", "# Keyspace" ]
String memoryInfo = jedis.info("memory"); // [ "# Memory", "used_memory:2257248", "used_memory_human:2.15M", "used_memory_rss:2891776", "used_memory_rss_human:2.76M", "used_memory_peak:2295312", "used_memory_peak_human:2.19M", "used_memory_peak_perc:98.34%", "used_memory_overhead:1079328", "used_memory_startup:1078560", "used_memory_dataset:1177920", "used_memory_dataset_perc:99.93%", "allocator_allocated:2241760", "allocator_active:2857984", "allocator_resident:2857984", "total_system_memory:17179869184", "total_system_memory_human:16.00G", "used_memory_lua:34816", "used_memory_vm_eval:34816", "used_memory_lua_human:34.00K", "used_memory_scripts_eval:552", "number_of_cached_scripts:3", "number_of_functions:0", "number_of_libraries:0", "used_memory_vm_functions:32768", "used_memory_vm_total:67584", "used_memory_vm_total_human:66.00K", "used_memory_functions:216", "used_memory_scripts:768", "used_memory_scripts_human:768B", "maxmemory:0", "maxmemory_human:0B", "maxmemory_policy:noeviction", "allocator_frag_ratio:1.27", "allocator_frag_bytes:616224", "allocator_rss_ratio:1.00", "allocator_rss_bytes:0", "rss_overhead_ratio:1.01", "rss_overhead_bytes:33792", "mem_fragmentation_ratio:1.29", "mem_fragmentation_bytes:650016", "mem_not_counted_for_evict:0", "mem_replication_backlog:0", "mem_total_replication_buffers:0", "mem_clients_slaves:0", "mem_clients_normal:0", "mem_cluster_links:0", "mem_aof_buffer:0", "mem_allocator:libc", "active_defrag_running:0", "lazyfree_pending_objects:0", "lazyfreed_objects:0" ]
String persistenceInfo = jedis.info("persistence"); // [ "# Persistence", "loading:0", "async_loading:0", "current_cow_peak:0", "current_cow_size:0", "current_cow_size_age:0", "current_fork_perc:0.00", "current_save_keys_processed:0", "current_save_keys_total:0", "rdb_changes_since_last_save:62", "rdb_bgsave_in_progress:0", "rdb_last_save_time:1688182944", "rdb_last_bgsave_status:ok", "rdb_last_bgsave_time_sec:0", "rdb_current_bgsave_time_sec:-1", "rdb_saves:4", "rdb_last_cow_size:0", "rdb_last_load_keys_expired:0", "rdb_last_load_keys_loaded:0", "aof_enabled:0", "aof_rewrite_in_progress:0", "aof_rewrite_scheduled:0", "aof_last_rewrite_time_sec:-1", "aof_current_rewrite_time_sec:-1", "aof_last_bgrewrite_status:ok", "aof_rewrites:0", "aof_rewrites_consecutive_failures:0", "aof_last_write_status:ok", "aof_last_cow_size:0", "module_fork_in_progress:0", "module_fork_last_cow_size:0" ]
String cpuInfo = jedis.info("cpu"); // [ "# CPU", "used_cpu_sys:13.962461", "used_cpu_user:8.832280", "used_cpu_sys_children:0.008685", "used_cpu_user_children:0.001927" ]
String clusterInfo = jedis.info("cluster"); // [ "# Cluster", "cluster_enabled:0" ]
String clientsInfo = jedis.info("clients"); // [ "# Clients", "connected_clients:1", "cluster_connections:0", "maxclients:10000", "client_recent_max_input_buffer:16896", "client_recent_max_output_buffer:0", "blocked_clients:0", "tracking_clients:0", "clients_in_timeout_table:0" ]
String replicationInfo = jedis.info("replication"); // [ "# Replication", "role:master", "connected_slaves:0", "master_failover_state:no-failover", "master_replid:012cc66a59cfa565b4ec92c20017c00e465908f6", "master_replid2:0000000000000000000000000000000000000000", "master_repl_offset:0", "second_repl_offset:-1", "repl_backlog_active:0", "repl_backlog_size:1048576", "repl_backlog_first_byte_offset:0", "repl_backlog_histlen:0" ]
String keyspaceInfo = jedis.info("keyspace"); // [ "# Keyspace" ]
```

### DBSIZE

The **DBSIZE** command returns the number of existing keys in a Redis server.

```java
 long dbSize = jedis.dbSize(); // 0
```

### DEBUG SEGFAULT

The **DEBUG SEGFAULT** command crashes the Redis server process by performing an invalid memory process.
It can be quite interesting to simulate bugs during the development of your application.


### MONITOR

The command **MONITOR** shows all the commands processed by the Redis server in real time.
It can be quite interesting to simulate bugs during the development of your application.

Let's first start monitor:

```java
jedis.monitor(new JedisMonitor() {
    @Override
    public void onCommand(String command) {
        logger.info("Received command: {}", command);
    }
});
```

Then we perform some actions on Redis:
```java
jedis.zadd("fruit-ninja-score", 70, "Hugo");
jedis.zadd("fruit-ninja-score", 200, "Max");
jedis.zadd("fruit-ninja-score", 30, "Arthur");
```

After that, we would see be logs on monitoring side:
```
Received command: 1686231126.791042 [0 127.0.0.1:52021] "ZADD" "fruit-ninja-score" "70.0" "Hugo"
Received command: 1686231126.791240 [0 127.0.0.1:52021] "ZADD" "fruit-ninja-score" "200.0" "Max"
Received command: 1686231126.791409 [0 127.0.0.1:52021] "ZADD" "fruit-ninja-score" "30.0" "Arthur"
```

> While the **MONITOR** command is very helpful for debugging, it has a cost.
> In the Redis documentation page for **MONITOR**, an unscientific benchmark test says that **MONITOR**
> could **reduce the Redis's throughput by over 50%**.
{: .prompt-warning }


### CLIENT commands

The **CLIENT LIST** command returns a list of all clients connected to the server, as well as
relevant information and statistics about the client.

The **CLIENT SETNAME** command changes a client name, **it is only useful for debugging purposes**.

The **CLIENT KILL**  command terminates a client connection. It is possible to terminate client
connections by IP, port, ID, or type.

```java
String clientList = jedis.clientList(); // id=1335 addr=127.0.0.1:64406 laddr=127.0.0.1:6379 fd=8 name= age=3 idle=3 flags=N db=0 sub=0 psub=0 ssub=0 multi=-1 qbuf=0 qbuf-free=12 argv-mem=0 multi-mem=0 rbs=1024 rbp=0 obl=0 oll=0 omem=0 tot-mem=1792 events=r cmd=NULL user=default redir=-1 resp=2 id=1338 addr=127.0.0.1:64409 laddr=127.0.0.1:6379 fd=9 name= age=0 idle=0 flags=N db=0 sub=0 psub=0 ssub=0 multi=-1 qbuf=26 qbuf-free=16864 argv-mem=10 multi-mem=0 rbs=16384 rbp=16384 obl=0 oll=0 omem=0 tot-mem=34058 events=r cmd=client|list user=default redir=-1 resp=2 
jedis.clientSetname("My-New-Name");
String name = jedis.clientGetname(); // My-New-Name
long clientKill = jedis.clientKill(new ClientKillParams().type(ClientType.SLAVE)); // 0
```

### FLUSHALL
The **FLUSHALL** command deletes all keys from Redis - **this cannot be undone**.

```java
jedis.set("key1", "value1");
jedis.set("key2", "value2");
jedis.set("key3", "value3");
Set<String> allKeys = jedis.keys("*"); // [ "key1", "key2", "key3" ]
String flushAll = jedis.flushAll(); // "OK"
allKeys = jedis.keys("*"); // [  ]
```

### RANDOMKEY
The command **RANDOMKEY** returns a random existing key name. This may help you get an overview of
the available keys in Redis. The alternative would be to run the **KEYS** command, but it analyzes
all the existing keys in Redis. If the keyspace is large, it may block the Redis server entirely
during its execution.

```java
jedis.set("key1", "value1");
jedis.set("key2", "value2");
jedis.set("key3", "value3");
Set<String> keys = jedis.keys("*"); // [ "key1", "key2", "key3" ]
String randomKey = jedis.randomKey(); // "key2"
```

### EXPIRE and EXPIREAT, TTL and PTTL

The command **EXPIRE** sets a timeout in seconds for a given key. The key will be deleted after
the specific amount of second. A negative timeout will delete the key instantaneously(just like
running the command **DEL**)

The command **EXPIREAT sets a timeout for a given key based on a Unix timestamp. A timestamp of
the past will delete the key instantaneously.

These commands return 1 if the key timeout is set successfully or 0 if key does not exist.

The **TTL** command returns the remaining time to live(in seconds) for a key that has an
associated timeout. if the key does not have an associated **TTL**, it returns `-1`, and if the key
does not exist, it returns `-2`. The **PTTL** command does the same thing, but the return value is
in milliseconds rather than seconds.

```java
jedis.set("key1", "value1");
jedis.set("key2", "value2");
jedis.set("key3", "value3");

jedis.expire("key1", 1);
jedis.expireAt("key2", System.currentTimeMillis() / 1000 + 3);

long pttl1 = jedis.ttl("key1"); // 1 s
long ttl2 = jedis.pttl("key2"); // 2233 ms
long ttl3 = jedis.ttl("key3"); // -1
long ttlNonExist = jedis.ttl("non-exist"); // -2
```

### PERSIST and SETEX
The **PERSiST** command removes the existing timeout of a given key. Such a key will never expire,
unless a new timeout is set. It returns _1_ if  the timeout is removed or _0_ if the key does
not have associated timeout.


```java
jedis.setex("key1", 3, "value1");
jedis.set("key2", "value2");

long persist1 = jedis.persist("key1"); // 1
long persist2 = jedis.persist("key2"); // 0
```

### DEL and EXISTS
The **DEL** command removes one or many keys from the Redis and returns the number of removed keys
- the command cannot be undone.

The **EXISTS** command returns _1_ if a certain key exists or _0_ if it does not.


```java
jedis.set("key1", "value1");
jedis.set("key2", "value2");

Set<String> keys = jedis.keys("*"); // [ "key1", "key2" ]

long delKey1 = jedis.del("key1"); // 1
boolean key1Exists = jedis.exists("key1"); // false
boolean key2Exists = jedis.exists("key2"); // true
```

### PING

The **PING** command returns the string "PONG". It is useful for testing a server/client connection
and verifying that Redis is able to exchange data.

```java
String pong = jedis.ping(); // "PONG"
String pongMessage = jedis.ping("message"); // "message"
```

### Others

#### MIGRATE

The **MIGRATE** command moves a given key to a destination Redis server. This is an **atomic**
command, and during the key migration, **both Redis servers are blocked**. If the key already
exists in the destination, this command fails(unless the **REPLACE** parameter is specified).

There are tow optional parameters for the command **MIGRATE**, which can be used separately or
combined:

* **COPY**: Keep the key in the local Redis server and create a copy in the destination Redis server
* **REPLACE**: Replace the existing key in the destination server

#### SELECT

Redis has a concept of multiple databases, each of which is identified by a number from 0 to 15
(there are 16 database by default). It is **not recommended** to use multiple database with Redis.
A better approach would be to use multiple redis-server process rather than a single one, because
 multiple processes are able to use multiple CPU cores and give better insights into bottlenecks.
#### AUTH

The **AUTH** command is used to authorise a client to connect to Redis. If authorisation is enabled
on the Redis server, clients are allowed to run commands only after executing the **AUTH** command
with the right authorisation key.
#### SCRIPT KILL

The **SCRIPT KILL** command terminates the running Lua script if no write operations have been performed
by the script. If the script has performed any write operation the **SCRIPT KILL** command will
not be able to terminate it; in that case the **SHUTDOWN NOSAVE** command must be executed.

There are 3 possible return values for this command:
* **OK**
* **NOTBUSY**: No scripts in execution right now.
* **UNKILLABLE**: Sorry the script already executed write commands against the dataset. You can either wait
  the script termination or kill the server in a hard way using the SHUTDOWN NOSAVE command.

#### SHUTDOWN

The **SHUTDOWN** command stops all client, causes data to persist if enabled, and shut down the Redis server.

This command accepts one of the following operation parameters:
* **SAVE**: forces Redis to save all of the data to f file called _dump.rdb_, even if persistence is not enabled.
* **NOSAVE**: prevents Redis from persisting data to the disk, even if persistence is enabled.

#### OBJECT ENCODING

The **OBJECT ENCODING** command returns the encoding used bya given key.


