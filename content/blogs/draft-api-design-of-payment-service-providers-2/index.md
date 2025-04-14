---
date: 2025-03-20T08:36:34+08:00
draft: true
title: API Design of Payment Service Providers - Error Handling
categories: 
  - API Design of Payment Service Providers
tags: 
    - API
    - PSP
    - Payment
---

## Types of Error

While not all languages differentiate between **checked** and **unchecked** exceptions, we believe it's generally a good practice to distinguish at least these two categories of errors—recoverable and unrecoverable—and handle them accordingly.

### Unchecked Errors
In a well-defined domain, checked exceptions can help enforce clear API contracts between internal services. They are especially useful for handling scenarios such as:

* **Validation Errors** — to stop execution early and provide clear feedback (e.g., invalid input format, missing required fields)
* **Business Rule Violations** — to represent known, rule-based failures (e.g., "Insufficient balance", "Order already fulfilled")
* **Recoverable External Failures** — to signal retry or fallback logic (e.g., a third-party API returns a known, handleable error)
* **Missing Resources** — to trigger 404-style behavior or retries (e.g., resource not found in the database)
* **Expected Domain Errors** — to allow consumers to respond to specific business scenarios (e.g., attempting to cancel an already shipped order)

However, using checked exceptions comes with trade-offs:
* **Boilerplate Code** — introduces extra try/catch or throws declarations, even when the logic doesn't need it
* **Exception Overload** — overuse can clutter code and dilute the meaning of exceptions
* **Tight Coupling** — may expose internal implementation details if exceptions propagate across service boundaries
* **Limited Compatibility with Async/Reactive Patterns** — doesn't work well with lambdas, streams, or reactive pipelines
* **Superficial Handling** — developers may end up catching and re-throwing without meaningful error management

### Checked Errors

Unchecked exceptions are not enforced by the compiler and are widely used in modern architecture because of their flexibility and reduced boilerplate. They're typically used for:
* Unexpected System Failures, e.g., out-of-memory, DB connection lost. These are unrecoverable. They should crash fast and be logged, not caught and handled locally.
* Programming Errors, e.g., Illegal arguments, unsupported operations, assertion failures. These usually indicate bugs or misuse of the system.
*  Framework or Library Exceptions, e.g., Spring’s `HttpMessageNotReadableException`, Hibernate’s `LazyInitializationException`. These are implementation-level issues typically not part of the service’s domain logic.
* Cross-cutting concerns, e.g, Unauthorized, AccessDenied. These are often handled by global exception handlers or middleware, not the application logic itself.

 In the meanwhile, the disadvantage are quite obvious as well:
* Less Explicit Contracts	Errors are not visible in method signatures, making them harder to anticipate
* Overuse Can Hide Intent	Developers may throw unchecked exceptions for everything, including recoverable errors
* *Harder to Document	Without annotations or external docs, consumers may not know what can go wrong
* Risk of Runtime Surprises	Developers may forget to handle critical exceptions, leading to production issues


Unchecked exceptions are not mandated by the compiler, making them a popular choice in modern architectures due to their flexibility and reduced boilerplate. They are commonly used in the following scenarios:

* **Unexpected System Failures** (e.g., out-of-memory errors, lost database connections): These are unrecoverable issues that should trigger a fast failure with proper logging, rather than being caught and handled locally.
* **Programming Errors** (e.g., illegal arguments, unsupported operations, assertion failures): These typically indicate bugs or incorrect usage of the system.
* **Framework/Library Exceptions** (e.g., Spring’s `HttpMessageNotReadableException`, Hibernate’s `LazyInitializationException`): These often stem from implementation details rather than domain logic.
* **Cross-Cutting Concerns** (e.g., unauthorized access, permission denials): These are usually managed by global exception handlers or middleware rather than application-specific logic.

However, unchecked exceptions also come with notable drawbacks:

* **Less Explicit Contracts**: Since errors aren’t declared in method signatures, they can be harder to anticipate.
* **Overuse Can Obscure Intent**: Developers might misuse unchecked exceptions for recoverable errors, blurring the line between expected and unexpected failures.
* **Poor Documentation Challenges**: Without annotations or external documentation, it’s difficult for API consumers to know what can go wrong.
* **Runtime Risks**: Critical exceptions may go unhandled, leading to unexpected crashes in production.



## How to respond error to API user
Regardless whether the error is unchecked or unchecked, there is no case where they should be returned the end user.
We should always returned a desired, well-designed and well-documented response to user.

There are two common approaches to returning error information to API consumers:
* Embedding an error object within the root-level response
* Using a separate response schema specifically for errors

### Nested Error Object in the Root Response

In this approach, the response always follows a consistent schema, even for errors. The success or failure is indicated via a status or error field within the root object.

#### PROs
* Uniform structure for all responses
* Easier to parse on the client side (same structure always)
* Can include both data and error if needed (e.g., partial success)

#### CONs
* May blur the line between success and failure (especially with 200 OK + error inside body)
* Slightly less idiomatic with HTTP status codes if not used carefully


For example, 
```JSON
{
  "status": "failure",
  "error": {
    "code": "INVALID_INPUT",
    "message": "The 'email' field is invalid."
  }
}
```

### Different Schema for Error Responses
Success and error responses follow completely different schemas, and usually HTTP status codes are relied on more strictly (e.g., 4xx/5xx for errors).

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
There are no strictly right or wrong decision on this. It should be always based on the use case. You can even think about combining the benefits of both, e.g., use HTTP semantics for option 1 as well, though it may lead to confusion if developers only rely on the success flag and ignore HTTP status.

There’s no absolute right or wrong choice between the two approaches—it ultimately depends on the specific use case. In some cases, it may even make sense to combine elements of both, such as using HTTP status codes alongside a consistent response structure (Option 1). However, this hybrid approach can introduce ambiguity if developers rely solely on fields like `success` and overlook the HTTP status code.