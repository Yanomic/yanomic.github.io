---
title: Time Series(A Collection of Observations)
date: 2023-05-29T19:22:57+0800
categories:
- Redis
tags: 
- Redis
- Time Series
- Jedis
- Redis Essentials
---

A time series is an ordered sequence of values(data points) made over a time interval.
They can be adopted in any domain that needs temporal measurements.

Examples of time series are:
* Usage of specific words or terms in a newspaper over time
* Minimum wage year-by-year
* Daily changes in stock prices
* Product purchases month-by-month
* Climate change

Many time series system face challenges with storage, since a dataset can grow too large very quickly.
Another aspect of a time series is that as time goes by, the smallest granularities lose their values.

In this chapter, we will implement a time series in Redis using the Strings, Hash, Sorted Set and HyperLogLog data types.
## Building the Foundation

In this section, we will demonstrate how to create a simple time series library using Redis Strings.
The solution will be able to save an event that happened at a given timestamp with a method called `insert`.
It will also provide a method called `fetch` to fetch values within a range of timestamps.

The solution supports multiple granularities: day, hour, minute, and second. For instance, if an event happens
on date 01/01/2015 at 00:00:00(represented by the timestamp 1420070400), the following Redis keys will be incremented(one key per granularity):
* events:1sec:1420070400
* events:1min:1420070400
* events:1hour:1420070400
* events:1day:1420070400

All events are grouped by granularities, which means that an event that happened at 02:04:01 will be save with
an event that happened at 02:04:02 - both happened at the same minute. The same grouping rules apply to the hour and day granularity.
Define constants used in the example.

```java
/**
 * Define time units
 */
@RequiredArgsConstructor
public enum Unit {
    second(1),
    minute(60),
    hour(60 * 60),
    day(24 * 60 * 60),
    ;
    private final long duration; // in seconds
}

/**
 * Define various granularities and its properties
 */
@RequiredArgsConstructor
public enum Granularity {
    perSecond("1sec", Unit.hour.duration * 2, Unit.second.duration),
    perMinute("1min", Unit.day.duration * 7, Unit.minute.duration),
    perHour("1hour", Unit.day.duration * 60, Unit.hour.duration),
    perDay("1day", null, Unit.day.duration),
    ;

    private final String name;
    private final Long ttl;
    private final long duration;
}
```
Define class `TimeSeries` with follow methods:
* `insert`: to register an event that happened at given timestamp in multiple granularities
* `fetch`: to get results for given granularity within given time range
* other supporting function to calculate keys

```java
/**
 * Fetch result
 */
@RequiredArgsConstructor
public static class Result {
    private final long timestamp;
    private final long value;
}

@RequiredArgsConstructor
public static class TimeSeries {
    private final Jedis client;
    private final String namespace;

    /**
     * Register an event that happened at given timestamp in multiple granularities
     */
    public void insert(long timestampInSec) {
        for (Granularity granularity : Granularity.values()) {
            String key = this.getKey(granularity, timestampInSec);
            this.client.incr(key);
            if (granularity.ttl != null) {
                this.client.expire(key, granularity.ttl);
            }
        }
    }

    /**
     * Get results of given {@code granularity} within given time range
     */
    public List<Result> fetch(Granularity granularity, long beginTimestamp, long endTimestamp) {
        var begin = this.getRoundedTimestamp(beginTimestamp, granularity.duration);
        var end = this.getRoundedTimestamp(endTimestamp, granularity.duration);
        List<String> keys = new ArrayList<>();
        for (var timestamp = begin; timestamp <= end; timestamp += granularity.duration) {
            keys.add(this.getKey(granularity, timestamp));
        }

        List<String> values = this.client.mget(keys.toArray(new String[]{}));
        List<Result> results = new ArrayList<>();
        for (var i = 0; i < values.size(); i++) {
            var timestamp = beginTimestamp + i * granularity.duration;
            var value = Long.parseLong(values.get(i));
            results.add(new Result(timestamp, value));
        }
        return results;
    }

    /**
     * @return a key name in the format {@code namespace:granularity:timestamp}
     */
    private String getKey(Granularity granularity, long timestampInSec) {
        var roundedTimestamp = this.getRoundedTimestamp(timestampInSec, granularity.duration);
        return String.join(":", this.namespace, granularity.name, String.valueOf(roundedTimestamp));
    }

    /**
     * if the {@code precision} is 60, any timestamp between 0 and 60 will result in 0,
     * any timestamp between 60 and 120 will result in 60, and so on.
     *
     * @return a normalized timestamp based on given precision
     */
    private long getRoundedTimestamp(long timestampInSec, long precision) {
        return (long) (Math.floor((double) timestampInSec / precision) * precision);
    }
}
```

Let's try it out!

```java
TimeSeries item1Purchases = new TimeSeries(jedis, "purchases:item1");
var beginTimeStamp = 0L;
item1Purchases.insert(beginTimeStamp);
item1Purchases.insert(beginTimeStamp + 1);
item1Purchases.insert(beginTimeStamp + 1);
item1Purchases.insert(beginTimeStamp + 3);
item1Purchases.insert(beginTimeStamp + 61);

List<Result> results4 = item1Purchases.fetch(Granularity.perSecond, beginTimeStamp, beginTimeStamp + 4); // [ "0:1", "1:2", "2:0", "3:1", "4:0" ]
List<Result> results120 = item1Purchases.fetch(Granularity.perMinute, beginTimeStamp, beginTimeStamp + 120); // [ "0:4", "60:1", "120:0" ]
```


## Optimizing with Hashes

The previous time series implementation use one Redis key for each second, minutes, hour and day.
In scenario where an event is inserted every second, there will be 87,865 keys in Redis for a full day.:
* 86,400 keys for the `1sec` granularity(24 * 60 * 60)
* 1,440 keys for the `1min` granularity(24 * 60)
* 24 keys for the `1hour` granularity(24 * 1)
* 1 key for the `1day` granularity

This is an enormous number of keys per day, and this number grows linearly over time. In the scenario where events inserted
every second for 24 hours, Redis will need to allocate about 11MB memory as per [the benchmark](https://gist.github.com/hltbra/2fbf5310aabbecee68c5).

We can optimize this solution by using Hashes instead of Strings. Small Hashes are encoded in a different data structure, called a `ziplist`.
The structure is memory optimized. There are two conditions for a Hash to be encoded as a ziplist and **both have to be respected**:
* it must have fewer fields than the threshold set in the configuration `hash-max-ziplist-entries`, default value is 512
* No field value can be larger than `hash-max-ziplist-value`, default value is 64 bytes.

In order to use Hashes and save memory space, the next solution will group multiple keys into a single Hash.

In a scenario where there is only the `1sec` granularity and there are data points across six different timestamps, the String solution will create following keys:

|Key Name|Key Value|
|:-----|:-----|
|namespace:1sec:0|10|
|namespace:1sec:1|15|
|namespace:1sec:2|25|
|namespace:1sec:3|100|
|namespace:1sec:4|200|
|namespace:1sec:5|300|

And if we use a Hash instead and create groups of three keys:

|Key Name|Field Name|Field Value|
|:-----|:-----|:-----|
|namespace:1sec:0|0|10|
||1|15|
||2|25|


|Key Name|Field Name|Field Value|
|:-----|:-----|:-----|
|namespace:1sec:3|3|100|
||4|200|
||5|300|

The Hash implementation will have the same methods. We will highlight the modified lines.

The field `quantity` is added to the granularity and used to determine the Hash distribution:
* 1sec granularity: Stores a maximum of 300 timestamps of 1 second each(5 minutes of data points)
* 1min granularity: Stores a maximum of 480 timestamps of 1 minute each(8 hours of data points)
* 1hour granularity: Stores a maximum of 240 timestamps of 1 hour each(10 days of data points)
* 1day granularity: Stores a maximum of 30 timestamps of 1 day each(30 days of data points)

The number are chosen based on the default Redis configuration(**hash-max-ziplist-entries** is 512).

```java
/**
 * Define various granularities and its properties
 */
@RequiredArgsConstructor
public enum Granularity {
    perSecond("1sec", Unit.hour.duration * 2, Unit.second.duration, Unit.minute.duration * 5),
    perMinute("1min", Unit.day.duration * 7, Unit.minute.duration, Unit.hour.duration * 8),
    perHour("1hour", Unit.day.duration * 60, Unit.hour.duration, Unit.day.duration * 10),
    perDay("1day", null, Unit.day.duration, Unit.day.duration * 30),
    ;

    private final String name;
    private final Long ttl;
    private final long duration;
    private final long quantity;
}
```

Update the `insert` method to use **HINCRBY**.
```java
/**
 * Register an event that happened at given timestamp in multiple granularities
 */
public void insert(long timestampInSec) {
    for (Granularity granularity : Granularity.values()) {
        String key = this.getKey(granularity, timestampInSec);
        String fieldName = String.valueOf(this.getRoundedTimestamp(timestampInSec, granularity.duration));
        this.client.hincrBy(key, fieldName, 1);
        if (granularity.ttl != null) {
            this.client.expire(key, granularity.ttl);
        }
    }
}
```

Update `getKey` to use `Granularity.quantity` instead of `Granularity.duration`.

```java
/**
 * @return a key name in the format {@code namespace:granularity:timestamp}
 */
private String getKey(Granularity granularity, long timestampInSec) {
    var roundedTimestamp = this.getRoundedTimestamp(timestampInSec, granularity.quantity);
    return String.join(":", this.namespace, granularity.name, String.valueOf(roundedTimestamp));
}
```
Since we need to get multiple keys and fields in one go, we will create a transaction to fetch them from the Hash.

```java
/**
 * Get results of given {@code granularity} within given time range
 */
public List<Result> fetch(Granularity granularity, long beginTimestamp, long endTimestamp) {
    var begin = this.getRoundedTimestamp(beginTimestamp, granularity.duration);
    var end = this.getRoundedTimestamp(endTimestamp, granularity.duration);

    Transaction multi = this.client.multi();
    for (var timestamp = begin; timestamp <= end; timestamp += granularity.duration) {
        String key = this.getKey(granularity, timestamp);
        String fieldName = String.valueOf(this.getRoundedTimestamp(timestamp, granularity.duration));
        multi.hget(key, fieldName);
    }

    List<Object> values = multi.exec();

    List<Result> results = new ArrayList<>();
    for (var i = 0; i < values.size(); i++) {
        var timestamp = beginTimestamp + i * granularity.duration;
        var value = values.get(i) == null ? 0L : Long.parseLong((String) values.get(i));
        results.add(new Result(timestamp, value));
    }
    return results;
}
```
Let's run the same script again!

```java
TimeSeries item1Purchases = new TimeSeries(jedis, "purchases:item1");
var beginTimeStamp = 0L;
item1Purchases.insert(beginTimeStamp);
item1Purchases.insert(beginTimeStamp + 1);
item1Purchases.insert(beginTimeStamp + 1);
item1Purchases.insert(beginTimeStamp + 3);
item1Purchases.insert(beginTimeStamp + 61);

List<Result> results4 = item1Purchases.fetch(Granularity.perSecond, beginTimeStamp, beginTimeStamp + 4); // [ "0:1", "1:2", "2:0", "3:1", "4:0" ]
List<Result> results120 = item1Purchases.fetch(Granularity.perMinute, beginTimeStamp, beginTimeStamp + 120); // [ "0:4", "60:1", "120:0" ]
```


## Adding Uniqueness with Sorted Sets and HyperLogLog

This section presents two different Time Series implementations that support unique insertions.
The first implementation uses Sorted Set and it is based on previous Hash implementation.
The second implementation uses HyperLogLog, and it is based on previous String implementation.

Each solution has pros and cons. The proper solution should be chosen based on how much data needed to be stored and how accurate it needs to be.
* The Sorted Set solution works well and is 100% accurate
* The HyperLogLog solution uses less memory than the Sorted Set solution, but it is only 99.19% accurate
### Sorted Set Implementation

Similar to Hash implementation, we need to define the quantity of each granularity.
```java
/**
 * Define various granularities and its properties
 */
@RequiredArgsConstructor
public enum Granularity {
    perSecond("1sec", Unit.hour.duration * 2, Unit.second.duration, Unit.minute.duration * 2),
    perMinute("1min", Unit.day.duration * 7, Unit.minute.duration, Unit.hour.duration * 2),
    perHour("1hour", Unit.day.duration * 60, Unit.hour.duration, Unit.day.duration * 5),
    perDay("1day", null, Unit.day.duration, Unit.day.duration * 30),
    ;

    private final String name;
    private final Long ttl;
    private final long duration;
    private final long quantity;
}
```

The field `quantity` was changed based on the Sorted Set configuration **zset-max-ziplist-entries**, which is default to 128.
* 1sec granularity: Stores a maximum of 120 timestamps of 1 second each(2 minutes of data points)
* 1min granularity: Stores a maximum of 120 timestamps of 1 minute each(2 hours of data points)
* 1hour granularity: Stores a maximum of 120 timestamps of 1 hour each(5 days of data points)
* 1day granularity: Stores a maximum of 30 timestamps of 1 day each(30 days of data points)


While inserting into Sorted Set, we add a new parameter, the `thing`, as unique value to be stored.
Together with timestamp, we create a unique identifier `timestamp:thing` to represent an event for `thing` happened at `timestamp`.
We use timestamp as the score for the identified so that we can query by time range later.

```java
/**
 * Register an event that happened at given timestamp in multiple granularities
 */
public void insert(long timestampInSec, String thing) {
    for (Granularity granularity : Granularity.values()) {
        String key = this.getKey(granularity, timestampInSec);
        long timestampScore = this.getRoundedTimestamp(timestampInSec, granularity.duration);
        String member = String.join(":", String.valueOf(timestampScore), thing);
        this.client.zadd(key, timestampScore, member);
        if (granularity.ttl != null) {
            this.client.expire(key, granularity.ttl);
        }
    }
}
```

While fetching, we use command **ZCOUNT** to calculate the unique values for each given time range.

```java
/**
 * Get results of given {@code granularity} within given time range
 */
public List<Result> fetch(Granularity granularity, long beginTimestamp, long endTimestamp) {
    var begin = this.getRoundedTimestamp(beginTimestamp, granularity.duration);
    var end = this.getRoundedTimestamp(endTimestamp, granularity.duration);

    Transaction multi = this.client.multi();
    for (var timestamp = begin; timestamp <= end; timestamp += granularity.duration) {
        String key = this.getKey(granularity, timestamp);
        multi.zcount(key, timestamp, timestamp);
    }

    List<Object> values = multi.exec();

    List<Result> results = new ArrayList<>();
    for (var i = 0; i < values.size(); i++) {
        var timestamp = beginTimestamp + i * granularity.duration;
        var value = values.get(i) == null ? 0L : (Long) values.get(i);
        results.add(new Result(timestamp, value));
    }
    return results;
}
```
Let's try with some duplicated events!

```java
TimeSeries item1Purchases = new TimeSeries(jedis, "purchases:item1");
var beginTimeStamp = 0L;
item1Purchases.insert(beginTimeStamp, "user:max");
item1Purchases.insert(beginTimeStamp, "user:max"); // inserting duplicated events
item1Purchases.insert(beginTimeStamp + 1, "user:hugo");
item1Purchases.insert(beginTimeStamp + 1, "user:renata");
item1Purchases.insert(beginTimeStamp + 3, "user:hugo");
item1Purchases.insert(beginTimeStamp + 61, "user:kc");

List<Result> results4 = item1Purchases.fetch(Granularity.perSecond, beginTimeStamp, beginTimeStamp + 4); // [ "0:1", "1:2", "2:0", "3:1", "4:0" ]
List<Result> results120 = item1Purchases.fetch(Granularity.perMinute, beginTimeStamp, beginTimeStamp + 120); // [ "0:3", "60:1", "120:0" ]
```

### HyperLogLog Implementation

The HyperLogLog implementation does not perform any key grouping. It uses one key per timestamp.
Compared to the String implementation, it changes the `insert` method to use **PFADD** instead of **INCRBY**,
and changes the `fetch` method to make multiple calls to **PFCOUNT** instead of **MGET**.

```java
/**
 * Register an event that happened at given timestamp in multiple granularities
 */
public void insert(long timestampInSec, String thing) {
    for (Granularity granularity : Granularity.values()) {
        String key = this.getKey(granularity, timestampInSec);
        this.client.incr(key);
        this.client.pfadd(key, thing);
        if (granularity.ttl != null) {
            this.client.expire(key, granularity.ttl);
        }
    }
}
```

```java
/**
 * Get results of given {@code granularity} within given time range
 */
public List<Result> fetch(Granularity granularity, long beginTimestamp, long endTimestamp) {
    var begin = this.getRoundedTimestamp(beginTimestamp, granularity.duration);
    var end = this.getRoundedTimestamp(endTimestamp, granularity.duration);

    Transaction multi = this.client.multi();
    for (var timestamp = begin; timestamp <= end; timestamp += granularity.duration) {
        multi.pfcount(this.getKey(granularity, timestamp));
    }

    List<Object> values = multi.exec();
    List<Result> results = new ArrayList<>();
    for (var i = 0; i < values.size(); i++) {
        var timestamp = beginTimestamp + i * granularity.duration;
        var value = values.get(i) == null ? 0L : (Long) values.get(i);
        results.add(new Result(timestamp, value));
    }
    return results;
}
```
Here comes the last shot!

```java
TimeSeries item1Purchases = new TimeSeries(jedis, "purchases:item1");
var beginTimeStamp = 0L;
item1Purchases.insert(beginTimeStamp, "user:max");
item1Purchases.insert(beginTimeStamp, "user:max"); // inserting duplicated events
item1Purchases.insert(beginTimeStamp + 1, "user:hugo");
item1Purchases.insert(beginTimeStamp + 1, "user:renata");
item1Purchases.insert(beginTimeStamp + 3, "user:hugo");
item1Purchases.insert(beginTimeStamp + 61, "user:kc");

List<Result> results4 = item1Purchases.fetch(Granularity.perSecond, beginTimeStamp, beginTimeStamp + 4); // [ "0:1", "1:2", "2:0", "3:1", "4:0" ]
List<Result> results120 = item1Purchases.fetch(Granularity.perMinute, beginTimeStamp, beginTimeStamp + 120); // [ "0:3", "60:1", "120:0" ]
```


