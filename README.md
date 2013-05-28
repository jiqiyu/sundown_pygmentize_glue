WHAT IS SUNDOWN_PYGMENTIZE_GLUE?  
sundown_pygmentize_glue is a command line tool that translates Markdown to HTML with syntax highlight.

HOW IT WORKS?  
1. Extract codes surrounded by "\`\`\`"(block code) or "\`"(inline code).  
2. [Sundown(Markdown to html)](https://github.com/vmg/sundown) the full document not containing codes.  
3. [Pygmentize(syntax highlighting)](http://pygments.org) the extracted codes.  
4. Glue all outputs.

USAGE(WINDOWS XP):  
```bat
    REM Install python and set Environment Variable; set PATH=C:\Python27;%PATH%
    REM Downlaod this project; https://github.com/huxingyi/sundown_pygmentize_glue/archive/master.zip
    REM Change directory to this project; cd sundown_pygmentize_glue-master
    type README.md | sundown_pygmentize_glue.exe > README.html
    REM If you have installed cygwin, please use: cat README.md | ./sundown_pygmentize_glue > README.html
```

USAGE(LINUX):  
I have not test it on linux, but i think it will works as the same as on windows. You should compile Sundown and install Pygments.

USAGE(EMACS):  
```lisp
;; markdown2html
(defun m2h (filename)
  "Convert markdown to html. Usage: M-x m2h <RET>
The tool 'sundown_pygmentize_glue' is needed, which can be downloaded at
https://github.com/huxingyi/sundown_pygmentize_glue
"
  (interactive "sFilename, enter the absolute path to file(default is current buffer): ")
  (let ((f filename)
        ;; here you should replace the path to your own
        (cmd0 "cd /d d:/files/bin/sundown_pygmentize_glue-master && cat ")
        (cmd_t " | sundown_pygmentize_glue.exe > ")
        cmd_tt
        dest)
    (if (string-equal "" f)
        (setq f (buffer-file-name)))
    (if (and (string-match "\\.[^.]+$" f)
             (file-name-extension f))
        (setq dest (replace-match "_.html" nil nil f))
      (setq dest (concat f "_.html")))
    (setq cmd_tt (concat " && start " dest))
    (if (file-exists-p f)
        (shell-command (concat cmd0 f cmd_t dest cmd_tt))
      (message "Error: file does not exist."))))
```

HOW DO I COMPILE THE SUNDOWN?  
 Modify this line in sundown/examples/sundown.c to enable all mkd_extensions:
```c
    markdown = sd_markdown_new(0xff, 16, &callbacks, &options);
```

CHARSET:  
Only supports "utf-8". utf-8 in utf-8 out. 
You could replace all "utf-8" in `pygmentize.bat` and `src/sundown_pygmentize_glue.c` to support other charsets.
