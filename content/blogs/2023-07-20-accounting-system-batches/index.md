---
title: Accounting System Batches
date: 2023-07-20T17:10:23+0800
categories:
- Accounting
tags: 
- Accounting
- Bookkeeping
- Software Design
---

A batch groups journal entries within a specific time period. This structure helps isolate concerns into well-defined segments, allowing us to trust that journals in past batches have been properly closed. If discrepancies occur, we only need to review batches that are still open. Closed batches can also be used for generating financial reports.

Each batch is identified by the **account** it belongs to, the **register type** it applies to, and the **time period** it covers. Its status is determined by several key timestamps:

* **Pre-Open**: The current date is before the batch's period start date. No journal entries dated before this start date may be added. Creating batches in this state ahead of time helps prevent a bottleneck when many batches start on the same date.
* **Open**: The current date falls between the period's start and end dates (exclusive of the end date). Journal entries without a specified batch are automatically assigned to this one. Only one batch can be open at a time for a given account and register. The start and end date may be the same to represent an instantaneous event.
* **Post-Open**: The current date is after the period end date, but no period lock date has been set. Journal entries can still be booked into this batch, but only if explicitly assigned.
* **Fixed**: The period lock date is set, but the close date is not. Only journal entries dated after the period end date may be posted into this batch. This usually reflects an external event occurring related to the batch period.
* **Closed**: The period close date has been set. No further entries may be posted to this batch. The sum of all lines in the batch should equal zero.
