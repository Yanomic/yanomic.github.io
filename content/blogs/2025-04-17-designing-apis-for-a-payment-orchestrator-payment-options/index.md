---
date: 2025-04-17T21:32:54+08:00
title: Designing APIs for a Payment Orchestrator – Fetching Available Payment Methods
categories: 
  - Designing APIs for a Payment Orchestrator
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
  - **amount** | `amount`: Details regarding the transaction amount and currency.
  - **shopperCountry** | `string`: The shopper’s country in ISO 3166-1 alpha-2 format.
  - **shopperLocale** | `string`: A combination of language and country codes to define the locale.
  - **shopperReference** | `string`: A unique reference identifying the shopper. Used to include store tokens in response.
  - **device** | `device`: Information about the device used to initiate the transaction.
```

### Path

The request path should indicate the following:
* **Resource identifier**, e.g., `/options`
* **API versioning**, e.g., `/v1/options`


### Query Parameter
Although query parameters are not frequently utilized in PSP APIs, they can serve specific purposes:
* **Filtering & Search**, to narrow down results based on specific criteria, e.g., `?allowed=card,alipay&blocked=upi`
* **Pagination & Limits**, to control the volume of data returned, e.g., `?limit=50`, `?offset=100`
* **Sorting & Ordering**, to specify how the data list in results are sorted, e.g., `?sort=type`, `?order=desc`


## The Response

### HTTP Status Code
The response code should consistently be `200 OK`.

### Body

```markmap{height="150px"}
- **Options Response**
  - **altopusId** | `string`: A unique reference for the request.
  - **options** | `[]Option`: A list of available payment options.
```
Each `Option` contains details of a payment option:
```markmap{height="150px"}
- **Option**
    - **variant** | `string`: Unique identifier for the payment variant.
    - **name** | `string`: User-friendly name for the payment variant.
    - **tokens** | `[]TokenDetail`: A list of stored tokens associated with this payment variant.
```

Each `TokenDetail` provides additional information about a stored token:

```markmap{height="250px"}  
- **TokenDetail**
  - **reference** | `string`: The unique identifier for the stored token.
  - **useCases** | `[]string`: Supported use cases for this token.
  - **details** | `map<string, string>`: Additional metadata related to the token.
```

The `details` field contains variant-specific data. Some keys may be shared across variants, such as mandate information.

* Token detail for card-based methods

```markmap{height="200px"}
- CardTokenDetail
  - **expiryMonth** | `string`: Two-digit expiration month of the card.
  - **expiryYear** | `string`: Last two digits of the expiration year.
  - **holderName** | `string`: Name of the cardholder.
  - **summary** | `string`: Last four digits of the card number.
```

* (More details to be added)

## Behind the Scene

As a payment orchestrator, we do not offer our own payment methods. Instead, we depend on connected Payment Service Providers (PSPs) to provide these options. The orchestrator's role is to aggregate and filter the methods available from the PSPs; it cannot expose any methods that are not supported by them.

If the orchestrator does not cache the available methods, it must query each PSP for every incoming options request. While this ensures real-time accuracy, it results in significant operational overhead, particularly when a merchant is integrated with multiple PSPs. Since payment methods typically change infrequently, this approach can be inefficient.

A more scalable approach is to cache the available payment methods and return the cached data when responding to the merchant. To keep the cache synchronized with the PSPs, here are a few common strategies.

### Webhook-Based Sync
PSPs send a webhook notification to the orchestrator when their payment method configuration changes.

**Pros:**
* Real-time updates
* Minimal network traffic
* Scales effectively with multiple PSPs


**Cons:**
* Relies on PSPs supporting webhooks
* Requires reliable error handling and retry mechanisms
* Necessitates secure verification and authentication

### Periodic Polling
The orchestrator periodically queries each PSP (e.g., hourly) to refresh the available payment method configurations.

**Pros:**
* Easy to implement
* No reliance on webhook infrastructure

**Cons:**
* Data may become outdated between polling intervals
* Increased load if polling frequency is high
* Less efficient with a large number of PSPs

### Manual Trigger
Synchronization is initiated manually, either by the merchant or through internal tools.

**Pros:**
* Low maintenance and operational cost
* Suitable for configurations that change infrequently

**Cons:**
* Risk of stale or outdated data
* Dependent on human intervention
* Not ideal for dynamic or rapidly changing environments

### Sync on First Access / Lazy Loading
The orchestrator synchronizes payment methods on demand—only when a merchant or PSP is accessed and the cache is missing or expired.

**Pros:**
* Minimizes unnecessary synchronization
* Efficient for merchants with low traffic
* Works well with time-to-live (TTL) based caching strategies

**Cons:**
* Slower response for the first request (cold start)
* Requires careful management of cache expiration and invalidation
* Potential for inconsistent experiences if merchants receive updates at different times


## Token Vault
Tokens can be stored on either the orchestrator's side, the PSP's side, or in some cases, a combination of both.

The main benefit of storing tokens within the orchestrator is enhanced interoperability. For example, in a Card-on-File (COF) scenario, if card details are kept with the orchestrator, it can direct the transaction to the most suitable PSP (assuming that PSP supports the card brand). If the first attempt fails, the orchestrator has the option to retry with a different PSP, which can enhance overall reliability and success rates.

However, this advantage comes with limitations. Its effectiveness depends heavily on the broader payment ecosystem’s support for interoperability. In practice, such flexibility is generally supported by major card schemes, while alternative and bank-based payment methods offer limited or no support.

Most importantly, since PSPs interact directly with banks or wallets, the tokens they create are typically only usable through that specific PSP. Furthermore, many PSPs do not disclose or share the actual token generated at the scheme, issuer, or wallet level, which further restricts the orchestrator's ability to operate independently.

