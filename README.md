xxs: a vector language for small systems
===

These are just some ideas. Nothing works. Will never be finished.

Motivation
---

I like the ideas I explored in XXL, but the implementation is pretty slow and
buggy.

Now that I have a need to work on an ESP32 from Espressif, I want a way to
develop interactively, using a feature set similar to XXL, K/Q, Kerf, etc. None
of those systems target this platform.

I am willing to sacrifice some power for good performance and small size. Even
if it means weird trade-offs. 

Style
---

Mix of infix and postfix, as XXL. Simpler types (no symbols), hopefully much simpler implementation and better performance.

Goal
---

Be able to telnet to a just-flashed ESP32 and write simple apps. Don't malloc() so damn much.

Status
---

Just collecting ideas at this point. It's possible nothing will come out of it.

Requirements
---

C compiler. 

Eventually it will require ESP-IDF, but we're nowhere near that.

Compile
---

`make`




