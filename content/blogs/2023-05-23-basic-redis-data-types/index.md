---
title: Basic Redis Data Types
date: 2023-05-23T23:55:53+0800
categories:
- Redis
tags: 
- Redis
- Redis Data Types
- Jedis
- Redis Essentials
---

Redis supports various basic data types.
Although you don't need to use all the data types,
it is important to understand how they work so that you can choose the right ones.
## Strings

Strings are the most versatile data types in Redis because they have many commands and multiple purposes.
A String can behave as an integer, float, text string, or bitmap based on its value and the commands used.
It can store any kind of data: text(XML), JSON, HTML, or raw text), integers, floats, or binary data(videos, images, or audio files).
A String value cannot exceed **512MB** of text or binary data.
### Cache Mechanisms

It is possible to cache text or binary data in Redis, which could be anything
from HTML pages and API responses to images and videos. A simple cache system could be implemented
with commands **SET**, **GET**, **MSET**, **MGET**.
* Store string to Redis.

   ```java
   jedis.set("string", "this is a string");
   ```
* Store byte[] to Redis.

   ```java
   jedis.set("string".getBytes(StandardCharsets.UTF_8), "this is a string".getBytes(StandardCharsets.UTF_8));
   ```
* Store multiple key-value pairs to Redis.

   ```java
   jedis.mset("key1", "value1", "key2", "values2", "key3", "values3");
   ```
* Get values stored on Redis.

   ```java
   List<String> values = jedis.mget("string", "byte", "key1", "key2", "key3"); // ["this is a string", "this is a byte", "value1", "values2", "values3"]
   ```

### Cache with Automatic Expiration

Strings combined with automatic key expiration can make a robust cache system using the commands
**SETEX**, **EXPIRE**, and **EXPIREAT**. This is very useful when database queries take a long time
to run and can be cached for a given period of time. Consequently, this avoids running those queries too
frequently and can give a performance boost to applications.
#### Expire key after set

The **TTL(Time To Live)** command returns one of the following:
* **A positive integer**: This is the number of seconds a given key has left to live
* **-2**: If the key is expired or does not exist
* **-1**: If the key exists but has no expiration time set


* Store key without expiry
   ```java
   jedis.set("keyNoExpiry", "valueNoExpiry");
   ```
* Check TTL of the key
   ```java
   long ttl = jedis.ttl(keyNoExpiry); // -1
   ```
* Check TTL after set expiry
   ```java
   jedis.expire("keyNoExpiry", 3);
   ttl = jedis.ttl("keyNoExpiry"); // 3
   ```
* Sleep for 1s
   ```java
   Thread.sleep(1000);
   ```
* Check TTL after sleep for 1s
   ```java
   ttl = jedis.ttl("keyNoExpiry"); // 2
   ```
* Sleep for another 2s
   ```java
   Thread.sleep(2000);
   ```
* Check TTL after sleep for another 2s
   ```java
   ttl = jedis.ttl("keyNoExpiry"); // -2
   String valueExpired = jedis.get("keyNoExpiry"); // null
   ```
#### Specify expiry while set

* Store key that will expire in 3s.
   ```java
   jedis.set("keyExpiryIn3s", "valueExpiryIn3s", SetParams.setParams().ex(3));
   ```
* Get value within 3s.
   ```java
   String valueBeforeExpiry = jedis.get("keyExpiryIn3s"); // "valueExpiryIn3s"
   ```
* Sleep for 3s
   ```java
   Thread.sleep(3000);
   ```
* Get value after 3s
   ```java
   String valueWithinExpiry = jedis.get("keyExpiryIn3s"); // null
   ```

### Counting

A counter can easily be implemented with Strings and the commands **INCR** and **INCRBY**.
Good examples of counters are page views, video views, and likes. Strings also provide other counting
commands, such as **DECR**, **DECRBY**, and **INCRFLOATBY**.
* Set counter to 100
    ```java
    jedis.set("counter", "100");
    ```
* Increase counter by 1
    ```java
    long afterIncrease = jedis.incr("counter");
    // afterIncrease = 101
    ```
* Increase counter by 5
    ```java
    long afterIncreaseBy5 = jedis.incrBy("counter", 5);
    // afterIncreaseBy5 = 106
    ```
* Decrease counter by 1
    ```java
    long afterDecrease = jedis.decr("counter");
    // afterDecrease = 105
    ```
* Increase counter by 100
    ```java
    long afterDecreaseBy100 = jedis.incrBy("counter", 100);
    // afterDecreaseBy100 = 205
    ```
* Increase counter by 1.8
    ```java
    double increaseByFloat = jedis.incrByFloat("counter", 1.8);
    // increaseByFloat = 206.8
    ```


## Lists

Lists are a very flexible data type in Redis because they can act like simple collection, stack, or queue.
Many event systems use Redis's Lists as their queue because Lists' operation ensure that concurrent systems
will not overlap popping items from a queue - List commands are atomic.

There are blocking commands in Redis's Lists, which means that when a client executes a blocking command in
an empty List, the client will wait for a new item to be added to the List.

Redis's Lists are linked lists. The maximum number of elements a List can hold is 2^32 - 1.

A List can be encoded and memory optimized if it has less elements than the **list-max-ziplist-entries** configuration
and if each element is smaller than the configuration **list-max-ziplist-value**(by bytes).
### Basics

Since Lists in Redis are linked lists, there are commands used to insert data into the head and tail
of a List.
The command **LPUSH** inserts data at the beginning of a list(left push)
and **RPUSH** insert data at the end of a List(right push)

```java
jedis.lpush("books", "Clean Code");
jedis.rpush("books", "Code Complete");
jedis.lpush("books", "Peopleware");
```
The command **LLEN** returns the length of a list.

```java
long length = jedis.llen(books); // 3
```
The command **LINDEX** returns the element in a given index(indices are zero-based).
It is possible to use negative indices to access the tail of the list.

```java
String firstItem = jedis.lindex("books", 0); // "Peopleware"
String lastItem = jedis.lindex("books", -1); // "Code Complete"
String nonExistItem = jedis.lindex("books", 999); // null
```
The command **LRANGE** returns an array with all elements from a given index range,
including the elements in both the start and end indices.

```java
List<String> range1 = jedis.lrange("books", 0, 1); // "Peopleware", "Clean Code"
List<String> range2 = jedis.lrange("books", 0, -1); // "Peopleware", "Clean Code", "Code Complete"
```
The command **LPOP** removes and returns the first element of a list.
The command **RPOP** removes and returns the last element of a list.

```java
String lpop = jedis.lpop("books"); // "Peopleware"
String rpop = jedis.rpop("books"); // "Code Complete"
List<String> remaining = jedis.lrange("books", 0 , 1); // "Clean Code"
```

### Event Queue

Lists are used in many tools, including Resque, Celery, and Logstash, as the queueing system.

In following section, we are going to implement a Queue prototype using Redis Lists.
#### Define `Queue` class

```java
@Getter
@Setter
public static class Queue {
    private final String queueName;
    private final Jedis redisClient;
    private final String queueKey;
    private int timeout = 0;
                   public Queue(String queueName, Jedis redisClient) {
        this.queueName = queueName;
        this.redisClient = redisClient;
        this.queueKey = "queues:" + queueName;
        this.timeout = 3; // 3s
    }

    /**
     * @return size of the queue
     */
    public long size() {
        return this.redisClient.llen(this.queueKey);
    }

    /**
     * Push {@code data} into the queue
     */
    public void push(String data) {
        this.redisClient.lpush(this.queueKey, data);
    }

    /**
     * Pop data from the queue
     * <p>
     * if queue is empty, block until data is added to the queue.
     * <p>
     * With this blocking api, we don't need to implement polling and no need to worry about empty list.
     */
    public String pop() {
        List<String> results = this.redisClient.brpop(this.timeout, this.queueKey);
        if (results == null) {
            return null;
        }
        return results.get(1);
    }
}
```
#### Create a Producer Worker

```java
public static class ProducerWorker extends Thread {
    private final static int MSG_NUM = 5;
    private final Jedis redisClient;
    private final String queueName;

    public ProducerWorker(Jedis redisClient, String queueName) {
        super("Producer");
        this.redisClient = redisClient;
        this.queueName = queueName;
    }

    @Override
    public void run() {
        Queue queue = new Queue(queueName, redisClient);
        for (var i = 0; i < MSG_NUM; i++) {
            queue.push("Hello world #" + i);
        }
        logger.info("[Producer] Created " + MSG_NUM + " messages");
    }
}
```
#### Create a Consumer Worker

```java
public static class ConsumerWorker extends Thread {
    private final Jedis redisClient;
    private final String queueName;

    public ConsumerWorker(Jedis redisClient, String queueName) {
        super("Consumer");
        this.redisClient = redisClient;
        this.queueName = queueName;
    }

    @Override
    public void run() {
        execute();
    }

    private void execute() {
        Queue queue = new Queue(queueName, redisClient);
        String message = queue.pop();
        if (message != null) {
            logger.info("[Consumer] Got message: " + message);
            long size = queue.size();
            logger.info(size + " messages left");
            execute();
        }
    }
}
```
#### Start Producer and Consumer Workers

```java
ProducerWorker producer = new ProducerWorker(jedis, "logs");
producer.start();
producer.join(2000);

long size = jedis.llen("queues:logs"); // 5

ConsumerWorker consumer = new ConsumerWorker(jedis, "logs");
consumer.start();
consumer.join(1000);

boolean isAlive = consumer.isAlive(); // true
consumer.join(5000);
isAlive = consumer.isAlive(); // false
```


## Hashes

Hashes are a great data structure for storing objects because you can map fields to values.
In a Hash both the field name and the value are Strings.

A big advantage of Hashes is that they are **memory-optimized**. The optimization is based on the
**hash-max-ziplist-entries** and **hash-max-ziplist-value** configurations.

Internally, a Hash can be a ziplist or a hash table. A ziplist is a dually linked list designed to be memory efficient.
In a ziplist, integers are stored as real integers rather than a sequence of characters.
Although a ziplist has memory optimizations, lookups are not performed in constant time.
On the other hand, a hash table has constant-time lookup but is not memory-optimized;
### Basics

The command **HSET** sets a value to a field of a given key.
The command **HMSET** sets multiple field values to a key, separated by spaces.
Both **HSET** and **HMSET** create a field if it does not exist or overwrite its value if it already exists.

The command **HINCRBY** increments a field by a given integer Both **HINCRBY** and **HINCRBYFLOAT** are similar to
**INCRBY** and **INCRBYFLOAT**.
* Set key-value pairs for Hashes.

```java
jedis.hset("movie", "title", "The Godfather");
jedis.hmset("movie", Map.of("years", "1972", "ratings", "9.2", "watchers", "1000000"));
jedis.hincrBy("movie", "watchers", 3);
```
* Get `title` field value for given key

```java
String title = jedis.hget("movie", "title"); // "The Godfather"
```
* Get multiple values for given key

```java
List<String> multipleValues = jedis.hmget("movie", "title", "watchers"); // [ "The Godfather", "1000003" ]
```
* Removed field for given key

```java
long removed = jedis.hdel("MOVIE", "watchers"); // 1 field removed
```
* List all fields for a given key

```java
Map<String, String> allValues = jedis.hgetAll("movie"); // { "title" : "The Godfather", "ratings" : "9.2", "years" : "1972" }
```
* List all field keys

```java
Set<String> keys = jedis.hkeys("movie"); // [ "ratings", "title", "years" ]
```
* List all field values

```java
List<String> values = jedis.hvals("movie"); // [ "The Godfather", "9.2", "1972" ]
```

### Voting System

This section creates a set of functions to save alink and then upvote and downvote it.
First, we create function `saveLink` to store value in Redis then add `upVote` and `downVote` to update the `score`.
Function `showDetails` shows all the fields in a Hash, based on the link Id.
```java
public void saveLink(Jedis client, String key, String author, String title, String link) {
    client.hmset(key, Map.of("author", author, "title", title, "link", link, "score", "0"));
}

public void upVote(Jedis client, String key) {
    client.hincrBy(key, "score", 1);
}

public void downVote(Jedis client, String key) {
    client.hincrBy(key, "score", -1);
}

public Map<String, String> showDetails(Jedis client, String key) {
    return client.hgetAll(key);
}
```
Use the previously defined functions to save two links, upvote and downvote them, and then display their details.

```java
saveLink(jedis, "link:123", "dayvson", "Maxwell Dayvson's Github page", "https://github.com/dayson");
upVote(jedis, "link:123");
upVote(jedis, "link:123");

saveLink(jedis, "link:456", "hltbra", "Hugo Tavares's Github page", "https://github.com/hltbra");
upVote(jedis, "link:456");
upVote(jedis, "link:456");
downVote(jedis, "link:456");

Map<String, String> details123 = showDetails(jedis, "link:123"); // { "title" : "Maxwell Dayvson's Github page", "link" : "https://github.com/dayson", "score" : "2", "author" : "dayvson" }
Map<String, String> details456 = showDetails(jedis, "link:456"); // { "title" : "Hugo Tavares's Github page", "link" : "https://github.com/hltbra", "score" : "1", "author" : "hltbra" }
```

> The command **HGETALL** may be a problem if a Hash has many fields and uses a lot of memory.
> It may slow down Redis because it needs to transfer all of that data through the network. A good alternative
> in such a scenario is the command **HSCAN**.
> 
> **HSCAN** does not return all the fields at once. It returns a cursor and the Hash fields with their values in chunks.
> **HSCAN** needs to be executed until the returned cursor is 0 in order to retrieve all the fields in a HASH.
{: .prompt-warning }

```java
Map<String, String> fields = new HashMap<>();
for (var i = 0; i < 1000; i++) {
    fields.put("field" + i, "value" + i);
}
jedis.hset("manyFields", fields);
ScanResult<Map.Entry<String, String>> scanResult = jedis.hscan("manyFields", ScanParams.SCAN_POINTER_START, new ScanParams().count(10));
int size = scanResult.getResult().size(); // 10
String cursor = scanResult.getCursor(); // "192"
boolean isCompleted = scanResult.isCompleteIteration(); // false
```


