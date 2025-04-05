---
date: '2025-03-20T08:36:34+08:00'
draft: true
title: 'Payment Processor API Comparision'
categories:
    - API
    - Payment
---


## Background
The post is to write about a comparision of the APIs of various payment processors. 
Will focus on the API design, user interactions and technilogies.

* Idempotency
* Types of integration
* Functionalities
* Channels, ECOM vs. POS


## Commom Issues

Payment systems have evolved over decades—and so have the platforms that power them. 
Leading Payment Service Providers (PSPs) have undergone extensive iterations to reach their current state. 
Yet, due to legacy constraints, rapid growth, and ever-changing industry demands, many PSP APIs still fall short of modern design expectations.

From our experience, we’ve observed a number of recurring issues across even the most established PSP platforms:

### Inconsistent Naming Conventions
APIs often mix different naming styles (e.g., `camelCase`, `snake_case`, `PascalCase`) across endpoints and fields, making integration error-prone. 
For example, a single API might use transaction_id in one endpoint and paymentRef in another, forcing developers to handle unnecessary variations.

### Unstructured or Overly Nested Fields

Some APIs bury critical data under excessive nesting, while others oversimplify into flat structures. Both approaches harm usability—either complicating parsing or obscuring logical relationships between fields.

### Unclear or Ambiguous Field Definitions

Field names and documentation often lack precision or consistency.
For instance, a field named status might return values like `1`, `completed`, or `SUCCESS`, with little or no explanation of what each value signifies.

### Duplicated or Redundant Fields
APIs sometimes expose the same data under different names (e.g., `amount` and `total_value`), creating confusion about which field to use. 
Worse, these fields may occasionally diverge in edge cases, introducing inconsistencies.

### Over-Flexibility Leading to Complexity
Some APIs allow too many ways to perform the same action (e.g., multiple optional parameters that change behavior unpredictably). 
While flexibility can be useful, excessive options increase integration complexity and testing overhead.

### Limited Flexibility Where It Matters
Conversely, some APIs enforce rigid, one-size-fits-all flows—such as requiring a fixed sequence of steps or mandatory fields that aren't relevant to all use cases.
This leads to brittle integrations or the need for error-prone workarounds.

### Non-standard Error Handling
Errors may return different formats (e.g., HTTP status codes, custom error objects, or plain-text messages) across endpoints. Some APIs bury critical errors in nested objects, making them hard to detect programmatically.

### Missing Idempotency Support
Support for idempotency keys—crucial for safe retries in payment flows—is often missing, inconsistently implemented, or poorly documented.
This increases the risk of duplicate charges or actions during network interruptions.

### Security Oversights

Some APIs fall short of modern security best practices, such as:

* Exposing internal resource identifiers, which may enable enumeration attacks.
* Returning overly verbose or sensitive response payloads.
* Using weak or outdated authentication mechanisms (e.g., static API keys in URLs).

## In a ideal world...
This actually triggers our thinking on how would an ideal PSP API look like.
The ideal API should be well-structured, consistent, extensibile, backward/forward compatible and free of problems mentioned above.

### What are fields to be included
A typical HTTP request consists of 4 key components that work together to communiate.

#### Headers
Headers are key-value pairs that provide metadata about the request or response. 
In terms of HTTP request to PSP, the fields should be used to handle following information:
* Authentication, e.g.,  `Authorization: Bearer {token}`, `X-API-KEY: {api-key} `
* Content Negotiation, e.g., `Content-Type: application/json`, `Accept: application/x-www-form-urlencoded`
* Idempotency, e.g., `X-Idempotency-Key: {idempotency-key}`
* Merchant/Partner identifier, e.g., `X-Merchant-Code: {merchant-code}`, `X-Partner-ID: {partner-id}`
* Payload Integrity validation, e.g.,  `X-Signature:sha256={signature}`
* Rate Limiting, e.g., `X-RateLimit-Limit: {limit}`, `X-RateLimit-Remaining: {remaining}`


##### Which should not be included
* Client info, e.g., `User-Agent: Mozilla/5.0`
The request is usually sent from merchant server to PSP, where the client in such request is the merchant server. 
The client info is not so useful for PSP to process and coudl easily be confused with the device that shopper use to initiate the transaction.

We prefer to include Client Info as part of the request body as we mention below.

* Caching, `Cache-Control: no-cache`
The purpose of the request to take action, e.g., making a payment, or query the latest state.
In neither case, caching makes any sense, e.g., the value will always be `no-cache`, which we suggest to just not send at all.
  
#### Body
Regardless of the format of the request(e.g., `json`, `xml`, `form-data`), request body should hold data that directly linked the action.

#### Path
Path should used indicate the:
* Resource identifier, e.g., `/payments/{payment_id}`
* Versioning, e.g., `/refunds/{refund_id}`
* Action or Sub-Resources, examples:
  * `/payments/{payment_id}/refunds`, list refunds of a payment
  * `/invoices/{invoice_id}/void`, void an invoice

#### Query Parameter
Query parameters are not so commonly used in PSPs. But it can be potentially used for:
* Filtering & Search, to narrow down results based on specific criteria, e.g., `?status=completed`
* Pagination & Limits, to control the volume of data returned, e.g., `?limit=50`, `?offset=100`
* Sorting & Ordering, to specify how the data list in results are sorted, e.g., `?sort=created_at`, `?order=desc`
* Field Selection, to reduce payload size by requesting only needed fields, `?fields=id,amount,status,customer.name,refunds`, 


> [Figma Diagram](https://www.figma.com/board/qWPsv48tqBdkTJmdshjFRG/Comparison-of-PSP-APIs?t=blAqwNyslrDv2Kjn-0)

This will cover the following major payment processors:
|PSP|URL|Online Documentation|
|:---:|:---:|:---|
|ACI|https://www.aciworldwide.com/| -- |
|Adyen|https://www.adyen.com/|https://docs.adyen.com/|
|Airwallex|||
|AmazonPay|||
|Stripe|https://stripe.com/|https://docs.stripe.com/api|
|Nuvei|||
|Klarna|||
|PayPal|||
|Rapyd|||
|RazorPay|||
|Square|||
|WorldPay|||
|2C2P|https://2c2p.com/|https://developer.2c2p.com/docs/general|

## Grossory
### Integration Type
Integration type refers to the ways merchant integrate with PSP, which typically includes:
* **API**: server to server communication made between merchants' backend and PSP's server
* **Component**: Merchant uses PSP's customizable frontend UI in their checkout page to initiate payment
* **Drop-in**: Merchant uses PSP's pre-built frontend UI in their checkout page to initiate payment
* **Hosted Checkout**: Merchant redirects shopper to a PSP-hosted checkout page to initiate payment
* **SDK**: A build-in library included in merchant mobile application to provide payment processing

### PayByLink
// TODO

## Payment Request Structure

Ideally, the data in payment related requests should be structural, giving the API user a clear overview of the kind of information that could be coverred by this API at the first glance.

### Guideline of Request Structure
Based on the research, we realize 3 best practices for a user-friendly structured APIs design:
* Fields to the request/transaction itself should appears as root level
* Group fields into section by the reason to collect those fields
* The hierarchy should be of no more than 4-level depth to maintain good understandability.

### Request Sections
* **Authentication**: Fields used to authenticate the transaction or derived from the authentication result.
* **Config**: Fields defining the behavior of the payment flow.
* **Device**: Fields describing the device through which the shopper initiates the transaction.
* **Fund**: Fields related to the funds used in the transaction.
* **Installments**: Fields detailing the installment plan.
* **Mandate**: Fields related to mandate details.
* **Merchant/Sub-Merchant**: Fields identifying the entity associated with the transaction.
* **Order**: Fields containing order details.
* **Payment** Method: Fields specifying the payment method used in the transaction.
* **Risk**: Fields related to transaction risk data.
* **Root**: Fields present at the root level of the request, typically the required transaction properties.
* **Tokenization**: Fields used to tokenize the payment method for future transactions.
* **Shopper**: Fields related to the shopper.
* **Splits**: Fields defining how the payment is split.


## Adyen

### Secure
ApiKey + HTTPS

|Integration Type|Status|
|:---:|:---:|
|API|✅|
|Component|✅|
|Drop-in|✅|
|Plugin|✅|
|Hosted Checkout|✅|
|SDK|✅|

### Header Parameters
|Header|Type|Description|
|:----:|:----:|:----|
|Idempotency-Key|string|A unique identifier for the message with a maximum of 64 characters (we recommend a UUID).|

### Integration Flows

## Stripe
### Secure
API keys + HTTPS

### Idempotency
HTTP HEADER: Idempotency-Key

## 2C2P

### Secure
JWT + JWS

* Quick Pay  --> PayByLink

Drop-in, customizable

|Integration Type|Status|
|:---:|:---:|
|API|✅|
|Drop-in|✅|
|Hosted Checkout|✅|
|SDK|✅|


## Reference
* [Open Payments](https://openpayments.dev/)