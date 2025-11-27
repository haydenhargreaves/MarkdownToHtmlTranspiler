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

    fn parse_paragraph(&mut self) -> Node {
        self.consume_whitespace();
        Node::Paragraph {
            children: self.parse_inline(),
        }
    }
    // --- INLINE PARSING ---
    fn parse_inline(&mut self) -> Vec<Node> {
        let mut nodes = vec![];
        let mut str = String::new();

        while !self.is_eof() {
            let c1 = self.peek();
            let c2 = self.peek_nth(1);
            let c3 = self.peek_nth(2);

            println!("parse_inline: c1={:?}, c2={:?}, c3={:?}", c1, c2, c3); // DEBUG

            match (c1, c2, c3) {
                (None, _, _) | (Some('\n'), Some('\n'), _) => break,

                // Check for ** (bold) before * (italic)
                (Some('*'), Some('*'), _) => {
                    println!("Matched bold");
                    if !str.is_empty() {
                        nodes.push(Node::Text {
                            content: str.clone(),
                        });
                        str.clear();
                    }
                    nodes.push(self.parse_bold());
                }

                // Check for * (italic)
                (Some('*'), _, _) => {
                    println!("Matched italic");
                    if !str.is_empty() {
                        nodes.push(Node::Text {
                            content: str.clone(),
                        });
                        str.clear();
                    }
                    nodes.push(self.parse_italic());
                }

                (Some(c), _, _) => {
                    str.push(c);
                    self.consume();
                }
            }
        }

        if !str.is_empty() {
            nodes.push(Node::Text { content: str });
        }

        nodes
    }

    fn parse_bold(&mut self) -> Node {
        self.consume_n(2); // Consume opening '**'

        let mut children = vec![];
        let mut str = String::new();

        while !self.is_eof() {
            let c1 = self.peek();
            let c2 = self.peek_nth(1);

            println!("parse_bold: c1={:?} c2={:?}", c1, c2);

            match (c1, c2) {
                (None, _) | (Some('\n'), Some('\n')) => break,

                // Found closing '**'
                (Some('*'), Some('*')) => {
                    println!("parse_bold: matched closing bold");
                    if !str.is_empty() {
                        children.push(Node::Text { content: str });
                    }
                    self.consume_n(2);
                    return Node::Bold { children };
                }

                // Single '*' inside bold (italic)
                (Some('*'), _) => {
                    println!("parse_bold: matched italic");
                    if !str.is_empty() {
                        children.push(Node::Text {
                            content: str.clone(),
                        });
                        str.clear();
                    }
                    children.push(self.parse_italic());
                }

                (Some(c), _) => {
                    str.push(c);
                    self.consume();
                }
            }
        }

        // No closing '**' found - return as text with '**' prefix
        if !str.is_empty() {
            children.push(Node::Text { content: str });
        }
        let mut text = String::from("**");
        for child in children {
            if let Node::Text { content } = child {
                text.push_str(&content);
            }
        }
        Node::Text { content: text }
    }

    fn parse_italic(&mut self) -> Node {
        self.consume(); // Consume opening '*'

        let mut children = vec![];
        let mut str = String::new();

        while !self.is_eof() {
            let c1 = self.peek();
            let c2 = self.peek_nth(1);

            println!("parse_italic: c1={:?} c2={:?}", c1, c2);

            match (c1, c2) {
                (None, _) | (Some('\n'), Some('\n')) => break,

                // Check for '**' (bold inside italic)
                (Some('*'), Some('*')) => {
                    println!("parse_italic: matched bold");
                    if !str.is_empty() {
                        children.push(Node::Text {
                            content: str.clone(),
                        });
                        str.clear();
                    }
                    children.push(self.parse_bold());
                }

                // Single '*' - our closing delimiter
                (Some('*'), _) => {
                    println!("parse_italic: matched closing italic");
                    if !str.is_empty() {
                        children.push(Node::Text { content: str });
                    }
                    self.consume();
                    return Node::Italic { children };
                }

                (Some(c), _) => {
                    str.push(c);
                    self.consume();
                }
            }
        }

        // No closing '*' found - return as text with '*' prefix
        if !str.is_empty() {
            children.push(Node::Text { content: str });
        }
        let mut text = String::from("*");
        for child in children {
            if let Node::Text { content } = child {
                text.push_str(&content);
            }
        }
        Node::Text { content: text }
    }
    // --- INLINE PARSING ---
    // fn parse_inline(&mut self) -> Vec<Node> {
    //     let mut nodes = vec![];
    //     let mut str = String::new();
    //
    //     while !self.is_eof() {
    //         let c1 = self.peek();
    //         let c2 = self.peek_nth(1);
    //         let c3 = self.peek_nth(2);
    //
    //         match (c1, c2, c3) {
    //             // Stop at double newline (paragraph break)
    //             (None, _, _) | (Some('\n'), Some('\n'), _) => break,
    //
    //             // Check for *** (bold italic) - LONGEST FIRST
    //             (Some('*'), Some('*'), Some('*')) => {
    //                 if !str.is_empty() {
    //                     nodes.push(Node::Text {
    //                         content: str.clone(),
    //                     });
    //                     str.clear();
    //                 }
    //                 nodes.push(self.parse_bold_italic());
    //             }
    //
    //             // Check for ** (bold)
    //             (Some('*'), Some('*'), _) => {
    //                 if !str.is_empty() {
    //                     nodes.push(Node::Text {
    //                         content: str.clone(),
    //                     });
    //                     str.clear();
    //                 }
    //                 nodes.push(self.parse_bold());
    //             }
    //
    //             // Check for * (italic)
    //             (Some('*'), _, _) => {
    //                 if !str.is_empty() {
    //                     nodes.push(Node::Text {
    //                         content: str.clone(),
    //                     });
    //                     str.clear();
    //                 }
    //                 nodes.push(self.parse_italic());
    //             }
    //
    //             // Regular character
    //             (Some(c), _, _) => {
    //                 str.push(c);
    //                 self.consume();
    //             }
    //         }
    //     }
    //
    //     if !str.is_empty() {
    //         nodes.push(Node::Text { content: str });
    //     }
    //
    //     nodes
    // }
    //
    // fn parse_bold_italic(&mut self) -> Node {
    //     self.consume_n(3); // Consume opening '***'
    //
    //     let mut children = vec![];
    //     let mut str = String::new();
    //
    //     while !self.is_eof() {
    //         let c1 = self.peek();
    //         let c2 = self.peek_nth(1);
    //         let c3 = self.peek_nth(2);
    //
    //         match (c1, c2, c3) {
    //             (None, _, _) | (Some('\n'), Some('\n'), _) => break,
    //
    //             // Found closing '***'
    //             (Some('*'), Some('*'), Some('*')) => {
    //                 if !str.is_empty() {
    //                     children.push(Node::Text { content: str });
    //                 }
    //                 self.consume_n(3);
    //                 return Node::BoldItalic { children };
    //             }
    //
    //             // Regular character (no nested formatting in bold-italic for simplicity)
    //             (Some(c), _, _) => {
    //                 str.push(c);
    //                 self.consume();
    //             }
    //         }
    //     }
    //
    //     // No closing '***' found - return as text
    //     if !str.is_empty() {
    //         children.push(Node::Text { content: str });
    //     }
    //     let mut text = String::from("***");
    //     for child in children {
    //         if let Node::Text { content } = child {
    //             text.push_str(&content);
    //         }
    //     }
    //     Node::Text { content: text }
    // }
    //
    // fn parse_bold(&mut self) -> Node {
    //     self.consume_n(2); // Consume opening '**'
    //
    //     let mut children = vec![];
    //     let mut str = String::new();
    //
    //     while !self.is_eof() {
    //         let c1 = self.peek();
    //         let c2 = self.peek_nth(1);
    //         let c3 = self.peek_nth(2);
    //
    //         match (c1, c2, c3) {
    //             (None, _, _) | (Some('\n'), Some('\n'), _) => break,
    //
    //             // Check for closing '**' but NOT '***'
    //             (Some('*'), Some('*'), Some('*')) => {
    //                 // This is ***, not our closing **
    //                 // Treat as text (or you could support nested bold-italic)
    //                 str.push('*');
    //                 self.consume();
    //             }
    //
    //             // Found closing '**'
    //             (Some('*'), Some('*'), _) => {
    //                 if !str.is_empty() {
    //                     children.push(Node::Text { content: str });
    //                 }
    //                 self.consume_n(2);
    //                 return Node::Bold { children };
    //             }
    //
    //             // Single '*' inside bold (italic)
    //             (Some('*'), _, _) => {
    //                 if !str.is_empty() {
    //                     children.push(Node::Text {
    //                         content: str.clone(),
    //                     });
    //                     str.clear();
    //                 }
    //                 children.push(self.parse_italic());
    //             }
    //
    //             (Some(c), _, _) => {
    //                 str.push(c);
    //                 self.consume();
    //             }
    //         }
    //     }
    //
    //     // No closing '**' found - return as text with '**' prefix
    //     if !str.is_empty() {
    //         children.push(Node::Text { content: str });
    //     }
    //     let mut text = String::from("**");
    //     for child in children {
    //         if let Node::Text { content } = child {
    //             text.push_str(&content);
    //         }
    //     }
    //     Node::Text { content: text }
    // }
    //
    // fn parse_italic(&mut self) -> Node {
    //     self.consume(); // Consume opening '*'
    //
    //     let mut children = vec![];
    //     let mut str = String::new();
    //
    //     while !self.is_eof() {
    //         let c1 = self.peek();
    //         let c2 = self.peek_nth(1);
    //         let c3 = self.peek_nth(2);
    //
    //         match (c1, c2, c3) {
    //             (None, _, _) | (Some('\n'), Some('\n'), _) => break,
    //
    //             // Check for '***' - not our closing
    //             (Some('*'), Some('*'), Some('*')) => {
    //                 // Treat as text or handle specially
    //                 str.push('*');
    //                 self.consume();
    //             }
    //
    //             // Check for '**' (bold inside italic)
    //             (Some('*'), Some('*'), _) => {
    //                 if !str.is_empty() {
    //                     children.push(Node::Text {
    //                         content: str.clone(),
    //                     });
    //                     str.clear();
    //                 }
    //                 children.push(self.parse_bold());
    //             }
    //
    //             // Single '*' - our closing delimiter
    //             (Some('*'), _, _) => {
    //                 if !str.is_empty() {
    //                     children.push(Node::Text { content: str });
    //                 }
    //                 self.consume();
    //                 return Node::Italic { children };
    //             }
    //
    //             (Some(c), _, _) => {
    //                 str.push(c);
    //                 self.consume();
    //             }
    //         }
    //     }
    //
    //     // No closing '*' found - return as text with '*' prefix
    //     if !str.is_empty() {
    //         children.push(Node::Text { content: str });
    //     }
    //     let mut text = String::from("*");
    //     for child in children {
    //         if let Node::Text { content } = child {
    //             text.push_str(&content);
    //         }
    //     }
    //     Node::Text { content: text }
    // }
    //
    //
    //
    //
    // fn parse_inline(&mut self) -> Vec<Node> {
    //     self.parse_inline_until(&[])
    // }
    //
    // fn parse_inline_until(&mut self, chars: &[char]) -> Vec<Node> {
    //     let mut nodes = vec![];
    //     let mut str = String::new();
    //
    //     while !self.is_eof() {
    //         let c1 = self.peek();
    //         let c2 = self.peek_nth(1);
    //         let c3 = self.peek_nth(2);
    //
    //         match (c1, c2, c3) {
    //             // Default stop conditions for all elements: empty or block break
    //             (None, _, _) | (Some('\n'), Some('\n'), _) => break,
    //
    //             // Parse bold nodes
    //             (Some('*'), Some('*'), _) => {
    //                 if !str.is_empty() {
    //                     nodes.push(Node::Text {
    //                         content: str.clone(),
    //                     });
    //                     str.clear();
    //                 }
    //                 nodes.push(self.parse_bold());
    //             }
    //
    //             // Check the dynamic stop conditions
    //             (Some(c), _, _) if chars.contains(&c) => break,
    //
    //             // Parse italic nodes
    //             (Some('*'), _, _) => {
    //                 if !str.is_empty() {
    //                     nodes.push(Node::Text {
    //                         content: str.clone(),
    //                     });
    //                     str.clear();
    //                 }
    //                 nodes.push(self.parse_italic());
    //             }
    //
    //             (Some(c), _, _) => {
    //                 str.push(c);
    //                 self.consume();
    //             }
    //         }
    //     }
    //
    //     // If content remains, push it to the list
    //     if !str.is_empty() {
    //         nodes.push(Node::Text { content: str });
    //     }
    //
    //     nodes
    // }
    //
    // fn parse_bold(&mut self) -> Node {
    //     self.consume_n(2); // Consume opening '**'
    //
    //     // DON'T pass '*' as stop char - we need to look for '**' specifically
    //     let mut children = vec![];
    //     let mut str = String::new();
    //
    //     while !self.is_eof() {
    //         let c1 = self.peek();
    //         let c2 = self.peek_nth(1);
    //
    //         match (c1, c2) {
    //             (None, _) | (Some('\n'), Some('\n')) => break,
    //
    //             // Found closing '**'
    //             (Some('*'), Some('*')) => {
    //                 if !str.is_empty() {
    //                     children.push(Node::Text { content: str });
    //                 }
    //                 self.consume_n(2);
    //                 return Node::Bold { children };
    //             }
    //
    //             // Single '*' inside bold (could be italic)
    //             (Some('*'), _) => {
    //                 if !str.is_empty() {
    //                     children.push(Node::Text {
    //                         content: str.clone(),
    //                     });
    //                     str.clear();
    //                 }
    //                 children.push(self.parse_italic());
    //             }
    //
    //             (Some(c), _) => {
    //                 str.push(c);
    //                 self.consume();
    //             }
    //         }
    //     }
    //
    //     // No closing '**' found
    //     if !str.is_empty() {
    //         children.push(Node::Text { content: str });
    //     }
    //     let mut text = String::from("**");
    //     for child in children {
    //         if let Node::Text { content } = child {
    //             text.push_str(&content);
    //         }
    //     }
    //     Node::Text { content: text }
    // }
    //
    // fn parse_italic(&mut self) -> Node {
    //     self.consume(); // Consume opening '*'
    //
    //     // Parse inline content until we hit closing '*' or end condition
    //     let children = self.parse_inline_until(&['*', '\n']);
    //
    //     // Check if we found the closing '*'
    //     if self.peek() == Some('*') {
    //         self.consume(); // Consume closing '*'
    //         Node::Italic { children }
    //     } else {
    //         // No closing '*' found - return as plain text with the '*' prefix
    //         let mut text = String::from("*");
    //         for child in children {
    //             if let Node::Text { content } = child {
    //                 text.push_str(&content);
    //             }
    //             // Note: This is simplified - you'd need to flatten properly
    //         }
    //         Node::Text { content: text }
    //     }
    // }

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
            let html = "*hello world\n";
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

    #[test]
    fn test_nested_bold_and_italics() {
        {
            let s = "***a***";
            let html = "<strong><em>a</em></strong>";
            let mut p = Parser::new(s);
            let nodes = p.parse_inline();
            assert_eq!(nodes.len(), 1);
            assert_eq!(nodes[0].to_html(), html);
        }
        {
            let s = "*a **b** c*";
            let html = "<em>a <strong>b</strong> c</em>";
            let mut p = Parser::new(s);
            let nodes = p.parse_inline();
            assert_eq!(nodes.len(), 1);
            assert_eq!(nodes[0].to_html(), html);
        }
        {
            let s = "**a *b* c**";
            let html = "<strong>a <em>b</em> c</strong>";
            let mut p = Parser::new(s);
            let nodes = p.parse_inline();
            assert_eq!(nodes.len(), 1);
            assert_eq!(nodes[0].to_html(), html);
        }
        {
            let s = "*a *b* c*";
            let html_1 = "<em>a </em>";
            let html_2 = "b";
            let html_3 = "<em> c</em>";
            let mut p = Parser::new(s);
            let nodes = p.parse_inline();
            assert_eq!(nodes.len(), 3);
            assert_eq!(nodes[0].to_html(), html_1);
            assert_eq!(nodes[1].to_html(), html_2);
            assert_eq!(nodes[2].to_html(), html_3);
        }
        {
            let s = "**a **b** c**";
            let html_1 = "<strong>a </strong>";
            let html_2 = "b";
            let html_3 = "<strong> c</strong>";
            let mut p = Parser::new(s);
            let nodes = p.parse_inline();
            assert_eq!(nodes.len(), 3);
            assert_eq!(nodes[0].to_html(), html_1);
            assert_eq!(nodes[1].to_html(), html_2);
            assert_eq!(nodes[2].to_html(), html_3);
        }
    }
}
