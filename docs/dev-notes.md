# Dev Notes

## RenderGL review

Goal:
Check how tightly RenderGL is coupled to OpenGL and how hard it is to decouple later.

Questions:
- Are GL calls centralized or spread across code?
- Do renderers expose GL types?
- Can we introduce a thin render command layer later?

Observations:
- NodeRenderer / EdgeRenderer contain direct GL calls
- Camera uses glm (PUBLIC dependency)
- No abstraction layer yet

Conclusion (WIP):
- GL is currently deeply integrated
- Decoupling possible if render commands are introduced
- No action needed now

Next:
- Keep GL usage localized
- Avoid spreading GL calls outside RenderGL

## TODO

- [ ] Review RenderGL boundaries
- [ ] Decide on render command abstraction (later)
- [ ] Add .clang-format
- [ ] Setup auto-format on commit