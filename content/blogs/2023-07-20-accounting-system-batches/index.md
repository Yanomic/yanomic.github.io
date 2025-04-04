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

A batch is used to group journals in a time-bounded period.
With batches, we could separate our concerns into bounded blocks and safely assume journals in past batches have been
correctly.
Once discrepancy happens, we only need to check bathes that have not closed.
Closed batches could also used to generate financial reports.

Each batch is defined by the **account** it links to, the **register type** it is for, and the **period** it refers to.
There are a few timestamps that define the status a batch is in:

* **Pre-Open**: Current date is before the period start date. No journal with a booking date before the period start
  date may be booked into the batch. We could create batch in Pre-Open status upfront to avoid spike when creating a lot
  of batches at the same time(e.g., with the same period start date).
* **Open**: Current date is after the period start date, but before the period end date(closed-open interval). A
  journal entry that does not specify a batch will by default be assigned to this batch. No 2 batches are open at the
  same time. The period start date can be the same as period end date to indicate the batch was created for a (instant)
  event.
* **Post-Open**: Current date is after the period end date. However no period lock date has been filled in. Journals may
  still be **booked** into the batch, but only if explicitly specified.
* **Fixed**: The period lock date has been filled in, but not the period close date. Journals may only be **posted**
  into this batch if the booking date is after the period end date. This typically indicates the moment an external
  event has happened for the period of the batch.
* **Closed**: The closed date is filled in. No journals may be posted to the batch. The sum of all lines in the batch
  adds up tp zero.
