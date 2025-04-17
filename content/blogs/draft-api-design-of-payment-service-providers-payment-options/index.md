---
date: 2025-03-20T08:36:34+08:00
draft: true
title: API Design of Payment Service Providers – Fetching Available Payment Methods
categories: 
  - API Design of Payment Service Providers
tags: 
  - API
  - PSP
  - Payment
---
## Background
The initial step in any payment flow involves retrieving the list of available payment methods. This list can be filtered based on various criteria, such as customer type, region, or currency.

This article examines the design of an effective API for fetching available payment methods.

## The Request

### Headers

Similar to the [headers used in a payment request](../draft-api-design-of-payment-service-providers/#headers), the following headers are typically included when requesting a list of available payment methods:

* **Authentication**, e.g., `Authorization: Bearer {token}`, `X-API-KEY: {api-key} `
* **Content Negotiation**, e.g., `Content-Type: application/json`, `Accept: application/x-www-form-urlencoded`
* **Merchant/Partner Identifier**, e.g., `X-Merchant-Code: {merchant-code}`, `X-Partner-ID: {partner-id}`
* **Payload Integrity Validation**, e.g., `X-Signature: sha256={signature}`
* **Rate Limiting**, e.g., `X-RateLimit-Limit: {limit}`, `X-RateLimit-Remaining: {remaining}`

{{% callout note %}}
<div class="mt-1 font-semibold text-lg">Headers to exclude</div>
 
* **X-Idempotency-Key**, e.g. ,`X-Idempotency-Key: {idempotency-key}`

  Idempotency keys are used to safely retry state-changing operations such as POST, PUT, or DELETE. Since fetching payment methods is a read-only operation with no side effects, this header is not needed.

* **Caching**, `Cache-Control: no-cache`
  Similar to the initiate request, responses for available payment methods should not be cached to ensure merchants always receive the most up-to-date list.
{{% /callout %}}


### Body
In comparison to an initiate payment request, the options request is relatively straightforward. We recommend a flat structure for the request payload, which can be submitted in formats such as `JSON`, `XML`, or `form-data`.


```markmap{height="250px"}
- Payload
    - **allowedPaymentMethods** | `[]string`: A list of payment methods permitted for the shopper.
  - **amount** | `amount`: Details regarding the transaction amount and currency.
  - **blockedPaymentMethods** | `[]string`: A list of payment methods that should be excluded.
  - **shopperCountry** | `string`: The shopper’s country in ISO 3166-1 alpha-2 format.
  - **shopperLocale** | `string`: A combination of language and country codes to define the locale.
  - **shopperReference** | `string`: A unique reference identifying the shopper.
  - **device** | `device`: Information about the device used to initiate the transaction.
```

### Path

The request path should indicate the following:
* **Resource identifier**, e.g., `/options`
* **API versioning**, e.g., `/v1/options`


### Query Parameter
Although query parameters are not frequently utilized in PSP APIs, they can serve specific purposes:
* **Filtering & Search**, to narrow down results based on specific criteria, e.g., `?type=card`
* **Pagination & Limits**, to control the volume of data returned, e.g., `?limit=50`, `?offset=100`
* **Sorting & Ordering**, to specify how the data list in results are sorted, e.g., `?sort=type`, `?order=desc`


## The Response

### Response Code
The response code should consistently be `200 OK`.

### Body

```markmap{height="200px"}
- Options Response
  - **altopusId** | `string`: A unique reference for the request.
  - **options** | `[]option`: A list of available payment options.
    - **variant** | `string`: Unique identifier for the payment variant.
    - **name** | `string`: User-friendly name for the payment method.
    - **tokens** | `[]TokenDetail`: A list of stored tokens associated with this payment variant.
```
Each `TokenDetail` provides additional information about a stored token:

```markmap{height="250px"}  
- TokenDetail
  - **reference** | `string`: The unique identifier for the stored token.
  - **useCases** | `[]string`: Supported use cases for this token.
  - **details** | `map<string, string>`: Additional metadata related to the token.
```

The `details` field contains variant-specific data. Some keys may be shared across variants, such as mandate information.

#### Token detail for card-based methods

```markmap{height="200px"}
- CardTokenDetail
  - **expiryMonth** | `string`: Two-digit expiration month of the card.
  - **expiryYear** | `string`: Last two digits of the expiration year.
  - **holderName** | `string`: Name of the cardholder.
  - **summary** | `string`: Last four digits of the card number.
```

## Behind the scene