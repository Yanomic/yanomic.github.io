---
date: 2025-03-20T08:36:34+08:00
draft: true
title: API Design of Payment Service Providers
categories: 
  - API Design of Payment Service Providers
tags: 
    - API
    - PSP
    - Payment
---

## Background

Payment systems have evolved over decades—and so have the platforms that power them. 
Leading Payment Service Providers (PSPs) have undergone extensive iterations to reach their current state. 
Yet, due to legacy constraints, rapid growth, and ever-changing industry demands, many PSP APIs still fall short of modern design expectations.

From our experience, we’ve observed a number of recurring issues across even the most established PSP platforms:

* **Inconsistent Naming Conventions**
  
  APIs often mix different naming styles (e.g., `camelCase`, `snake_case`, `PascalCase`) across endpoints and fields, making integration error-prone. 
  For example, a single API might use transaction_id in one endpoint and paymentRef in another, forcing developers to handle unnecessary variations.

* **Unstructured or Overly Nested Fields**

  Some APIs bury critical data under excessive nesting, while others oversimplify into flat structures. Both approaches harm usability—either complicating parsing or obscuring logical relationships between fields.

* **Unclear or Ambiguous Field Definitions**

  Field names and documentation often lack precision or consistency.
  For instance, a field named status might return values like `1`, `completed`, or `SUCCESS`, with little or no explanation of what each value signifies.

* **Duplicated or Redundant Fields**
  
  APIs sometimes expose the same data under different names (e.g., `amount` and `total_value`), creating confusion about which field to use. 
  Worse, these fields may occasionally diverge in edge cases, introducing inconsistencies.

* **Over-Flexibility Leading to Complexity**
  
  Some APIs allow too many ways to perform the same action (e.g., multiple optional parameters that change behavior unpredictably). 
  While flexibility can be useful, excessive options increase integration complexity and testing overhead.

* **Limited Flexibility Where It Matters**
  
  Conversely, some APIs enforce rigid, one-size-fits-all flows—such as requiring a fixed sequence of steps or mandatory fields that aren't relevant to all use cases.
  This leads to brittle integrations or the need for error-prone workarounds.

* **Non-standard Error Handling**
  
  Errors may return different formats (e.g., HTTP status codes, custom error objects, or plain-text messages) across endpoints. Some APIs bury critical errors in nested objects, making them hard to detect programmatically.

* **Missing Idempotency Support**
  Support for idempotency keys—crucial for safe retries in payment flows—is often missing, inconsistently implemented, or poorly documented.
  This increases the risk of duplicate charges or actions during network interruptions.

* **Security Oversights**

  Some APIs fall short of modern security best practices, such as:

  * Exposing internal resource identifiers, which may enable enumeration attacks.
  * Returning overly verbose or sensitive response payloads.
  * Using weak or outdated authentication mechanisms (e.g., static API keys in URLs).

## In an ideal world...

> [Figma Diagram](https://www.figma.com/board/qWPsv48tqBdkTJmdshjFRG/Comparison-of-PSP-APIs?t=blAqwNyslrDv2Kjn-0)

This actually triggers our thinking on how would an ideal PSP API look like.
The ideal API should be well-structured, consistent, extensibile, backward/forward compatible and free of problems mentioned above.

Based on the research, we realize 3 best practices for a user-friendly structured APIs design:
* Fields to the request/transaction itself should appears as root level
* Group fields into section by the reason to collect those fields
* The hierarchy should be of no more than 4-level depth to maintain good understandability.


### Headers
Headers are key-value pairs that provide metadata about the request or response. 
In terms of HTTP request to PSP, the fields should be used to handle following information:
* Authentication, e.g.,  `Authorization: Bearer {token}`, `X-API-KEY: {api-key} `
* Content Negotiation, e.g., `Content-Type: application/json`, `Accept: application/x-www-form-urlencoded`
* Idempotency, e.g., `X-Idempotency-Key: {idempotency-key}`
* Merchant/Partner identifier, e.g., `X-Merchant-Code: {merchant-code}`, `X-Partner-ID: {partner-id}`
* Payload Integrity validation, e.g.,  `X-Signature: sha256={signature}`
* Rate Limiting, e.g., `X-RateLimit-Limit: {limit}`, `X-RateLimit-Remaining: {remaining}`


#### Which should not be included
* **Client information**, e.g., `User-Agent: Mozilla/5.0`

  The request is usually sent from merchant server to PSP, where the client in such request is the merchant server. 
  The client info is not so useful for PSP to process and coudl easily be confused with the device that shopper use to initiate the transaction.

  We prefer to include Client Info as part of the [Device](#device) as we mention below.

* Caching, `Cache-Control: no-cache`
  The purpose of the request to take action, e.g., making a payment, or query the latest state.
  In neither case, caching makes any sense, e.g., the value will always be `no-cache`, which we suggest to just not send at all.
  
### Body
Regardless of the format of the request(e.g., `json`, `xml`, `form-data`), we suggest following structure for payload:

```markmap
- Payload
  - [Root](#root)
  - [Authentication](#authentication)
  - [Configuration](#configuration)
  - [Device](#device)
  - [Fund](#fund)
  - [Forex](#forex)
  - [Installments](#installments)
  - [Mandate](#mandate)
  - [Merchant/Sub-Merchant](#merchantsub-merchant)
  - [Metadata](#metadata)
  - [Order](#order)
  - [Payment Method](#method)
  - [Risk](#risk)
  - [Tokenization](#tokenization)
  - [Shopper](#shopper)
  - [Splits](#splits)
```

Each category may also contain its own logical subgroups. To maintain clarity and prevent excessive nesting, we recommend that leaf nodes should not exceed **four** levels of depth from the root.


#### Root
Fields to be presented in the root level of the request.
Fields relates to the transaction directly, should be placed on the root level. 
Also, the fields that required for all kinds of transaction should be placed at root level.
```markmap{height="200px"}
- Root
  - **amount**, The amount information for the transaction.
  - **shopperStatement**, The text to be shown on the shopper's bank statement.
  - **sessionValidity**, The date and time until when the session remains valid.
```

#### Authentication
Fields to hold data used to authenticate the transaction.
```markmap{height="200px"}
- **Authentication**
  - **AccountInfo**
    - **accountAgeIndicator**, Indicates how long the cardholder's account has existed.
    - **accountChangeDate**, The last date when account details (e.g., email, phone, name) were modified.
    - **accountChangeIndicator**, Indicates when the account details were last changed.
  - **ThreeDS**
    - **dataOnly**, Indicates whether to perform data only flow.
    - **challengeWindowSize**, The size of the challenge window displayed to the shopper in the challenge flow.
    - **scaExemption**, Indicates the exemption type that you want to request for the particular transaction. 
    - **version**, Indicates your preference for the 3D Secure version.
  - **MpiData**
    - **authenticationResponse**, In 3D Secure 2, this is the transStatus from the challenge result. If the transaction was frictionless, omit this parameter.
    - **cavv**, The cardholder authentication value (base64 encoded, 20 bytes in a decoded form).
```

#### Configuration
Fields related to configuration of the payment behaviour.


#### Device
Fields related to device with which the transaction is made.

#### Fund
Fields related to fund used in the transaction.

#### Forex
Fields related multi-currency payment.

#### Installments
Fields related to installment plan details.

#### Mandate
Fields related to mandate details.

#### Merchant/Sub-Merchant
Fields related to the entity to which the transaction belongs.

#### Metadata
Stores additional, non-functional information related to the payment.

#### Order
Fields related to order details.

#### OpenInvoice
Fields related to OpenInvoice details.


#### Payment Method
Fields related to the payment method used in the transaction.

#### Risk
Fields related to risk data of the transaction.


#### Tokenization
Fields related to tokenize the payment methods for future use.

#### Shopper
Fields related to the shopper.

#### Splits
Fields related to how to split a payment.


### Path
Path should used indicate the:
* Resource identifier, e.g., `/payments/{payment_id}`
* Versioning, e.g., `/refunds/{refund_id}`
* Action or Sub-Resources, examples:
  * `/payments/{payment_id}/refunds`, list refunds of a payment
  * `/invoices/{invoice_id}/void`, void an invoice

### Query Parameter
Query parameters are not so commonly used in PSPs. But it can be potentially used for:
* Filtering & Search, to narrow down results based on specific criteria, e.g., `?status=completed`
* Pagination & Limits, to control the volume of data returned, e.g., `?limit=50`, `?offset=100`
* Sorting & Ordering, to specify how the data list in results are sorted, e.g., `?sort=created_at`, `?order=desc`
* Field Selection, to reduce payload size by requesting only needed fields, `?fields=id,amount,status,customer.name,refunds`, 

