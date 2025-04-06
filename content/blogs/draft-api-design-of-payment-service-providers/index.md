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

{{% callout note %}}
<div class="mt-1 font-semibold text-lg">Brainstorm</div>

[Figma Diagram](https://www.figma.com/board/qWPsv48tqBdkTJmdshjFRG/Comparison-of-PSP-APIs?t=blAqwNyslrDv2Kjn-0)
{{% /callout %}}

This actually triggers our thinking on how would an ideal PSP API look like.
The ideal API should be well-structured, consistent, extensibile, backward/forward compatible and free of problems mentioned above.


### Headers
Headers are key-value pairs that provide metadata about the request or response. 
In terms of HTTP request to PSP, the fields should be used to handle following information:
* **Authentication**, e.g.,  `Authorization: Bearer {token}`, `X-API-KEY: {api-key} `
* **Content Negotiation**, e.g., `Content-Type: application/json`, `Accept: application/x-www-form-urlencoded`
* **Idempotency**, e.g., `X-Idempotency-Key: {idempotency-key}`
* **Merchant/Partner Identifier**, e.g., `X-Merchant-Code: {merchant-code}`, `X-Partner-ID: {partner-id}`
* **Payload Integrity Validation**, e.g.,  `X-Signature: sha256={signature}`
* **Rate Limiting**, e.g., `X-RateLimit-Limit: {limit}`, `X-RateLimit-Remaining: {remaining}`

{{% callout note %}}
<div class="mt-1 font-semibold text-lg">Which should not be included</div>
 
* **Client information**, e.g., `User-Agent: Mozilla/5.0`

  The request is usually sent from merchant server to PSP, where the client in such request is the merchant server. 
  The client info is not so useful for PSP to process and coudl easily be confused with the device that shopper use to initiate the transaction.

  We prefer to include Client Info as part of the [Device](#device) as we mention below.

* **Caching**, `Cache-Control: no-cache`
  The purpose of the request to take action, e.g., making a payment, or query the latest state.
  In neither case, caching makes any sense, e.g., the value will always be `no-cache`, which we suggest to just not send at all.
{{% /callout %}}

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

Each category may also contain its own logical subgroups. To maintain clarity and prevent excessive nesting, we recommend that leaf nodes should not exceed **five** levels of depth from the root.


#### Root
Fields that are directly related to the transaction or required across all transaction types should be placed at the root level of the request.

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
    - **addressMatch** |`bool`: Whether the chosen delivery address is identical to the billing address.
    - **challengeWindowSize**, The size of the challenge window displayed to the shopper in the challenge flow.
    - **dataOnly**, Indicates whether to perform data only flow.
    - **deliveryAddressIndicator** |`string`: Indicator regarding the delivery address.
    - **scaExemption**, Indicates the exemption type that you want to request for the particular transaction. 
    - **version**, Indicates your preference for the 3D Secure version.
  - **MpiData**
    - **authenticationResponse**, In 3D Secure 2, this is the transStatus from the challenge result. If the transaction was frictionless, omit this parameter.
    - **cavv**, The cardholder authentication value (base64 encoded, 20 bytes in a decoded form).
```

#### Configuration
Fields related to configuration of the payment behaviour.

```markmap{height="200px"}
- Configuration
  - Routing
    - **routingFlag** |`string`: Allows you to determine the acquirer account that should be used for the transaction.
  - capture
    - **mode** |`string`: Allows you to determine the way the capture should happen.
    - **delayInHours** |`integer`: The delay between the authorisation and scheduled auto-capture, specified in hours.
```

#### Device
Fields related to device with which the transaction is made.

```markmap{height="300px"}
- Device
  - **channel** |`string`: The platform where a payment transaction takes place.
  - **deviceFingerprint** |`string`: A string containing the shopper's device fingerprint.
  - **deviceRefence** |`string`: The unique reference for device that is processing the payment.
  - **ipAddress** |`string`: The IP address of the device.
  - **locale** |`string`: Locale on the device.
  - **origin** |`string`: The origin URL of the page that you are loading the payment or 3D Secure Component from.
  - **os** |`string`: Operating system running on the device.
  - **osVersion** |`string`: Version of operating system running on the device.
  - BrowserInfo
    - **acceptHeader** |`string`: The accept header value of the shopper's browser.
    - **colorDepth** |`integer`: The `screen.colorDepth` property which indicates the color depth of the shopper's browser in bits per pixel.
    - **javaEnabled** |`bool`: Boolean value indicating if the shopper's browser is able to execute Java.
    - **javaScriptEnabled** |`bool`: Boolean value indicating if the shopper's browser is able to execute JavaScript.
    - **language** |`string`: The `navigator.language` value of the shopper's browser (as defined in IETF BCP 47).
    - **screenHeight** |`integer`: The total height of the shopper's device screen in pixels.
    - **screenWidth** |`integer`: The total width of the shopper's device screen in pixels.
    - **timeZoneOffset** |`integer`: Time difference between UTC time and the shopper's browser local time, in minutes.
    - **userAgent** |`string`: The user agent value of the shopper's browser.
```

#### Fund
Fields related to fund used in the transaction.
```markmap{height="100px"}
- Fund
  - **origin** |`string`: The person or entity funding the money.
  - **recipient** |`string`: The person or entity receiving the money
```

#### Forex
Fields related multi-currency payment.
```markmap{height="200px"}
- **baseAmount** |`amount`: The base amount.
- **basePoint** |`integer`:The base amount.
- **buyRate** |`amount`: The buy rate.
- **interbank** |`amount`: The interbank amount.
- **sellRate** |`amount`: The sell rate.
- **source** |`string`: The source of the forex quote.
- **type** |`string`: The type of forex.
- **validTill** |`date|YYYY-MM-DDThh:mm:ssZ`: The date until which the forex quote is valid.
```

#### Installments
Fields related to installment plan details.
```markmap{height="100px"}
- Installments
  - **extra** |`integer`: Defines the bonus percentage, refund percentage or if the transaction is Buy now Pay later.
  - **plan** |`string`: The installment plan.
  - **numberOfInstallments** |`integer`: Defines the number of installments. Usually, the maximum allowed number of installments is capped.
```

#### Mandate
Fields related to mandate details.
```markmap{height="200px"}
- Mandate
  - **amount** |`amount`: The billing amount (in minor units) of the recurring transactions.
  - **amountRule** |`string`: The limitation rule of the billing amount.
  - **attemptsRule** |`string`: The rule to specify the period, within which the recurring debit can happen, relative to the mandate recurring date.
  - **billingDay** |`integer`: The number of the day, on which the recurring debit can happen. 
  - **count** |`string`: The number of transactions that can be performed within the given frequency.
  - **endsAt** `date|YYYY-MM-DD`: Expiry of the mandate.
  - **frequency** |`string`: The frequency with which a shopper should be charged.
  - **remarks** |`string`: The message shown to the shopper on the approval screen.
  - **startsAt** `date|YYYY-MM-DD`: Start date of the mandate.
```

#### Merchant/Sub-Merchant
Fields related to the entity to which the transaction belongs.
```markmap{height="200px"}
- Merchant/Sub-Merchant
  - **mcc** `string`: The merchant category code, which relates to a particular market segment.
  - SubMerchant
    - **address** `string`: Sub-merchant's address.
    - **taxId** `string`: The tax ID of the sub-merchant.
    - **id** `string`: The unique ID of sub-merchant.
    - **name** `string`: The name of sub-merchant.
```

#### Metadata
Stores additional, non-functional information related to the payment.

#### Order
Fields related to order details.
```markmap{height="1000px"}
- Order
  - **billingAddress** |`address`: The address where to send the invoice.
  - **deliverAt** |`date|YYYY-MM-DD`: The date and time the purchased goods should be delivered.
  - **deliveryAddress** |`string`: The address where the purchased goods should be delivered.
  - **deliveryEmailAddress** |`string`: Email associated with the given product for electronic delivery.
  - **deliveryTimeframe** |`string`: The estimated delivery time for the shopper to receive the goods. 
  - Items[]
    - **amountExcludingTax** |`amount`: Item amount excluding the tax, in minor units.
    - **amountIncludingTax** |`amount`: Item amount including the tax, in minor units.
    - **brand** |`string`: Brand of the item.
    - **category** |`string`: Item category.
    - **color** |`string`: Color of the item.
    - **description** |`string`: Description of the line item.
    - **id** |`string`: ID of the line item.
    - **imageUrl** |`string`: Link to the picture of the purchased item.
    - **manufacturer** |`string`: Manufacturer of the item.
    - **productUrl** |`string`: Link to the purchased item.
    - **quantity** |`string`: Number of items.
    - **size** |`string`: Size of the item.
    - **sku** |`string`: Stock keeping unit.
    - **tax** |`amount`: Tax amount.
    - **taxRate** |`integer`: Tax percentage, in minor units.
    - **upc** |`integer`: Universal Product Code.
  - Airline
    - **agencyCode** |`string`: IATA code of the travel agency.
    - **agencyName** |`string`: Name of the travel agency.
    - **agencyInvoiceNumber** |`string`: Invoice number issued by the travel agency.
    - **agencyPlanName** |`string`: Name of the agency's fare plan (e.g., corporate travel plan).
    - **airlineCode** |`string`: IATA 2-letter airline code.
    - **airlineName** |`string`: Full name of the Airline.
    - Legs[]
      - **arrivalAirport** |`string`: IATA 3-letter arrival airport code.	
      - **class** |`string`: Travel class (Economy, Premium, Business, First).
      - **carrierCode** |`string`: Carrier code (if different from airline code).
      - **departureAirport** |`string`: IATA 3-letter departure airport code.
      - **fareBasis** |`string`: Fare class code (e.g., economy, business).
      - **flightDate** |`date|YYYY-MM-DDThh:mmZ`: Departure date/time (ISO 8601).
      - **flightNumber** |`string`: Departure date.
      - **stopoverCode** |`string`: Departure date.
    - Passengers[]
      - **dateOfBirth** |`date|YYYY-MM-DD`: Passenger’s birth date.
      - **firstName** |`string`: Passenger’s first name.
      - **frequentFlyerNumber** |`string`: Loyalty program number.
      - **lastName** |`string`: Passenger’s last name.
      - **passengerType** |`string`: The IATA passenger type code (PTC).
      - **ticketNumber** |`string`: Unique ticket number (13 digits).
  - CarRental
    - **agreementNumber** |`string`: The rental agreement number.
    - **pickUpDate** |`date|YYYY-MM-DD`: The pick-up date.
    - **pickUpLocation** |`address`: The pick-up location.
    - **dropOffDate** |`date|YYYY-MM-DD`: The drop-off date.
    - **dropOffLocation** |`address`: The drop-off location.
    - **renterName** |`string`: The drop-off location.
    - **rentalClassIndicator** |`string`: Indicates the classification of the rental car.
    - **fuelCharges** |`amount`: Fuel charges associated with the rental.
    - **insuranceCharges** |`amount`: Fuel charges associated with the rental.
    - **rate** |`amount`: The rental rate.
    - **rateBase** |`string`: Base Rental Rate(daily/weekly).
  - Lodging
    - **bookingNumber** |`string`: The unique id for the booking.
    - **checkInDate** |`date|YYYY-MM-DD`: The check-in date.
    - **checkOutDate** |`date|YYYY-MM-DD`: The check-out date.
    - **Rooms[]**
      - **numberOfGuests** `integer`: The total number of guests the room is booked for.
      - **numberOfNight** `integer`: The total number of nights the room is booked for.
      - **rate** `amount`: The rate for the room.
      - **type** `string`: The room type.      
```

#### OpenInvoice
Fields related to OpenInvoice details.


#### Payment Method
Fields related to the payment method used in the transaction.

#### Risk
Fields related to risk data of the transaction.
```markmap{height="100px"}
- Risk
  - **fraudOffset** |`integer`: An integer value that is added to the normal fraud score. The value can be either positive or negative.
````


#### Tokenization
Fields related to tokenize the payment methods for future use.
```markmap{height="100px"}
- Tokenization
  - **enableOneClick** |`bool`: Indicates whether to store the payment details for future one-click payments. 
  - **enableRecurring** |`bool`: Indicates whether to store the payment details for future recurring payments.
```

#### Shopper
Fields related to the shopper.
```markmap{height="200px"}
- Shopper
  - **countryCode** |`string`: The shopper country code in ISO 3166-1 alpha-2 format.
  - **dateOfBirth** |`date|YYYY-MM-DD`: The shopper's date of birth.
  - **email** |`string`: The shopper's email address.
  - **locale** |`string`: The locale of the shopper.
  - **name** |`name`: The shopper's full name.
  - **reference** |`string`: The reference to uniquely identify this shopper
  - **socialSecurityNumber** |`string`: The shopper's social security number.
  - **telephoneNumber** |`string`: The shopper's telephone number.
```

#### Splits
Fields instrut to split a payment.
```markmap{height="200px"}
- Splits[]
  - **account** |`string`: The unique identifier of the account to which the split amount is booked.
  - **amount** |`amount`: The amount of the split item.
  - **description** |`string`: Your description for the split item.
  - **reference** |`string`: The unique reference for the part of the payment booked to the specified account.
  - **type** |`string`: The type of the payment you want to book to the specified account.
```


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


## Appendix A: Common Data Types

### Address
```markmap{height="150px"}
- Address
  - **country** |`string`: Country.
  - **state** |`string`: State/Province/Region.
  - **city** |`string`: City.
  - **street** |`string`: Street Address
  - **building** |`string`: Postal/ZIP Code.
  - **postalCode** |`string`: Postal/ZIP Code.
```

### Amount
```markmap{height="40px"}
- Amount
  - **currency** |`string`: The three-character ISO-4217 currency code.
  - **value** |`integer`: The amount of the transaction, in minor units.
```

### Name
```markmap{height="40px"}
- Name
  - **firstName** |`string`: The first name.
  - **lastName** |`string`: The last name.
```