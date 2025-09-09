---
draft: true
title: How to be Guardrails
date: 2023-06-21T22:15:33+0800
categories:
- Career
tags: 
- Career
- Guardrails
- Coaching
- Staff Engineer
- The Staff Engineer's Path
---

## Guardrails

Think of the railing you might find along a cliffside walking path. They are not for leaning on, but they're there
to steady yourself when you need them. A small stumble will not fail you: the rail will stop you from going
over the edge. Guardrails encourage autonomy, exploration, and innovation.

If you want to be a good guardrail, don't ever rubber-stamp changes. Read carefully: every line of code, every section
of a design, every step of a proposed change. Here are some categories of problems you should look for.

#### Should this work exist?

What problem does your colleague intend to solve? Are they using a technical solution to solve a problem should be
solved by talking to someone?

#### Does this work actually solve the problem?

Will the solution work? Will users be able to do what they need and what they expect? Are there errors or typos?
Any bugs or performance issues? Does the design propose using a system in a way that won't work?

#### How will it handle failure?

How will the solution handle weird edge cases, malformed input, the network randomly disappearing, load spikes,
or whatever else can go wrong? Will it fail in a clean way, or will it corrupt data or take a user's money without
giving them the service they've paid for? How will you discover problems?

#### Is it understandable?

Will other people be able to maintain and debug new code or systems? Are the components or variables named intuitively?
Is the complexity contained in a well-chosen place?

#### Does it fit into the bigger picture?

Does the change set a precedent or create a new pattern you might now want other people to copy? Does it force other
teams to do extra work for future changes? Is this a risky change that's scheduled at the same time as a high-profile
launch?

#### Do the right people know about it?

Is everyone copied on the change who should be? Are there names attached to any actions that need to happen, or is
there a lot of passive voice where it isn't clear which team is doing what? Do the people involved know what is
expected of them?


