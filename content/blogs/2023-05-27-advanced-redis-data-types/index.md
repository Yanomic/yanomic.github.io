---
title: Advanced Redis Data Types
date: 2023-05-27T11:31:29+0800
categories:
- Redis
tags: 
- Redis
- Redis Data Types
- Jedis
- Redis Essentials
---

This chapter introduces the Set, Sorted Set, Bitmap, and HyperLogLog data types.
## Sets

A Set in Redis is an unordered collection of distinct Strings - it is not possible to add repeated elements to a Set.
Internally, a Set is implemented as a hash table, which is the reason that some operations are optimized.

The Set memory footprint will be reduced if all the members are integers, and the total number of elements can be as high
as the value of **set-max-intset-entries** configuration.

The maximum number of elements that a Set can hold is 2^32-1.

Some use cases for Sets are:
* **Data Filtering**: For example, filtering all flights that depart from a given city and arrive on another.
* **Data Grouping**: Grouping al users who viewed similar products(for example, recommendations on Amazon.com).
* **Membership Checking**: Checking whether a user is on a blacklist.

### Basics

The command **SADD** is responsible for adding one or many members to a Set.
**SADD** ignores members that already exist in a Set and returns the member of added members.
```java
jedis.sadd("user:max:favorite", "Arcade Fire", "Arctic Monkeys", "Belle & Sebastian", "Lenine");
jedis.sadd("user:hugo:favorite", "Daft Punk", "The Kooks", "Arctic Monkeys");
```
The command **SINTER** expects one or many Sets and returns an array with the members that belong to every Set.

```java
Set<String> sinter = jedis.sinter("user:max:favorite", "user:hugo:favorite"); // [ "Arctic Monkeys" ]
```
The command **SDIFF** expects one or many Sets. It returns an array with all members that belong to the first Set that do not exist in the Sets that follow it.

In this command, the key name order matters. Any key that does not exist is considered to be an empty Set.

```java
Set<String> sdiff1 = jedis.sdiff("user:max:favorite", "user:hugo:favorite"); // [ "Belle & Sebastian", "Lenine", "Arcade Fire" ]
Set<String> sdiff2 = jedis.sdiff("user:hugo:favorite", "user:max:favorite"); // [ "The Kooks", "Daft Punk" ]
```
The **SUNION** command expects one or many Sets. It returns any array with all members of all Sets.
The results has not repeated members.

```java
Set<String> sunion = jedis.sunion("user:max:favorite", "user:hugo:favorite"); // [ "The Kooks", "Daft Punk", "Belle & Sebastian", "Lenine", "Arcade Fire", "Arctic Monkeys" ]
```
The command **SRANDMEMBER** returns random member from a Set.
Because Sets are unordered, it is not possible to retrieve elements from a given position.

```java
String srandmember = jedis.srandmember("user:max:favorite"); // "Lenine"
```
The command **SISMEMBER** checks whether a member exists in a Set.

```java
boolean sismember = jedis.sismember("user:max:favorite", "Arctic Monkeys"); // true
```
The command **SREM** removes and returns members from a Set.
The command **SCARD** returns the number of members in a Set(also known as cardinality.

```java
long srem = jedis.srem("user:max:favorite", "Arctic Monkeys"); // 1
long sismember = jedis.sismember("user:max:favorite", "Arctic Monkeys"); // false
long cardinality = jedis.scard("user:max:favorite"); // 3
```
The command **SMEMBERS** returns an array with all members of a Set.

```java
Set<String> smembers = jedis.smembers("user:max:favorite"); // [ "Belle & Sebastian", "Lenine", "Arcade Fire" ]
```

### Deal Tracking System

Yipit, Groupon, and LivingSocial are examples of websites that send daily e-mails to users. These e-mails
contain a Set of deals(coupons and discounts) that users are interested in.These deals are based on the area
in which they live, as well their preferences.

This section will show how to create functions to mimic the features of these websites:
* Mark a deal as sent to user
* Check whether a user received a group of deals
* Gather metrics from the sent deals
Define the functions as follows:

```java
/**
 * Add a userId to a deal Set
 */
private void markDealAsSent(Jedis client, String dealId, String userId) {
    client.sadd(dealId, userId);
}

/**
 * Check whether a user id belongs to a deal Set. And sends a deal to a user only if it was not already sent
 */
private void sendDealIfNotSent(Jedis client, String dealId, String userId) {
    boolean isSent = client.sismember(dealId, userId);
    if (isSent) {
        logger.info("Deal {} was already sent to user {}", dealId, userId);
    } else {
        logger.info("Sending {} to user {}", dealId, userId);
        // code to send the deal to user
        markDealAsSent(client, dealId, userId);
    }
}

/**
 * @return all userIds that exist in all the deal Sets specified
 */
private Set<String> showUsersThatReceivedAllDeals(Jedis client, String... dealIds) {
    return client.sinter(dealIds);
}

/**
 * @return all userIds that exist in any of the deal Sets specified
 */
private Set<String> showUsersThatReceivedAtLeastOneDeal(Jedis client, String... dealIds) {
    return client.sunion(dealIds);
}
```
Use these functions to handle deal metrics.

```java
markDealAsSent(jedis, "deal:1", "user:1");
markDealAsSent(jedis, "deal:1", "user:2");
markDealAsSent(jedis, "deal:2", "user:1");
markDealAsSent(jedis, "deal:2", "user:3");

sendDealIfNotSent(jedis, "deal:1", "user:1");
sendDealIfNotSent(jedis, "deal:1", "user:2");
sendDealIfNotSent(jedis, "deal:1", "user:3");

Set<String> receivedAll = showUsersThatReceivedAllDeals(jedis, "deal:1", "deal:2"); // [ "user:1", "user:3" ]
Set<String> receivedAny = showUsersThatReceivedAtLeastOneDeal(jedis, "deal:1", "deal:2"); // [ "user:2", "user:1", "user:3" ]
````


## Sorted Sets

A Sorted Set is very similar to a Set, but each element of a Sorted Set has an **associated score**.
In other words, a Sorted Set is a collection of non-repeating Strings sorted by score.
It is possible to have elements with repeated scores. In this case, the repeated elements are ordered lexicographically.

Sorted Set operations are fast, but not as fast as Sets operations, because scores need to be compared. Adding, removing,
and updating an item in a Sorted Set runs in logarithmic time, O(log(N)), where N is the number of elements in a Sorted Set.

Internally, Sorted Sets are implemented as two separated data structure:
* A skip list with a hash table. A [skip list](https://en.wikipedia.org/wiki/Skip_list#:~:text=Skip%20lists%20are%20a%20probabilistic,faster%20and%20use%20less%20space.) is a data structure that allows fast search within a ordered sequence of elements.
* A ziplist, based on the **zset-max-ziplist-entries** and **zset-max-ziplist-value** configurations.

Sorted Sets could be used to:
* Build a real time waiting list for customer service
* Show a leaderboard of a massive online game that displays the top players, user with similar scores, or the scores of your friends
* Build an autocomplete system using millions of words
### Basics

The **ZADD** command adds one or many members to a Sorted Set. **ZADD** ignores members that already exist in a Sorted Set.
It returns the number of added members.
```java
long resultAlice = jedis.zadd("leader", 100, "Alice"); // 1
long resultZed = jedis.zadd("leader", 100, "Zed"); // 1
long resultHugo = jedis.zadd("leader", 102, "Hugo"); // 1
long resultMax = jedis.zadd("leader", 101, "Max"); // 1
```
There is a family of commands that can fetch ranges in a Sorted Set: **ZRANGE**, **ZRANGEBYLEX**, **ZRANGEBYSORE**, **ZREVRANGE**,
**ZREVRANGEBYLEX**, **ZREVRANGEBYSORE**. The only different is in how their result are sorted:
* **ZRANGE** returns elements from the lowest to the highest score, and it uses ascending lexicographical order if a score tie exists
* **ZREVRANGE** returns elements from the highest to the lowest score, and it uses descending lexicographical order if a score tie exists

Both of these commands expect a key name, a start index, and an end index. The indices are zero-based and can be positive or negative values.
```java
List<String> zrange = jedis.zrange("leader",0, 2); // [ "Alice", "Zed", "Max" ]
List<Tuple> zrangeWithScores = jedis.zrangeWithScores("leader", 0, -1); // [ "Alice:100.0", "Zed:100.0", "Max:101.0", "Hugo:102.0" ]
List<String> zrangeByScore = jedis.zrangeByScore("leader", 100, 101); // [ "Alice", "Zed", "Max" ]
```
When all the elements in a sorted set are inserted with the same score, in order to force lexicographical ordering,
the **ZRANGEBYLEX** command returns all the elements in the sorted set at key with a value between min and max.
```java
List<String> zrangeByLex = jedis.zrangeByLex("myzset", "[aaa", "(g"); // [ "b", "c", "d", "e", "f" ]
```

> If the elements in the sorted set have different scores, the returned elements are unspecified.
> See [more details](https://redis.io/commands/zrangebylex/).
{: .prompt-warning }

It is possible to retrieve a score or rank of a specific member in a Sorted Set using the commands **ZSCORE** and **ZRANK**/**ZREVRANK**.
```java
List<Tuple> allWithScores = jedis.zrangeWithScores("leader", 0, -1); // [[Alice,100.0], [Zed,100.0], [Max,101.0], [Hugo,102.0]]
Double zscore = jedis.zscore("leader", "Hugo"); // 102.0
Long zrank = jedis.zrank("leader", "Hugo"); // 3
```
The **ZREM** command removes a member from a Sorted Set.

```java
long zrem = jedis.zrem("leader", "Hugo"); // 1
```

### Leaderboard System

In this section, we are going to build a leaderboard application that can be used in an online game.
The application has the following features:
* Add and remove users
* Display the details of a user
* Show the top `x` users
* Show the users who are directly ranked about and below a given user

We will create a class `Leaderboard` and implement methods to add & remove user from the leaderboard
and fulfill the rest 3 requirements.

```java
@RequiredArgsConstructor
public static class Leaderboard {
    private final Jedis client;
    private final String key;

    /**
     * Add user to leaderboard
     */
    public void addUser(String username, double score) {
        this.client.zadd(this.key, score, username);
    }

    /**
     * Remove user from leaderboard
     */
    public void removeUser(String username) {
        this.client.zrem(this.key, username);
    }

    /**
     * @return score and rank for given user
     */
    public List<Number> getUserScoreAndRank(String username) {
        double score = this.client.zscore(this.key, username);
        long rank = this.client.zrevrank(this.key, username);
        return List.of(score, rank);
    }

    /**
     * @return top {@code n} users in the leaderboard
     */
    public List<Tuple> showTopUsers(int n) {
        return this.client.zrevrangeWithScores(this.key, 0, n - 1);
    }

    /**
     * @return {@code n} users around given user
     */
    public List<Tuple> getUsersAroundUser(String username, int n) {
        long rank = this.client.zrevrank(this.key, username);
        var startOffset = Math.max(rank - n / 2 + 1, 0);
        var endOffSet = startOffset + n - 1;
        return this.client.zrevrangeWithScores(this.key, startOffset, endOffSet);
    }
}
```
Let's try out these functions.
```java
Leaderboard leaderboard = new Leaderboard(jedis, "game-score");
leaderboard.addUser("Arthur", 70);
leaderboard.addUser("KC", 20);
leaderboard.addUser("Maxwell", 10);
leaderboard.addUser("Patrik", 30);
leaderboard.addUser("Ana", 60);
leaderboard.addUser("Felipe", 40);
leaderboard.addUser("Renata", 50);
leaderboard.addUser("Hugo", 80);

leaderboard.removeUser("Arthur");

List<Number> scoreAndRank = leaderboard.getUserScoreAndRank("Maxwell"); // [ "10.0", "6" ]
List<Tuple> top3Users = leaderboard.showTopUsers(3); // [ "Hugo:80.0", "Ana:60.0", "Renata:50.0" ]
List<Tuple> usersAround = leaderboard.getUsersAroundUser("Felipe", 5); // [ "Ana:60.0", "Renata:50.0", "Felipe:40.0", "Patrik:30.0", "KC:20.0" ]
```


## Bitmaps

A Bitmap is not a real data type in Redis. Under the hood, a Bitmap is a String. We can also say that a Bitmap
is a set of bit operations on a String. However, we are going to consider them as data types because Redis
provides commands to manipulate Strings as Bitmaps. Bitmaps are also known as bit arrays or bitsets.

A Bitmap is a sequence of bits where each bit can store 0 or 1. The Redis documentation refers to Bitmap indices are _offset_.

Bitmaps are memory efficient, support fast data lookups and can store up to 2^32 bits.
### Memory Efficiency

In order to see how a Bitmap can be memory efficient, we are going to compare a Bitmap to a Set.
The comparison scenario is an application that needs to store all user IDs that visited a website on a given day(
The Bitmap offset represents a user ID). We assume that our application has 5 million users in total, but only 2 million
users visited the website on that day, and that each user Id can be represented by 4 bytes(32 bits, which is the size of
an integer in a 32-bit computer).

The following table compare how much memory, in theory, a Bitmap and a Set implementation would take to store 2 million user IDs.

|Redis Key|Data type|Amount of bits per user|Stored users| Total memory|
|:-----|:-----|:-----|:-----|:-----|
|visits:2015-01-01:bitmap|Bitmap|1 bit|5 million|1 * 5,000,000 bits = 625 KB|
|visits:2015-01-01:set|Set|32 bit|2 million|32 * 2,000,000 bits = 8 MB|

The worst-case scenario for Bitmap implementation is represented in the preceding table. It had to
allocate memory for the entire user base even though only 2 million users visited the page.


> Updating 5 million records might take a few minutes.
{: .prompt-warning }

In reality, the difference is even more significant.

```java
for (var i = 0; i < 5_000_000; i++) {
    jedis.setbit("visits:2015-01-01:bitmap", i, i % 2 == 0);
}

for (var i = 0; i < 2_000_000; i++) {
    jedis.sadd("visits:2015-01-01:set", String.valueOf(i));
}

double bitmapMemInMB = jedis.memoryUsage("visits:2015-01-01:bitmap") / 1024.0 / 1024; // 1.0000686645507812 MB
double setMemInMB = jedis.memoryUsage("visits:2015-01-01:set") / 1024.0 / 1024; // 92.2940673828125 MB
```
However, Bitmaps are not always memory efficient. If we change the previous example to consider only 100
visits instead of 2 million, assuming the worst-case scenario again, the Bitmap implementation would not be
memory-efficient.

|Redis Key|Data type|Amount of bits per user|Stored users| Total memory|
|:-----|:-----|:-----|:-----|:-----|
|visits:2015-01-01:bitmap|Bitmap|1 bit|5 million|1 * 5,000,000 bits = 625 KB|
|visits:2015-01-01:set|Set|32 bit|100|32 * 100 bits = 3.125 KB|

Let's try with a real example.

```java
for (var i = 0; i < 10; i++) {
    jedis.setbit("visits:2015-01-01:bitmap:sparse", 5_000_000 - i, true);
}

for (var i = 0; i < 100; i++) {
    jedis.sadd("visits:2015-01-01:set:sparse", String.valueOf(i));
}

double bitmapMemInBytes = jedis.memoryUsage("visits:2015-01-01:bitmap:sparse") / 1024.0; // 1024.0859375 Bytes
double setMemInBytes = jedis.memoryUsage("visits:2015-01-01:set:sparse") / 1024.0; // 0.2734375 Bytes
```
Bitmaps are a great match for application that involve real-time analytics, because they can tell whether
a user performed an action(that is, "Did user _X_ pperform action _Y_ today?"), or how many times an event
occurred(that is, "How many users performed action _Y_ this week?").

### Basics

The **SETBIT** command is used to give a value to Bitmap offset and it accepts only 1 or 0.
If the Bitmap does not exist, it creates it.

```java
jedis.setbit("visits:2015-01-01", 10, true);
jedis.setbit("visits:2015-01-01", 15, true);

jedis.setbit("visits:2015-01-02", 10, true);
jedis.setbit("visits:2015-01-02", 11, true);

boolean value10 = jedis.getbit("visits:2015-01-01", 10); // true
boolean value15 = jedis.getbit("visits:2015-01-02", 15); // false
```
 The **BITCOUNT** command returns the number of bits marked as 1 in a Bitmap.

 ```java
long count20150101 = jedis.bitcount("visits:2015-01-01"); // 2
long count20150102 = jedis.bitcount("visits:2015-01-02"); // 2
 ```
 The **BITOP** command requires a bitwise operation, a destination key, and a list of keys to apply to that operation
 and store the result in the destination key.

 ```java
long bitOp = jedis.bitop(BitOP.OR, "totalUser", "visits:2015-01-01", "visits:2015-01-02"); // 2
long totalUserCount = jedis.bitcount("totalUser"); // 3
 ```

### Web Analytics

This section creates a simple web analytics system to save and count daily user visits to a website and then retrieve user IDs
from the visits on a given date.

Define functions for analysis.
```java
/**
 * Mark user with given {@code userId} as visited for given date, {@code date}(in format {@code yyyy-MM-dd})
 */
public void storeDailyVisit(Jedis client, String date, long userId) {
    var key = getKey("visits:daily:" + date);
    client.setbit(key, userId, true);
}

/**
 * @return number of visits on given date
 */
public long countVisits(Jedis client, String date) {
    var key = getKey("visits:daily:" + date);
    return client.bitcount(key);
}

/**
 * @return a list of user IDs that visited on given date
 */
public List<String> showUserIdsFromVisit(Jedis client, String date) {
    var key = getKey("visits:daily:" + date);
    String value = client.get(key);
    byte[] bytes = value.getBytes(StandardCharsets.UTF_8);
    List<String> userIds = new ArrayList<>();
    for (var byteIdx = 0; byteIdx < bytes.length; byteIdx++) {
        byte b = bytes[byteIdx];
        for (var bitIdx = 7; bitIdx >= 0; bitIdx--) {
            int visited = b >> bitIdx & 1;
            if (visited == 1) {
                userIds.add(String.valueOf((byteIdx * 8) + (7 - bitIdx)));
            }
        }
    }
    return userIds;
}
```
Demonstrate how the functions be used.
```java
storeDailyVisit(jedis, "2015-01-01", 1);
storeDailyVisit(jedis, "2015-01-01", 2);
storeDailyVisit(jedis, "2015-01-01", 10);
storeDailyVisit(jedis, "2015-01-01", 55);

long nVisits = countVisits(jedis, "2015-01-01"); // 4
List<Integer> userIds = showUserIdsFromVisit(jedis, "2015-01-01"); // [ "1", "2", "10", "55" ]
```


## HyperLogLogs

A HyperLogLog is not actually a real data type in Redis. Conceptually, a HyperLogLog is an algorithm that
uses randomization in order to provide a very good approvimation of the number of unique elements that exist
in a Set.

It is fascinating because it only runs in O(1), constant time, and uses a very small of memory - up to 12kB
of memory per key.

Although technically a HyperLogLog is not a real data typ, we are going to consider it as one because Redis provides
specific commands to manipulate Strings in order to calculate the cardinality of a set using the HyperLogLog algorithm.

The HyperLogLog algorithm is probabilistic, which means that it does not ensure 100 percent accuracy. The Redis implementation
of the HyperLogLog has a standard error of **0.81** percent. In theory, there is no practical limit for the cardinality of the sets
that can be counted.

> The HyperLogLog algorithm was described originally in the paper [HyperLogLog: the analysis of a near-optimal cardinality estimation algorithm](https://algo.inria.fr/flajolet/Publications/FlFuGaMe07.pdf) in 2007.
{: .prompt-info }

### Counting Unique Users

This section compares how much memory a HyperLogLog and a Set would need to count the unique visits to a given website per hour.

Let's look at the following scenario: a website has an average of 100,000 unique visits per hour. Each user who visits the page is identified by a UUID,
which is represented by a 32-byte string.

In order to store all unique visitors, a Redis key is created for every hour of a day. This means that in a day, there are 24 keys, and int a month there are 720(24*30) keys.

The following table shows how much memory, in theory, each data type would need to store 100,000 unique user visits in an hour, a day, and a month.

|Data type|Memory in an hour|Memory in a day|Memory in a month|
|:-----|:-----|:-----|:-----|
|HyperLogLog|12 KB|12 KB * 24 = 288 KB|288 KB * 30 = 8.4 MB|
|Set|32 bytes * 100000 = 3.2 MB|3.2 MB * 24 = 76.8 MB|76.8 MB * 30 = 2.25 GB|

Let's how it actually works in reality per hour.
Let's how it actually works in reality.
```java
for (var count = 0; count < 100_000; count++) {
    String user = "userId:" + count;
    jedis.pfadd("HyperLogLogKey", user);
    jedis.sadd("SetLogLogKey", user);
}

double hyperLogLogMemoryInKB = jedis.memoryUsage("HyperLogLogKey") / 1024.0; // 12.5546875 KB
double setMemoryInKB = jedis.memoryUsage("SetLogLogKey") / 1024.0; // 5442.359375 KB
```

### Basics

The command **PFDD** adds one or many strings to a HyperLogLog and returns 1 if cardinality was changed and 0 if it remains the same.

```java
long pfadd1 = jedis.pfadd("visits:2015-01-01", "carl", "max", "hugo", "arthur"); // 1
long pfadd2 = jedis.pfadd("visits:2015-01-01", "max", "hugo"); // 0
long pfadd3 = jedis.pfadd("visits:2015-01-02", "max", "kc", "hugo", "renata"); // 1
```
The command **PFCOUNT** accepts one or many keys as arguments.
When a single argument is specified, it returns the approximate cardinality.
When multiple keys are specified, it returns the approximate cardinality of the union of all unique elements.
```java
long count20150101 = jedis.pfcount("visits:2015-01-01"); // 4
long count20150102 = jedis.pfcount("visits:2015-01-02"); // 4
long countAll = jedis.pfcount("visits:2015-01-01", "visits:2015-01-02"); // 6
```
The command **PFMERGE** requires a destination key and one or many HyperLogLog keys are arguments.
It merges all the specified HyperLogLogs and stores the result in the destination key.

```java
String pfmerge = jedis.pfmerge("visits:total", "visits:2015-01-01", "visits:2015-01-02"); // "OK"
long countMerge = jedis.pfcount("visits:total");  // 6
```

> If the destination variable exists, it is treated as one of the source sets and its cardinality will be included in the cardinality of the computed HyperLogLog.
{: .prompt-warning }


### Counting and Retrieving Unique Website Visits

This section extends the previous example and adds an hour as granularity.
Later, it merges the 24 keys that represent each hour of a day into a single key.
Define functions to register visits, count visits and merge multiple visits for a specific date.

```java
/**
 * Register a unique visit
 *
 * @param date the date can be in {@code yyyy-MM-dd} or {@code yyyy-MM-ddTh} format.(for example, 2015-01-01, or 2015-01-01T2)
 */
public void addVisit(Jedis client, String date, String user) {
    String key = getKey("visits:" + date);
    client.pfadd(key, user);
}

/**
 * @return the number of unique visits on specific dates
 */
public long count(Jedis client, String... dates) {
    List<String> keys = Arrays.stream(dates).map(date -> getKey("visits:" + date)).toList();
    return client.pfcount(keys.toArray(new String[0]));
}

/**
 * Merge the visits on a given data
 */
public void aggregateDate(Jedis client, String date) {
    String dateKey = getKey("visits:" + date);
    List<String> hourKeys = new ArrayList<>();
    for (var i = 0; i < 24; i++) {
        hourKeys.add(getKey("visits:" + date + "T" + i));
    }
    client.pfmerge(dateKey, hourKeys.toArray(new String[0]));
}
```
Now let's simulate 200 users visiting the page 1,000 times in a period of 24 hours.

```java
for (var i = 0; i < 1_000; i++) {
    String username = "user_" + Math.floor(1 + Math.random() * 200);
    var hour = Math.floor(Math.random() * 24);
    addVisit(jedis, "2015-01-01T" + hour, username);
}

long count1 = count(jedis, "2015-01-01T0"); // 32
long count2 = count(jedis, "2015-01-01T5", "2015-01-01T6", "2015-01-01T7"); // 88
aggregateDate(jedis, "2015-01-01");

long count3 = count(jedis, "2015-01-01"); // 198
```


