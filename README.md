# MarkdownToHtmlCompiler

This compiler will convert a Markdown file into an HTML output.



### Notes

Recursive Descent Parser: This is the primary algorithm you'll use. It's a top-down parsing technique
where a set of recursive functions "descend" through the grammar of your simple Markdown language. 
For example, a parse_document() function would call parse_line(), which in turn might call parse_bold_text()
or parse_italic_text(). This method is intuitive and easy to implement for a simple grammar.

Stack: A stack is essential for handling nested elements. For instance, if you allow bold text inside
italic text (_This is *bold and italic* text_), you can push the _ token onto the stack and then push
the * token. When you encounter the closing *, you check if the top of the stack matches. This ensures
that all tags are correctly opened and closed. Your presentation can visually demonstrate this process 
with a stack diagram.

Hash Map or Map: A hash map (std::unordered_map) or a map (std::map) can be used to efficiently store
and retrieve the HTML equivalent for each Markdown tag. For example, you could map `#` to `<h1>` or "_"
to `<em>`. This provides O(1) average-case lookup time.

### Targets

- [-] Convert a .md file to an .html output

### Reaches
- [ ] Hot reload?


