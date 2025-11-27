use crate::node::Node;

#[derive(Debug)]
pub struct Parser<'a> {
    content: &'a str,
    position: usize,
}

impl<'a> Parser<'a> {
    // Content should be normalized before being passed into this function. Since we do not take
    // ownership here, we cannot mutate it.
    pub fn new(content: &'a str) -> Self {
        Self {
            content,
            position: 0,
        }
    }

    pub fn parse_document(&mut self) -> Node {
        let mut root = Node::Document { children: vec![] };

        while !self.is_eof() {
            let block = self.parse_block();
            if !block.is_empty() {
                root.add_child(block);
            }
        }

        root
    }

    // BUG: We should be using optional, not blank nodes
    fn parse_block(&mut self) -> Node {
        self.consume_whitespace();

        // If we are at the end, return an empty node, the caller should know to return it.
        if self.is_eof() {
            return Node::Text { content: "".into() };
        }

        let c1 = self.peek();
        let c2 = self.peek_nth(1);
        let c3 = self.peek_nth(2);

        // TODO: For now we are just implementing paragraphs. So we can start with inline parsing
        match (c1, c2, c3) {
            // (Some('#'), _, _) => self.parse_heading(),
            _ => self.parse_paragraph(),
        }
    }

    fn parse_heading(&mut self) -> Node {
        Node::Heading {
            level: 1,
            children: vec![],
        }
    }

    fn parse_paragraph(&mut self) -> Node {
        Node::Paragraph {
            children: self.parse_inline(),
        }
    }

    // --- INLINE PARSING ---
    fn parse_inline(&mut self) -> Vec<Node> {
        let mut nodes = vec![];
        let mut str = "".to_string();

        while !self.is_eof() {
            // c1 stores current char, c2/c3 store future, contextual chars
            let c1 = self.peek();
            let c2 = self.peek_nth(1);
            let c3 = self.peek_nth(2);

            // TODO: Need to redesign the nodes
            match (c1, c2, c3) {
                (None, _, _) | (Some('\n'), Some('\n'), _) => break,
                (Some('!'), Some('['), _) =>
                /* parse image */
                {
                    continue;
                }
                (Some('['), _, _) =>
                /* parse link */
                {
                    continue;
                }
                (Some('*'), Some('*'), Some('*')) =>
                /* parse bold italic */
                {
                    continue;
                }
                (Some('*'), Some('*'), _) =>
                /* parse bold */
                {
                    continue;
                }
                (Some('*'), _, _) => {
                    nodes.push(Node::Text {
                        content: str.clone(),
                    });
                    str = "".to_string();
                    let node = self.parse_italic();
                    if !node.is_empty() {
                        nodes.push(node);
                    }
                    continue;
                }
                (Some('`'), _, _) =>
                /* parse code */
                {
                    continue;
                }

                (Some(c), _, _) => {
                    str.push(c);
                    self.consume();
                }
            }
        }

        // TODO: Push text node
        nodes.push(Node::Text { content: str });
        nodes
    }

    fn parse_italic(&mut self) -> Node {
        let mut str = "".to_string();
        self.consume(); // Consume the '*'

        println!("'{}'", self.content);

        // Use loop instead of 'while !self.is_eof()' so we can make it to the (None, _) case to
        // exit
        loop {
            let c1 = self.peek();
            let c2 = self.peek_nth(1);

            match (c1, c2) {
                (None, _) | (Some('\n'), None) | (Some('\n'), Some('\n')) => {
                    // In this case, we did not find an ending star, so we should return a normal
                    // node. But we have to add the star back since we consumed it already
                    str.insert(0, '*');
                    return Node::Text { content: str };
                }
                (Some('*'), _) => {
                    self.consume();
                    break;
                }
                (Some(c), _) => {
                    str.push(c);
                    self.consume();
                }
            }
        }

        Node::Italic { content: str }
    }

    // --- HELPERS ---
    fn is_eof(&self) -> bool {
        self.position >= self.content.len()
    }

    fn peek(&self) -> Option<char> {
        self.peek_nth(0)
    }

    fn peek_nth(&self, n: usize) -> Option<char> {
        self.content[self.position..].chars().nth(n)
    }

    fn consume(&mut self) {
        self.consume_n(1)
    }

    fn consume_n(&mut self, n: usize) {
        for _ in 0..n {
            if let Some(c) = self.content[self.position..].chars().next() {
                self.position += c.len_utf8();
            } else {
                break;
            }
        }
    }

    fn consume_whitespace(&mut self) {
        while let Some(c) = self.peek() {
            if !c.is_whitespace() {
                break;
            }
            self.consume();
        }
    }
}

#[cfg(test)]
mod parser_tests {
    use super::Parser;

    #[test]
    fn test_parse_italic() {
        // This test only tests the `parse_italic` method, so it is expected that the first
        // character is a '*', otherwise the first character will be consumed. The `parse_inline`
        // tests will be able to handle more specific cases.
        {
            let s = "*hello world*";
            let html = "<em>hello world</em>";
            let mut p = Parser::new(s);
            let node = p.parse_italic();
            assert_eq!(node.to_html(), html);
        }
        {
            let s = "*hello* world";
            let html = "<em>hello</em>";
            let mut p = Parser::new(s);
            let node = p.parse_italic();
            assert_eq!(node.to_html(), html);
        }
        {
            let s = "*hello world";
            let html = "*hello world";
            let mut p = Parser::new(s);
            let node = p.parse_italic();
            assert_eq!(node.to_html(), html);
        }
        {
            let s = "*hello world\n";
            let html = "*hello world";
            let mut p = Parser::new(s);
            let node = p.parse_italic();
            assert_eq!(node.to_html(), html);
        }
        {
            let s = "*hello world\n\n";
            let html = "*hello world";
            let mut p = Parser::new(s);
            let node = p.parse_italic();
            assert_eq!(node.to_html(), html);
        }
        {
            let s = "*hello\n\nworld*";
            let html = "*hello";
            let mut p = Parser::new(s);
            let node = p.parse_italic();
            assert_eq!(node.to_html(), html);
        }
    }
}
