sundown_pygmentize_glue
=======================

`sundown_pygmentize_glue` is a command line tool that translates Markdown to HTML with syntax highlight.

## How it works ##
1. Extract codes surrounded by "\`\`\`"(block code) or "\`"(inline code).
2. [Sundown(Markdown to html)](https://github.com/vmg/sundown) the full document not containing codes.
3. [Pygmentize(syntax highlighting)](http://pygments.org) the extracted codes.
4. Glue all outputs.

## Usage(Windows XP) ##
```bat
    REM Install python and set Environment Variable; set PATH=C:\Python27;%PATH%
    REM Downlaod this project; https://github.com/huxingyi/sundown_pygmentize_glue/archive/master.zip
    REM Change directory to this project; cd sundown_pygmentize_glue-master
    type README.md | sundown_pygmentize_glue.exe > README.html
    REM If you have installed cygwin, please use: cat README.md | ./sundown_pygmentize_glue > README.html
```

## Usage(linux) ##
I have not test it on linux, but i think it will works as the same as on windows. You should compile Sundown and install Pygments.

## How did i compile sundown ##
 change sundown/examples/sundown.c to enable all mkd_extensions:
```c
    markdown = sd_markdown_new(0xff, 16, &callbacks, &options);
```

## Charset ##
Only supports "utf-8". utf-8 in utf-8 out. Other charset in, error out.
