#include <stdio.h>
#include <stdlib.h>

#define BLOCK_TYPE_TEXT 1
#define BLOCK_TYPE_CODE 2

#define CODE_TAG "```"
#define CODE_TAG_LEN (sizeof(CODE_TAG) - 1)

#define INLINE_CODE_TAG "`"
#define INLINE_CODE_TAG_LEN (sizeof(INLINE_CODE_TAG) - 1)

#define SUNDOWN_PYGMENTIZE_GLUE_TMP_IN "sundown_pygmentize_glue_tmp_in.bin"
#define SUNDOWN_PYGMENTIZE_GLUE_TMP_OUT "sundown_pygmentize_glue_tmp_out.bin"
#define SUNDOWN_PYGMENTIZE_GLUE_STUB_ "SUNDOWNPYGMENTIZEGLUESTUB"
#define SUNDOWN_PYGMENTIZE_GLUE_STUB_LEN (sizeof(SUNDOWN_PYGMENTIZE_GLUE_STUB_) - 1)
#define SUNDOWN_PYGMENTIZE_GLUE_CSS_FILE "./style.css"

#if defined(_WIN32) || defined(__CYGWIN__)
#define PYGMENTIZE_CMD "type ./" SUNDOWN_PYGMENTIZE_GLUE_TMP_IN " | pygmentize.bat -l %s -f html -o ./" SUNDOWN_PYGMENTIZE_GLUE_TMP_OUT
#define SUNDOWN_CMD "sundown.exe ./" SUNDOWN_PYGMENTIZE_GLUE_TMP_IN "> ./" SUNDOWN_PYGMENTIZE_GLUE_TMP_OUT
#else
#define PYGMENTIZE_CMD "cat ./" SUNDOWN_PYGMENTIZE_GLUE_TMP_IN " | ./pygmentize -l %s -f html -o ./" SUNDOWN_PYGMENTIZE_GLUE_TMP_OUT
#define SUNDOWN_CMD "./sundown ./" SUNDOWN_PYGMENTIZE_GLUE_TMP_IN "> ./" SUNDOWN_PYGMENTIZE_GLUE_TMP_OUT
#endif

typedef struct _block_t
{
    int is_inline;
    int type;
    int length;
    char lexer[50];
    char *content;
    struct _block_t *next;
} block_t;

#define rtrim(_line) do {\
    char *_p = (_line) + strlen(_line) - 1;\
    while ((_p >= (_line)) && \
        ('\r' == _p[0] ||\
        '\n' == _p[0] ||\
        ' ' == _p[0] ||\
        '\t' == _p[0]))\
    {\
        _p[0] = '\0';\
        --_p;\
    }\
} while (0)

#define ltrim(_line, _ret) do {\
    char *_p = (_line);\
    while ('\r' == _p[0] ||\
        '\n' == _p[0] ||\
        ' ' == _p[0] ||\
        '\t' == _p[0])\
    {\
        ++_p;\
    }\
    *(_ret) = _p;\
} while (0)

block_t *alloc_block(void)
{
    block_t *block = (block_t *)malloc(sizeof(block_t));
    if (!block)
    {
        return 0;
    }
    memset(block, 0, sizeof(block_t));
    return block;
}

#define ALLOC_BLOCK() do {\
    new_block = alloc_block();\
    if (!new_block)\
    {\
        fprintf(stderr, "alloc_block failed.\r\n");\
        goto failed;\
    }\
    if (last_block)\
    {\
        last_block->next = new_block;\
    }\
    else\
    {\
        first_block = new_block;\
    }\
    last_block = new_block;\
} while (0)

#define APPEND_TO_BLOCK(_block, _data, _len) do {\
    int _lenval = (int)(_len); \
    (_block)->content = realloc((_block)->content, (_block)->length + _lenval);\
    if (!(_block)->content)\
    {\
        fprintf(stderr, "realloc failed.\r\n");\
        goto failed;\
    }\
    memcpy((_block)->content + (_block)->length, \
        (_data), _lenval); \
    (_block)->length += _lenval; \
} while (0)

#define APPEND_TO_LAST_BLOCK(_data, _len) do {\
    if (!last_block)\
    {\
        ALLOC_BLOCK();\
        last_block->type = BLOCK_TYPE_TEXT; \
    }\
    APPEND_TO_BLOCK(last_block, (_data), (_len)); \
} while (0)

static void write_css(FILE *out)
{
    FILE *fp = fopen(SUNDOWN_PYGMENTIZE_GLUE_CSS_FILE, "rb");
    char line[10240];
    if (!fp)
    {
        return;
    }
    fprintf(out, "<style>\r\n");
    while (fgets(line, sizeof(line)- 1, fp) > 0)
    {
        fwrite(line, strlen(line), 1, out);
    }
    fprintf(out, "</style>\r\n");
    fclose(fp);
}

static int dispatch_tool(const char *cmd,
    block_t *input,
    block_t *output)
{
    int result;
    FILE *tmp_out = 0;
    FILE *tmp_in = fopen("./" SUNDOWN_PYGMENTIZE_GLUE_TMP_IN, "wb");
    if (!tmp_in)
    {
        fprintf(stderr, "fopen " SUNDOWN_PYGMENTIZE_GLUE_TMP_IN " failed.\r\n");
        return -1;
    }
    if (1 != fwrite(input->content, input->length, 1, tmp_in))
    {
        fprintf(stderr, "fwrite " SUNDOWN_PYGMENTIZE_GLUE_TMP_IN " failed.\r\n");
        return -1;
    }
    fclose(tmp_in);
    tmp_in = 0;
    unlink("./" SUNDOWN_PYGMENTIZE_GLUE_TMP_OUT);
    result = system(cmd);
    if (0 == result)
    {
        char line[10240];
        tmp_out = fopen("./" SUNDOWN_PYGMENTIZE_GLUE_TMP_OUT, "rb");
        if (!tmp_out)
        {
            fprintf(stderr, "fopen " SUNDOWN_PYGMENTIZE_GLUE_TMP_OUT " failed.\r\n");
            result = -1;
            goto failed;
        }
        while (fgets(line, sizeof(line)- 1, tmp_out) > 0)
        {
            APPEND_TO_BLOCK(output, 
                line, strlen(line));
        }
    }
failed:
    if (tmp_out)
    {
        fclose(tmp_out);
        tmp_out = 0;
    }
    unlink("./" SUNDOWN_PYGMENTIZE_GLUE_TMP_IN);
    unlink("./" SUNDOWN_PYGMENTIZE_GLUE_TMP_OUT);
    return result;
}

int main(int argc, char *argv[])
{
    int result = -1;
    FILE *in = stdin;
    FILE *out = stdout;
    block_t *first_block = 0;
    block_t *last_block = 0;
    block_t *new_block = 0;
    block_t *loop_block;
    block_t glue_block;
    block_t sundown_output_block;
    int code_opened = 0;
    char line[10240];
    int in_size = 0;
    char *code_pos;
    char *p;
    char *stop;
    while (fgets(line, sizeof(line)- 1, in) > 0)
    {
        int append_len;
        p = line;
        stop = p + strlen(line);
        while (p < stop)
        {
            if ((code_pos=strstr(p, CODE_TAG)))
            {
                if (code_opened)
                {
                    code_opened = 0;
                    if (code_pos > p)
                    {
                        APPEND_TO_LAST_BLOCK(p, code_pos - p);
                    }
                    ALLOC_BLOCK();
                    last_block->type = BLOCK_TYPE_TEXT;
                    p = stop;
                }
                else
                {
                    char *lexer;
                    int lexerlen;
                    code_opened = 1;
                    if (code_pos > p)
                    {
                        APPEND_TO_LAST_BLOCK(p, code_pos - p);
                    }
                    ALLOC_BLOCK();
                    last_block->type = BLOCK_TYPE_CODE;
                    lexer = code_pos + CODE_TAG_LEN;
                    p = lexer + strlen(lexer);
                    rtrim(lexer);
                    lexerlen = strlen(lexer);
                    if (lexerlen + 1 > sizeof(last_block->lexer))
                    {
                        fprintf(stderr, "lexel(%s) too long.\r\n", lexer);
                        goto failed;
                    }
                    memcpy(last_block->lexer,
                        lexer, lexerlen + 1);
                }
                continue;
            }
            else if ((!code_opened) && (code_pos=strstr(p, INLINE_CODE_TAG)))
            {
                char *another_code_pos = strstr(code_pos + INLINE_CODE_TAG_LEN, INLINE_CODE_TAG);
                if (another_code_pos)
                {
                    APPEND_TO_LAST_BLOCK(p, 
                        code_pos - p);
                    //inline code
                    ALLOC_BLOCK();
                    last_block->is_inline = 1;
                    last_block->type = BLOCK_TYPE_CODE;
                    APPEND_TO_LAST_BLOCK(code_pos + INLINE_CODE_TAG_LEN, 
                        another_code_pos - (code_pos + INLINE_CODE_TAG_LEN));
                    ALLOC_BLOCK();
                    last_block->type = BLOCK_TYPE_TEXT;
                    p = another_code_pos + INLINE_CODE_TAG_LEN;
                    continue;
                }
            }
            append_len = strlen(p);
            APPEND_TO_LAST_BLOCK(p, append_len);
            p += append_len;
        }
    }
    
    /*
    for (loop_block = first_block;
        loop_block;
        loop_block = loop_block->next)
    {
        int i;
        fprintf(out, "block type:%d lexer:%s length:%d {",
            loop_block->type,
            loop_block->lexer,
            loop_block->length);
        for (i = 0;
            i < loop_block->length;
            ++i)
        {
            fprintf(out, "%c", loop_block->content[i]);
        }
        fprintf(out, "}\r\n");
    }
    */
    
    memset(&glue_block, 0, sizeof(glue_block));
    for (loop_block = first_block;
        loop_block;
        loop_block = loop_block->next)
    {
        if (BLOCK_TYPE_TEXT == loop_block->type)
        {
            APPEND_TO_BLOCK(&glue_block, 
                loop_block->content, loop_block->length);
        }
        else
        {
            char stub[50];
            sprintf(stub, SUNDOWN_PYGMENTIZE_GLUE_STUB_ "%08x",
                (unsigned long)loop_block);
            APPEND_TO_BLOCK(&glue_block, 
                stub, strlen(stub));
        }
    }
    
    //fprintf(out, "%s", glue_block.content);
    memset(&sundown_output_block, 0, sizeof(sundown_output_block));
    result = dispatch_tool(SUNDOWN_CMD,
        &glue_block,
        &sundown_output_block);
    if (0 != result)
    {
        fprintf(stderr, "sundown failed.\r\n");
        goto failed;
    }
    
    p = sundown_output_block.content;
    stop = p + sundown_output_block.length;
    
    //fwrite(p, stop - p, 1, out);
    //return 0;
    
    fprintf(out, "<html>\r\n");
    fprintf(out, "<head>\r\n");
    fprintf(out, "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\r\n");
    write_css(out);
    fprintf(out, "</head>\r\n");
    fprintf(out, "<body>\r\n");
    
    while (p < stop)
    {
        block_t *pyg_in;
        block_t pyg_out;
        char cmdline[260];
        char numstr[9];
        code_pos = strstr(p, SUNDOWN_PYGMENTIZE_GLUE_STUB_);
        if (code_pos)
        {
            if (code_pos > p)
            {
                fwrite(p, code_pos - p, 1, out);
            }
            memcpy(numstr, code_pos + SUNDOWN_PYGMENTIZE_GLUE_STUB_LEN, 8);
            numstr[8] = '\0';
            pyg_in = (block_t *)strtoul(numstr, 0, 16);
            if (pyg_in->is_inline)
            {
                fprintf(out, "<span class=\"inline\">");
                fwrite(pyg_in->content, pyg_in->length, 1, out);
                fprintf(out, "</span>");
            }
            else
            {
                result = -1;
                memset(&pyg_out, 0, sizeof(pyg_out));
                if (pyg_in->lexer[0])
                {
                    sprintf(cmdline,
                        PYGMENTIZE_CMD,
                        pyg_in->lexer);
                    result = dispatch_tool(cmdline,
                        pyg_in,
                        &pyg_out);
                }
                if (0 == result)
                {
                    fwrite(pyg_out.content, pyg_out.length, 1, out);
                }
                else
                {
                    fprintf(out, "<pre>");
                    fwrite(pyg_in->content, pyg_in->length, 1, out);
                    fprintf(out, "</pre>");
                }
                if (pyg_out.content)
                {
                    free(pyg_out.content);
                }
            }
            p = code_pos + SUNDOWN_PYGMENTIZE_GLUE_STUB_LEN + 8;
        }
        else
        {
            fwrite(p, stop - p, 1, out);
            break;
        }
    }
    fprintf(out, "</body>");
    fprintf(out, "</html>");
    
failed:
    return result;
}

