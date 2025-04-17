---
date: 2025-03-20T08:36:34+08:00
draft: true
title: API Design of Payment Service Providers - Get Payment Method List
categories: 
  - API Design of Payment Service Providers
tags: 
    - API
    - PSP
    - Payment
---
## Background
To begin a payment flow, the first essential step is for a merchant to retrieve all available payment methods, which may be filtered based on various factors such as customer type, region, or currency.

This article explores what a well-designed API for fetching payment methods should look like.

## The Design


### Headers

As with the [headers used in a payment request](../draft-api-design-of-payment-service-providers/#headers), the following headers are commonly included when requesting a list of available payment methods:

* **Authentication**, e.g.,  `Authorization: Bearer {token}`, `X-API-KEY: {api-key} `
* **Content Negotiation**, e.g., `Content-Type: application/json`, `Accept: application/x-www-form-urlencoded`
* **Merchant/Partner Identifier**, e.g., `X-Merchant-Code: {merchant-code}`, `X-Partner-ID: {partner-id}`
* **Payload Integrity Validation**, e.g.,  `X-Signature: sha256={signature}`
* **Rate Limiting**, e.g., `X-RateLimit-Limit: {limit}`, `X-RateLimit-Remaining: {remaining}`

{{% callout note %}}
<div class="mt-1 font-semibold text-lg">Which should not be included</div>
 
* **X-Idempotency-Key**, e.g., `X-Idempotency-Key: {idempotency-key}`

  Idempotency keys are used to safely retry state-changing operations such as POST, PUT, or DELETE. Since fetching payment methods is a read-only operation with no side effects, this header is not needed.

* **Caching**, `Cache-Control: no-cache`
  Similar to the initiate request, responses for available payment methods should not be cached to ensure merchants always receive the most up-to-date list.
{{% /callout %}}


### Body
Compared to the initiate payment request, the options request is relatively simple. We recommend using a flat structure for the request payload, which can be submitted in various formats such as `JSON`, `XML`, or `form-data`.


```markmap{height="250px"}
- Payload
  - **amount** | `amount`: The amount and currency details relevant to the intended transaction.
  - **shopperCountry** | `string`: The shopper’s country, expressed in ISO 3166-1 alpha-2 format.
  - **shopperReference** | `string`: A unique identifier for the shopper.
  - **device** | `device`: Details about the device used to initiate the transaction.
```

### Path
Path should used indicate the:
* **Resource identifier**, e.g., `/options`
* **Versioning**, e.g., `/v1/options`


### Query Parameter
Although query parameters are not widely used in PSP APIs, they can still serve useful purposes in specific contexts, such as:
* **Filtering & Search**, to narrow down results based on specific criteria, e.g., `?type=card`
* **Pagination & Limits**, to control the volume of data returned, e.g., `?limit=50`, `?offset=100`
* **Sorting & Ordering**, to specify how the data list in results are sorted, e.g., `?sort=type`, `?order=desc`
