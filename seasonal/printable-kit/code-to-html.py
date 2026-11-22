"""Turn source code into highlighted <pre> HTML with numbered markers.

usage: codehtml.py <source> <html> <lang> <MARKER>=<from>-<to> ... --anno N=line ... --hl from-to
Replaces <!--MARKER--> ... <!--/MARKER--> in the html with a <pre> of those lines.
"""
import html, re, sys

src, out, lang = sys.argv[1:4]
args = sys.argv[4:]
blocks, anno, hl = [], {}, set()
mode = "blocks"
for a in args:
    if a == "--anno": mode = "anno"; continue
    if a == "--hl": mode = "hl"; continue
    if mode == "blocks":
        name, rng = a.split("="); f, t = map(int, rng.split("-")); blocks.append((name, f, t))
    elif mode == "anno":
        n, line = a.split("="); anno[int(line)] = n
    else:
        f, t = map(int, a.split("-")); hl.update(range(f, t + 1))

lines = open(src).read().split("\n")
if lang == "c":
    KW = r"#include|const char|volatile int|int|void|for|if|else|while|return|break"
    COMMENT = re.compile(r"/\*.*?\*/")
else:
    KW = r"from|import|def|for|in|if|while|True|and|or|not|range"
    COMMENT = re.compile(r"#.*$")
kw_re = re.compile(r"(?<![\w#])(" + KW + r")(?!\w)")
str_re = re.compile(r'"[^"]*"')

def colour(code):
    # split off comment, then highlight keywords outside strings
    m = COMMENT.search(code)
    # don't treat '#' inside strings as a python comment
    if lang == "py" and m:
        before = code[:m.start()]
        if before.count('"') % 2 == 1:
            m = None
    body, com = (code[:m.start()], code[m.start():]) if m else (code, "")
    parts, last = [], 0
    for s in str_re.finditer(body):
        parts.append(kw_re.sub(r'<span class="k">\1</span>', html.escape(body[last:s.start()], quote=False)))
        parts.append(html.escape(s.group(), quote=False))
        last = s.end()
    parts.append(kw_re.sub(r'<span class="k">\1</span>', html.escape(body[last:], quote=False)))
    res = "".join(parts)
    if com:
        res += '<span class="c">' + html.escape(com, quote=False) + "</span>"
    return res

page = open(out).read()
for name, f, t in blocks:
    rendered = []
    i = f
    while i <= t:
        if i in hl:
            j = i
            group = []
            while j <= t and j in hl:
                group.append(colour(lines[j - 1]) + (f'<span class="anno">{anno[j]}</span>' if j in anno else ""))
                j += 1
            rendered.append('<span class="hl">' + "\n".join(group) + "</span>")
            i = j
            continue
        rendered.append(colour(lines[i - 1]) + (f'<span class="anno">{anno[i]}</span>' if i in anno else ""))
        i += 1
    pre = '<pre class="code">' + "\n".join(rendered) + "</pre>"
    page, n = re.subn(r"<!--%s-->.*?<!--/%s-->" % (name, name), lambda _: f"<!--{name}-->{pre}<!--/{name}-->", page, flags=re.S)
    assert n == 1, name
open(out, "w").write(page)
print("ok")
