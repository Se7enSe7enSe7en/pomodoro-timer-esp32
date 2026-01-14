---
description: Create short documentation for the topic
---

1. check first if a docs/generated-docs/ directory exists, if not then create it
2. check if a the topic already exists, if not create a short document about the topic
3. the topic is the string parameter after the workflow directive (eg. @doc-this "topic"). if there is none (eg. @doc-this) the topic is whatever the last topic of the converstion.
4. the contents of the document should contain (in order):
   4.a a short summary of the concept using simple understandable language
   4.b a "black box" section for the concept if possible (input and output), else if do a Pros and Cons section instead, else skip this section
   4.c a concrete example for the concept
   4.d "What is it?" section, slightly longer summary of what it is
   4.e "Why is it important?", the usual purpose of the topic
   4.f References section, links to the sources of the information.
