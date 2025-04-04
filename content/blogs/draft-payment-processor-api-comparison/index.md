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