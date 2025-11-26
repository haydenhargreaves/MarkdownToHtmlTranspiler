use std::iter::Peekable;
use std::str::Chars;

use crate::node::Node;

#[derive(Debug)]
pub struct Parser {
    content: String,
}

impl Parser {
    /// Create a new parser object with the content attached. This does not take ownership of the
    /// string provided and therefore dies with the string. The input string is normalized to
    /// support operation on all operating systems.
    pub fn new(content: &str) -> Self {
        let normalized = content.replace("\r\n", "\n").replace("\r", "");
        Self {
            content: normalized,
        }
    }

    pub fn parse_document(&self) -> Node {
        let mut stream = self.content.chars().peekable();

        let mut root = Node::Document { children: vec![] };

        // Same as !IsEOF from the CPP implementation
        while stream.peek().is_some() {
            let block = self.parse_block(&mut stream);
            if !block.is_empty() {
                root.add_child(block);
            }
        }

        root
    }

    /// BUG: USING CLONES IS FUCKED AS HELL, STOP THIS SHIT
    fn parse_block(&self, stream: &mut Peekable<Chars<'_>>) -> Node {
        self.consume_whitespace(stream);

        // If we are at the end, return an empty node; it will be ignored
        let Some(&c1) = stream.peek() else {
            return Node::Text { content: "".into() };
        };

        // Use a clone to look ahead
        let mut clone = stream.clone();
        clone.next();

        let c2 = clone.next();
        let c3 = clone.next();

        // Now we can handle numbers from 0 to 99 for ordered lists
        match (c1, c2, c3) {
            ('#', _, _) => self.parse_heading(stream),
            ('*' | '-' | '+', Some(' ' | '\t'), _) => self.parse_list(stream, false),
            (d, Some('.'), _) if d.is_ascii_digit() => self.parse_list(stream, true),
            (d1, Some(d2), Some('.')) if d1.is_ascii_digit() && d2.is_ascii_digit() => {
                self.parse_list(stream, true)
            }
            ('`', Some('`'), Some('`')) => self.parse_code_block(stream),
            ('!', Some('['), _) => self.parse_image(stream),
            ('>', _, _) => self.parse_block_quote(stream),
            _ => self.parse_paragraph(stream),
        }
    }

    // --- STRUCTURE PARSING ---
    fn parse_paragraph(&self, stream: &mut Peekable<Chars<'_>>) -> Node {
        let children = self.parse_inline(stream);
        Node::Paragraph { children }
    }

    fn parse_heading(&self, stream: &mut Peekable<Chars<'_>>) -> Node {
        // Consume the hashes to determine the size, then consume the whitespace
        let hashes = self.consume_until_char(stream, ' ');
        self.consume_whitespace(stream);

        let children = self.parse_inline(stream);
        dbg!(&children);
        Node::Heading {
            level: hashes.len(),
            children,
        }
    }

    fn parse_list(&self, stream: &mut Peekable<Chars<'_>>, ordered: bool) -> Node {
        Node::Text { content: "".into() }
    }

    fn parse_block_quote(&self, stream: &mut Peekable<Chars<'_>>) -> Node {
        Node::Text { content: "".into() }
    }

    fn parse_code_block(&self, stream: &mut Peekable<Chars<'_>>) -> Node {
        Node::Text { content: "".into() }
    }

    fn parse_image(&self, stream: &mut Peekable<Chars<'_>>) -> Node {
        Node::Text { content: "".into() }
    }

    // --- INLINE PARSING ---
    fn parse_inline(&self, stream: &mut Peekable<Chars<'_>>) -> Vec<Node> {
        let mut nodes = vec![];
        let mut str = String::new();

        // use a clone to allow for peeking ahead
        // REMEMBER TO ALSO CONSUME ANYTIME MAIN STREAM IS CONSUMED
        let mut clone = stream.clone();
        clone.next(); // Stay one ahead

        while let Some(&c1) = stream.peek() {
            let c2 = clone.next();
            let c3 = clone.next();

            // println!("({}, {}, {})", c1, c2.unwrap_or('~'), c3.unwrap_or('~'));

            match (c1, c2, c3) {
                ('\n', _, _) => break,
                ('!', Some('['), _) => { /* Image */ }
                ('[', _, _) => { /* Link */ }
                ('*', Some('*'), Some('*')) => { /* Bold Italic */ }
                ('*', Some('*'), _) => {
                    nodes.push(Node::Text { content: str });
                    str = "".into();
                    let node = self.parse_bold(stream);
                    if !node.is_empty() {
                        nodes.push(node)
                    }
                    continue;
                }
                ('*', _, _) => { /* Italic */ }
                ('`', _, _) => { /* Code */ }

                _ => {
                    // Should we swap '\n' with ' '
                    str.push(c1);
                    stream.next();
                    clone.next();
                }
            }
        }

        // Push final node
        if !str.is_empty() {
            nodes.push(Node::Text { content: str });
        }
        nodes
    }

    /// BUG: THIS FUNCTION SHOULD PARSE UNTIL IT FINDS EITHER THE ** OR AN ENDING OF A BLOCK. FOR
    /// EXAMPLE: \n\n IS A NEW BLOCK AND THEN IT SHOULD END. BUT IF IT ENDS ON A NEW BLOCK, IT
    /// SHOULD RETURN A TEXT NODE, WITH THE ** PREPENDED, SIGNIFYING FAILURE TO COMPLETE THE ENTIRE
    /// STRONG BLOCK.
    fn parse_bold(&self, stream: &mut Peekable<Chars<'_>>) -> Node {
        let mut str = String::new();

        stream.next();
        stream.next();

        let mut clone = stream.clone();
        clone.next();

        while let Some(&c1) = stream.peek() {
            let c2 = clone.peek();

            match (c1, c2) {
                ('\n', Some('\n')) => break,
                ('*', Some('*')) => {
                    stream.next();
                    stream.next();
                    break;
                }
                _ => str.push(c1),
            }
            stream.next();
            clone.next();
        }

        println!("@str '{}'", str);

        Node::Bold { content: str }
    }

    // --- HELPER FUNCTIONS ---
    fn consume_whitespace(&self, stream: &mut Peekable<Chars<'_>>) {
        while let Some(&c) = stream.peek() {
            if !c.is_whitespace() {
                break;
            }
            stream.next();
        }
    }

    /// THIS DOES NOT CONSUME THE TARGET, IT STOPS RIGHT BEFORE IT AND RETURNS THE STRING UNTIL
    /// ITSELF
    fn consume_until_char(&self, stream: &mut Peekable<Chars<'_>>, target: char) -> String {
        let mut out = String::new();

        while let Some(&c) = stream.peek() {
            if c == target {
                break;
            }
            stream.next();
            out.push(c);
        }

        out
    }
}
