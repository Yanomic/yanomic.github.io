---
title: Accounting System Basics
date: 2023-07-16T11:10:33+0800
categories:
- Accounting
tags: 
- Accounting
- Bookkeeping
- Software Design
---

## Double-Entry Booking

[Double-entry bookkeeping](https://en.wikipedia.org/wiki/Double-entry_bookkeeping) is a method of bookkeeping that
relies on a two-sided accounting entry to maintain financial information. Every entry to an account requires a
corresponding and opposite entry to a different account.

## Account

An account can represent the corporate entities involved in the transactions.
The accounts form a hierarchy describing the legal ownership relation.

Some basic properties of an account could have:

* Account Type
* Account Code
* Description

## Register

A register represents a specific ledger for an account that can be associated with a status.
Different accounts have different registers since we might need to track different things.

Some basic properties of an account could have:

* Register type
* Description

## Journal

A journal represents an event that moves the balances of a transaction between registers and is associated to one or
more transactions. As a result, the status of a transaction can change.

A journal is atomic.

A journal has two timestamps:

* Post date: moment the journal was created
* Book date: moment the balance changes are effectively available

## Transactions

A transaction represents an event(external) that needs to be tracked and accounted for. A transaction is always owned by
one account, and may have a parent transaction to indicate a sub-item that needs to be tracked independently.

Some basic properties of an transaction could have:

* transaction id
* transaction type

## Summary

* A external(to accounting system) event could trigger one or more transactions in accounting system
* A transaction could involve 2 or more accounts
* A transaction could create records in 1 or more registers/ledgers
* Each register will generate 1 or more journal entries for involved accounts
* Journal entries will be grouped into journals to ensure atomicity(Each journal is atomic).
* A transaction could link to multiple journals(money transfer from A->C via B instead of direct transfer, for cost
  efficiency purpose)
* A journal could link to multiple transactions(payment transaction have journals for Authorisation and Commission
  separately)

