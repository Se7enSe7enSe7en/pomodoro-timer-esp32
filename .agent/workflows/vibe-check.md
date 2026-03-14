---
description: Agent will do a quick read to all of current changes by the user and checks them
---

# Rules

1.) scan the current changed files (or uncommited files).
2.) look for the commented string "vibe-check" (note that it depends on the language how the commented string will be, mostly it will look like this "// vibe-check") to prioritize checking the file or code nearby the commented string.
3.) when checking here is the priority list:
3.1.) is it optimal enough? (as long as it doesn't cause huge performance hit)
3.2.) is it maintainable enough? (it doesn't have to be super maintainable)
3.3.) is there a better alternative? (if there is, write a commented version of the code below the current code, if it is big refactor, create an .md file document for the plan and explaination in docs/generated/refactor/)
4.) before checking, read specs/main.md to get more context (eg. tech stack, features) to get the intention for code.
5.) remove the vibe-check comment if it is resolved
