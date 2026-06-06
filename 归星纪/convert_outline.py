"""Convert the full volume outline from markdown to docx."""
import re
from docx import Document
from docx.shared import Pt, Cm, RGBColor
from docx.enum.text import WD_ALIGN_PARAGRAPH

FONT_NAME = '宋体'
FONT_SIZE = Pt(12)
INDENT = Pt(24)  # 2 chars first-line indent
LINE_SPACING = 1.5

def set_run_font(run, size=Pt(12), bold=False, color=None):
    run.font.size = size
    run.font.name = FONT_NAME
    run.font.bold = bold
    run._element.rPr.rFonts.set(
        '{http://schemas.openxmlformats.org/wordprocessingml/2006/main}eastAsia', FONT_NAME)
    if color:
        run.font.color.rgb = color

def add_styled_para(doc, text, size=Pt(12), bold=False, align=None, indent=True, color=None):
    para = doc.add_paragraph()
    para.paragraph_format.line_spacing = LINE_SPACING
    para.paragraph_format.space_before = Pt(0)
    para.paragraph_format.space_after = Pt(0)
    if indent:
        para.paragraph_format.first_line_indent = INDENT
    if align is not None:
        para.alignment = align
    run = para.add_run(text)
    set_run_font(run, size=size, bold=bold, color=color)
    return para

def process_inline(text):
    """Handle bold (**text**) and italic (*text*) inline markup."""
    parts = []
    # Split on bold markers
    segments = re.split(r'(\*\*.*?\*\*)', text)
    for seg in segments:
        if seg.startswith('**') and seg.endswith('**'):
            parts.append(('bold', seg[2:-2]))
        else:
            # Handle italic within non-bold segments
            sub_segs = re.split(r'(\*.*?\*)', seg)
            for sub in sub_segs:
                if sub.startswith('*') and sub.endswith('*'):
                    parts.append(('italic', sub[1:-1]))
                else:
                    parts.append(('normal', sub))
    return parts

def add_rich_para(doc, text, size=Pt(12), indent=True, color=None):
    """Add a paragraph with inline formatting (bold, italic)."""
    para = doc.add_paragraph()
    para.paragraph_format.line_spacing = LINE_SPACING
    para.paragraph_format.space_before = Pt(0)
    para.paragraph_format.space_after = Pt(0)
    if indent:
        para.paragraph_format.first_line_indent = INDENT
    parts = process_inline(text)
    for style, content in parts:
        run = para.add_run(content)
        set_run_font(run, size=size, bold=(style == 'bold'), color=color)
    return para

def convert_outline(md_path, docx_path):
    with open(md_path, 'r', encoding='utf-8') as f:
        lines = f.readlines()

    doc = Document()
    for section in doc.sections:
        section.top_margin = Cm(2.54)
        section.bottom_margin = Cm(2.54)
        section.left_margin = Cm(3.18)
        section.right_margin = Cm(3.18)

    in_table = False
    in_code = False
    in_quote = False
    code_lines = []
    table_rows = []

    def flush_code():
        nonlocal code_lines
        if code_lines:
            for cl in code_lines:
                para = doc.add_paragraph()
                para.paragraph_format.line_spacing = 1.2
                para.paragraph_format.space_before = Pt(0)
                para.paragraph_format.space_after = Pt(0)
                para.paragraph_format.left_indent = Cm(1)
                run = para.add_run(cl)
                set_run_font(run, size=Pt(9), color=RGBColor(80, 80, 80))
            code_lines = []

    def flush_table():
        nonlocal table_rows
        if table_rows:
            # Parse markdown table
            if len(table_rows) >= 2:
                # header row
                headers = [c.strip() for c in table_rows[0].split('|') if c.strip()]
                # skip separator row (index 1)
                data_rows = []
                for r in table_rows[2:]:
                    cells = [c.strip() for c in r.split('|') if c.strip()]
                    data_rows.append(cells)

                num_cols = len(headers)
                table = doc.add_table(rows=1 + len(data_rows), cols=num_cols)
                table.style = 'Table Grid'
                # Header
                for i, h in enumerate(headers):
                    cell = table.rows[0].cells[i]
                    cell.text = ''
                    run = cell.paragraphs[0].add_run(h)
                    set_run_font(run, size=Pt(10), bold=True)
                # Data
                for ri, row in enumerate(data_rows):
                    for ci, cell_text in enumerate(row):
                        if ci < num_cols:
                            cell = table.rows[ri + 1].cells[ci]
                            cell.text = ''
                            run = cell.paragraphs[0].add_run(cell_text)
                            set_run_font(run, size=Pt(10))
                doc.add_paragraph()  # spacing after table
            table_rows = []

    i = 0
    while i < len(lines):
        line = lines[i].rstrip()

        # Code blocks
        if line.startswith('```'):
            if in_code:
                flush_code()
                in_code = False
            else:
                flush_table()
                in_code = True
                code_lines = []
            i += 1
            continue

        if in_code:
            code_lines.append(line)
            i += 1
            continue

        # Table detection
        if line.startswith('|') and line.endswith('|'):
            flush_code()
            in_table = True
            table_rows.append(line)
            i += 1
            continue
        else:
            if in_table:
                flush_table()
                in_table = False

        # Blockquote
        if line.startswith('> '):
            flush_code()
            flush_table()
            content = line[2:]
            add_rich_para(doc, content, size=Pt(12), indent=True, color=RGBColor(100, 100, 100))
            i += 1
            continue

        # Horizontal rule
        if line.strip() == '---':
            flush_code()
            flush_table()
            doc.add_paragraph()
            i += 1
            continue

        # Headers
        h_match = re.match(r'^(#{1,4})\s+(.+)$', line)
        if h_match:
            flush_code()
            flush_table()
            level = len(h_match.group(1))
            title_text = h_match.group(2)
            if level == 1:
                add_styled_para(doc, title_text, size=Pt(18), bold=True, align=WD_ALIGN_PARAGRAPH.CENTER, indent=False)
            elif level == 2:
                add_styled_para(doc, title_text, size=Pt(15), bold=True, indent=False)
            elif level == 3:
                add_styled_para(doc, title_text, size=Pt(13), bold=True, indent=False)
            elif level == 4:
                add_styled_para(doc, title_text, size=Pt(12), bold=True, indent=False)
            i += 1
            continue

        # Empty line
        if not line.strip():
            flush_code()
            flush_table()
            i += 1
            continue

        # Regular paragraph
        flush_code()
        flush_table()

        # Handle bold markers
        if '**' in line:
            add_rich_para(doc, line, size=FONT_SIZE, indent=True)
        else:
            add_styled_para(doc, line, size=FONT_SIZE, indent=True)

        i += 1

    # Flush remaining
    flush_code()
    flush_table()

    doc.save(docx_path)
    print(f'Done: {docx_path}')

if __name__ == '__main__':
    src = 'E:/项目/归星纪/归星纪-第一部总纲-起点版.md'
    dst = 'E:/项目/归星纪/归星纪-第一部-未定义文明-总纲.docx'
    convert_outline(src, dst)
