sundown_pygmentize_glue
=======================

`sundown_pygmentize_glue` is a command line tool that translates Markdown to HTML with syntax highlight.

# How it works #
1. Extract codes surrounded by "\`\`\`"(block code) or "\`"(inline code).
2. [Sundown(Markdown to html)](https://github.com/vmg/sundown) the full document not containing codes.
3. [Pygmentize(syntax highlighting)](http://pygments.org) the extracted codes.
4. Glue all outputs.

# Usage(Windows XP) #
```bat
    REM Install python and set Environment Variable; set PATH=C:\Python27;%PATH%
    REM Downlaod this project; https://github.com/huxingyi/sundown_pygmentize_glue/archive/master.zip
    REM Change directory to this project; cd sundown_pygmentize_glue-master
    type README.md | sundown_pygmentize_glue.exe > README.html
    REM If you have installed cygwin, please use: cat README.md | ./sundown_pygmentize_glue > README.html
```

## Usage(linux) ##
I have not test it on linux, but i think it will works as the same as on windows. You should compile Sundown and install Pygments.

## Usage(emacs) ##
```lisp
;; markdown2html
(defun m2h (filename)
  "Convert markdown to html. Usage: M-x m2h <RET>
The tool 'sundown_pygmentize_glue' is needed, which can be downloaded at
https://github.com/huxingyi/sundown_pygmentize_glue
"
  (interactive
   "sFilename, enter the absolute path to file(default is current buffer): ")
  (setq f filename)
  (setq cmd0 "cd d:/files/bin/sundown_pygmentize_glue-master && cat ")
  (setq cmd_t " | sundown_pygmentize_glue.exe > ")
  (if (string-equal "" f)
      (setq f (buffer-file-name)))
  (string-match "\.[a-zA-Z0-9]+$" f)
  (setq dest (replace-match "_\.html" nil nil f))
  (setq cmd_tt (concat " && start " dest))
  (if (file-exists-p f)
      (shell-command (concat cmd0 f cmd_t dest cmd_tt))
    (message "Error: file does not exist.")))
```

## How did i compile sundown ##
 change sundown/examples/sundown.c to enable all mkd_extensions:
```c
    markdown = sd_markdown_new(0xff, 16, &callbacks, &options);
```

## Charset ##
Only supports "utf-8". utf-8 in utf-8 out. 
You could replace all "utf-8" in `pygmentize.bat` and `src/sundown_pygmentize_glue.c` to support other charsets.
