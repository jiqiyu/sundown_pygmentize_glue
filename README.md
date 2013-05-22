sundown_pygmentize_glue
=======================

`sundown_pygmentize_glue` is a command line tool that translates Markdown to HTML with syntax highlight.

## How it works ##
1. Extract codes surrounded by "\`\`\`"(block code) or "\`"(inline code).
2. [Sundown(Markdown to html)](https://github.com/vmg/sundown) the full document not containing codes.
3. [Pygmentize(syntax highlighting)](http://pygments.org) the extracted codes.
4. Glue all outputs.

## Usage(Windows XP) ##
1. Download the precompiled package and extract to any place then cd to that place and `make clean && make`.
2. `cat README.md | ./sundown_pygmentize_glue > README.html`

## Usage(linux) ##
I have not test it on linux, but i think it will works as the same as on windows. You should compile Sundown and install Pygmentize.
