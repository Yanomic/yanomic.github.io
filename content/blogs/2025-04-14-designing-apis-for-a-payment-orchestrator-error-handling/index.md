---
date: 2025-04-14
title: Designing APIs for a Payment Orchestrator - Error Handling
categories: 
  - Designing APIs for a Payment Orchestrator
tags: 
    - API
    - PSP
    - Payment
---

## Types of Error

While not all languages differentiate between **checked** and **unchecked** exceptions, we believe it's generally a good practice to distinguish at least these two categories of errors—recoverable and unrecoverable—and handle them accordingly.

### Checked Errors
In a well-defined domain, checked exceptions can help enforce clear API contracts between internal services. They are especially useful for handling scenarios such as:

* **Validation Errors** — to stop execution early and provide clear feedback (e.g., invalid input format, missing required fields)
* **Business Rule Violations** — to represent known, rule-based failures (e.g., "Insufficient balance", "Order already fulfilled")
* **Recoverable External Failures** — to signal retry or fallback logic (e.g., a third-party API returns a known, handleable error)
* **Missing Resources** — to trigger 404-style behavior or retries (e.g., resource not found in the database)
* **Expected Domain Errors** — to allow consumers to respond to specific business scenarios (e.g., attempting to cancel an already shipped order)

However, using checked exceptions comes with trade-offs:
* **Boilerplate Code** — introduces extra `try`/`catch` declarations, even when the logic doesn't need it
* **Exception Overload** — overuse can clutter code and dilute the meaning of exceptions
* **Tight Coupling** — may expose internal implementation details if exceptions propagate across service boundaries
* **Limited Compatibility with Async/Reactive Patterns** — doesn't work well with lambdas, streams, or reactive pipelines
* **Superficial Handling** — developers may end up catching and re-throwing without meaningful error management

### Unchecked Errors

Unchecked exceptions are not enforced by the compiler and are widely used in modern architecture because of their flexibility and reduced boilerplate. They're typically used for:

* **Unexpected System Failures** (e.g., out-of-memory errors, lost database connections): These are unrecoverable issues that should trigger a fast failure with proper logging, rather than being caught and handled locally.
* **Programming Errors** (e.g., illegal arguments, unsupported operations, assertion failures): These typically indicate bugs or incorrect usage of the system.
* **Framework/Library Exceptions** (e.g., Spring’s `HttpMessageNotReadableException`, Hibernate’s `LazyInitializationException`): These often stem from implementation details rather than domain logic.
* **Cross-Cutting Concerns** (e.g., unauthorized access, permission denials): These are usually managed by global exception handlers or middleware rather than application-specific logic.

However, unchecked exceptions also come with notable drawbacks:

* **Less Explicit Contracts**: Since errors aren’t declared in method signatures, they can be harder to anticipate.
* **Overuse Can Obscure Intent**: Developers might misuse unchecked exceptions for recoverable errors, blurring the line between expected and unexpected failures.
* **Poor Documentation Challenges**: Without annotations or external documentation, it’s difficult for API consumers to know what can go wrong.
* **Runtime Risks**: Critical exceptions may go unhandled, leading to unexpected crashes in production.



## How to Communicate Errors to API Users
Whether an error is checked or unchecked, it should never be exposed directly to the end user. 
Instead, APIs should always return a well-structured, intentional, and properly documented error response.

There are two common approaches to returning error information to API consumers:
* Embedding an error object within the root-level response
* Using a distinct schema dedicated specifically to error responses

### Embedding an error object

In this approach, the response always follows a consistent schema, even for errors. The success or failure is indicated via a status or error field within the root object.

#### PROs
* Uniform structure for all responses
* Easier to parse on the client side (same structure always)
* Can include both data and error if needed (e.g., partial success)

#### CONs
* May blur the line between success and failure (especially with 200 OK + error inside body)
* Slightly less idiomatic with HTTP status codes if not used carefully

```JSON
{
  "status": "failure",
  "error": {
    "code": "INVALID_INPUT",
    "message": "The 'email' field is invalid."
  }
}
```

### Using a distinct schema
Success and error responses follow completely different schemas, and usually HTTP status codes are relied on more strictly.

#### PROs
* Clear separation between success and error
* Fully leverages HTTP semantics (e.g., 400 Bad Request)
* Can tailor the error schema specifically to error handling

#### CONs
* Requires clients to handle multiple response shapes
* Parsing logic is slightly more complex


For example, in case of success(HTTP 200):
```JSON
{
  "userId": "1234",
  "name": "Alice",
  "email": "alice@email.com"
}
```
and, when in error(HTTP 400):
```JSON
{
  "errorCode": "INVALID_INPUT",
  "message": "The 'email' field is invalid."
}
```

### Decision
There’s no definitive right or wrong choice between the two approaches—what works best depends on the specific use case. In some scenarios, combining elements of both can be beneficial—for example, applying HTTP semantics while using a consistent response structure . 

However, this hybrid model can lead to confusion if developers focus only on indicators like a success flag and ignore the actual HTTP status code.


## Reference
* [RFC 9457 Problem Details for HTTP APIs](https://tools.ietf.org/html/rfc9457)