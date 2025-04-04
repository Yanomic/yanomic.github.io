---
title: Ideas for Code Review and Design Review
date: 2023-06-21T22:15:33+0800
categories:
- Career
tags: 
- Career
- Code Review
- Design Review
- Staff Engineer
- Coaching
- The Staff Engineer's Path
---

## Ideas for Code Review and Design Review

Reviewing code and designs can be an excellent form of teaching. You get to highlight perils your colleague might
not know about and suggest safer alternatives. You also get to encourage behaviors you want to see more of. Here are
some ideas to bear in mind as you review to teach.

#### Understand the Assignment

**Be aware of the context**. Is your colleague new to the language or technology and looking to learn, or do they just
need a second pair of eyes for safety? **Understand the stage of work**, too: if you're reading a first high-level
draft, start with the foundations and the approach, and don't get into the nitpicky details. If everyone has bought
in and this is the last review before launch, it is not the time for big directional questions: get right into the weeds
and be extra alert for what could go wrong.

#### Explain Why as well as What

A review comment like "Don't use *share_ptr*, use *unique_ptr*" only tells the code author what to do right now.
They won't know what to do next time. Teaching means sharing understanding, not just facts. While the code author
can go read the documentation on whatever you just told them about, they might not recognize why it applies. A short
explanation or a link to a relevant article or specific StackOverflow post(rather than a general manual) will be a
shortcut to help them learn.

#### Give an Example of What Would be Better

If a section of the design is confusing, don't just say "Please make this more clear." It's hard to know what to do with
that! Offer of a couple of suggestions of what you think the author is trying to say.

#### Be Clear about What Matters

When you're less experienced, it can be hard to calibrate the advice you're given. Some things are vitally important,
some are nice to have, and some are personal preferences. Annotate your advice so it's clear.

#### Choose Your Battles

John Turner, a software engineer who has written about code review for the Squarespace engineering blog, recommends
reviewing code in several passes: first high-level comments, then in increasing detail. As he points out: "if the code
does not do what it's supposed to, then it doesn't matter if the indentation is correct or not." This advice works for
RFCs(Request For Comment) toos: if your first comment is that the author is solving the wrong problem, it's not helpful
to leave a hundred technical suggestions.

#### If You Mean "Yes", Say "Yes"

Make it clear whether you consider your comments to be blocking or not, and whether you're otherwise happy with the
change. Call out the good as well as the bad. In particular, explicitly say "This looks good to me" on design documents.
Code review tends to end by clicking a button to say that you believe the change is safe to merge. When there are a lot
of reviewers, though, each one may be hesitant to approve until the others have weighed in. If you have no objections,
say so.
