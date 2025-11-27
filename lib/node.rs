#[derive(Debug, Clone)]
pub enum Node {
    // Structure Nodes
    Document { children: Vec<Node> },
    Heading { level: usize, children: Vec<Node> },
    Paragraph { children: Vec<Node> },
    List { ordered: bool, children: Vec<Node> },
    ListItem { children: Vec<Node> },
    CodeBlock { children: Vec<Node> },
    BlockQuote { children: Vec<Node> },
    Link { href: String, children: Vec<Node> },

    Bold { children: Vec<Node> },
    Italic { children: Vec<Node> },
    BoldItalic { children: Vec<Node> },

    // Inline Nodes
    Text { content: String },
    Code { content: String },

    // Special Nodes
    Image { src: String, alt: String },
}

impl Node {
    /// Recursively convert a node into a HTML string. This is used to generate the DOM output
    /// tree. This should only be called on the root node of the tree. This function will recursively
    /// call itself to create the entire tree.
    ///
    /// Currently, this function does not create indentation, that would be a nice touch though.
    pub fn to_html(&self) -> String {
        match self {
            // Structure nodes
            Node::Document { children } => {
                let inner = children.iter().map(|x| x.to_html()).collect::<String>();
                format!(
                    "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n<meta charset=\"UTF-8\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n<title>Document</title>\n</head>\n<body>\n{}</body>\n</html>",
                    inner
                )
            }
            Node::Heading { level, children } => {
                let inner = children.iter().map(|x| x.to_html()).collect::<String>();
                format!("<h{level}>{}</h{level}>\n", inner, level = level)
            }
            Node::Paragraph { children } => {
                let inner = children.iter().map(|x| x.to_html()).collect::<String>();
                format!("<p>{}</p>\n", inner)
            }
            Node::List { ordered, children } => {
                let inner = children.iter().map(|x| x.to_html()).collect::<String>();
                let tag = if *ordered { "ol" } else { "ul" };
                format!("<{tag}>{}</{tag}>\n", inner, tag = tag)
            }
            Node::ListItem { children } => {
                let inner = children.iter().map(|x| x.to_html()).collect::<String>();
                format!("<li>{}</li>\n", inner)
            }
            Node::CodeBlock { children } => {
                let inner = children.iter().map(|x| x.to_html()).collect::<String>();
                format!("<code>{}</code>\n", inner)
            }
            Node::BlockQuote { children } => {
                let inner = children.iter().map(|x| x.to_html()).collect::<String>();
                format!("<blockquote>{}</blockquote>\n", inner)
            }
            Node::Link { href, children } => {
                let inner = children.iter().map(|x| x.to_html()).collect::<String>();
                format!("<a href=\"{}\">{}</a>", href, inner)
            }
            Node::Bold { children } => {
                let inner = children.iter().map(|x| x.to_html()).collect::<String>();
                format!("<strong>{}</strong>", inner)
            }
            Node::Italic { children } => {
                let inner = children.iter().map(|x| x.to_html()).collect::<String>();
                format!("<em>{}</em>", inner)
            }
            Node::BoldItalic { children } => {
                let inner = children.iter().map(|x| x.to_html()).collect::<String>();
                format!("<strong><em>{}</em></strong>", inner)
            }

            // Inline nodes
            Node::Text { content } => format!("{}", content),
            Node::Code { content } => format!("<code>{}</code>", content),

            // Special nodes
            Node::Image { src, alt } => format!("<img src=\"{}\" alt=\"{}\">\n", src, alt),
        }
    }

    /// Determines if a node is empty. For structure nodes (those with `children`) this will be
    /// true when there are no elements in the list. For inline nodes (those without `children`)
    /// this will be true when the string content of all fields are blank.
    pub fn is_empty(&self) -> bool {
        match self {
            // Structure nodes
            Node::Document { children }
            | Node::Heading { level: _, children }
            | Node::Paragraph { children }
            | Node::List {
                ordered: _,
                children,
            }
            | Node::ListItem { children }
            | Node::CodeBlock { children }
            | Node::BlockQuote { children }
            | Node::Bold { children }
            | Node::Italic { children }
            | Node::BoldItalic { children } => children.is_empty(),
            Node::Link { href, children } => children.is_empty() && href.is_empty(),

            // Inline nodes
            Node::Text { content } | Node::Code { content } => content.is_empty(),

            // Special rules
            Node::Image { src, alt } => src.is_empty() && alt.is_empty(),
        }
    }

    /// Returns Some children if they exist, otherwise None will be returned. For nodes that do not
    /// have children, None will be returned.
    pub fn children(&self) -> Option<&[Node]> {
        match self {
            // Structure Nodes
            Node::Document { children }
            | Node::Heading { level: _, children }
            | Node::Paragraph { children }
            | Node::List {
                ordered: _,
                children,
            }
            | Node::ListItem { children }
            | Node::CodeBlock { children }
            | Node::BlockQuote { children }
            | Node::Bold { children }
            | Node::Italic { children }
            | Node::BoldItalic { children }
            | Node::Link { href: _, children } => Some(&children),

            // Inline Nodes
            Node::Text { content: _ } | Node::Code { content: _ } => None,

            // Special Nodes
            Node::Image { src: _, alt: _ } => None,
        }
    }

    /// Add a child to the back of the list of children. If the node is a type which does not allow
    /// children to be added, this function will panic.
    pub fn add_child(&mut self, child: Node) {
        match self {
            // Structure Nodes
            Node::Document { children }
            | Node::Heading { level: _, children }
            | Node::Paragraph { children }
            | Node::List {
                ordered: _,
                children,
            }
            | Node::ListItem { children }
            | Node::CodeBlock { children }
            | Node::BlockQuote { children }
            | Node::Bold { children }
            | Node::Italic { children }
            | Node::BoldItalic { children }
            | Node::Link { href: _, children } => children.push(child),

            // Inline Nodes
            Node::Text { content: _ } | Node::Code { content: _ } => {
                panic!("Can't add child to this node type.")
            }

            // Special Nodes
            Node::Image { src: _, alt: _ } => panic!("Can't add child to this node type."),
        };
    }
}

#[cfg(test)]
mod node_tests {
    use super::Node;

    #[test]
    fn can_return_html_string_for_structure_nodes() {
        let child = Node::Text {
            content: "CONTENT".into(),
        };

        // Document
        {
            let node = Node::Document {
                children: vec![child.clone()],
            };
            let html = "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n<meta charset=\"UTF-8\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n<title>Document</title>\n</head>\n<body>\nCONTENT</body>\n</html>";
            assert_eq!(node.to_html(), html);
        }

        // Heading
        {
            let node_h1 = Node::Heading {
                level: 1,
                children: vec![child.clone()],
            };
            let node_h2 = Node::Heading {
                level: 2,
                children: vec![child.clone()],
            };
            let node_h3 = Node::Heading {
                level: 3,
                children: vec![child.clone()],
            };
            let node_h4 = Node::Heading {
                level: 4,
                children: vec![child.clone()],
            };
            let node_h5 = Node::Heading {
                level: 5,
                children: vec![child.clone()],
            };
            let node_h6 = Node::Heading {
                level: 6,
                children: vec![child.clone()],
            };
            let html_h1 = "<h1>CONTENT</h1>\n";
            let html_h2 = "<h2>CONTENT</h2>\n";
            let html_h3 = "<h3>CONTENT</h3>\n";
            let html_h4 = "<h4>CONTENT</h4>\n";
            let html_h5 = "<h5>CONTENT</h5>\n";
            let html_h6 = "<h6>CONTENT</h6>\n";
            assert_eq!(node_h1.to_html(), html_h1);
            assert_eq!(node_h2.to_html(), html_h2);
            assert_eq!(node_h3.to_html(), html_h3);
            assert_eq!(node_h4.to_html(), html_h4);
            assert_eq!(node_h5.to_html(), html_h5);
            assert_eq!(node_h6.to_html(), html_h6);
        }

        // Paragraph
        {
            let node = Node::Paragraph {
                children: vec![child.clone()],
            };
            let html = "<p>CONTENT</p>\n";
            assert_eq!(node.to_html(), html);
        }

        // List
        {
            let node_ul = Node::List {
                ordered: false,
                children: vec![child.clone()],
            };
            let node_ol = Node::List {
                ordered: true,
                children: vec![child.clone()],
            };
            let html_ul = "<ul>CONTENT</ul>\n";
            let html_ol = "<ol>CONTENT</ol>\n";
            assert_eq!(node_ul.to_html(), html_ul);
            assert_eq!(node_ol.to_html(), html_ol);
        }

        // List Item
        {
            let node = Node::ListItem {
                children: vec![child.clone()],
            };
            let html = "<li>CONTENT</li>\n";
            assert_eq!(node.to_html(), html);
        }

        // CodeBlock
        {
            let node = Node::CodeBlock {
                children: vec![child.clone()],
            };
            let html = "<code>CONTENT</code>\n";
            assert_eq!(node.to_html(), html);
        }

        // BlockQuote
        {
            let node = Node::BlockQuote {
                children: vec![child.clone()],
            };
            let html = "<blockquote>CONTENT</blockquote>\n";
            assert_eq!(node.to_html(), html);
        }

        // Bold
        {
            let node = Node::Bold {
                children: vec![child.clone()],
            };
            let html = "<strong>CONTENT</strong>";
            assert_eq!(node.to_html(), html);
        }

        // Italic
        {
            let node = Node::Italic {
                children: vec![child.clone()],
            };
            let html = "<em>CONTENT</em>";
            assert_eq!(node.to_html(), html);
        }

        // BoldItalic
        {
            let node = Node::BoldItalic {
                children: vec![child.clone()],
            };
            let html = "<strong><em>CONTENT</em></strong>";
            assert_eq!(node.to_html(), html);
        }

        // Link
        {
            let node = Node::Link {
                href: "HREF".into(),
                children: vec![child.clone()],
            };
            let html = "<a href=\"HREF\">CONTENT</a>";
            assert_eq!(node.to_html(), html);
        }
    }

    #[test]
    fn can_return_html_string_for_inline_nodes() {
        // Text
        {
            let node = Node::Text {
                content: "x".into(),
            };
            let html = "x";
            assert_eq!(node.to_html(), html);
        }

        // Code
        {
            let node = Node::Code {
                content: "x".into(),
            };
            let html = "<code>x</code>";
            assert_eq!(node.to_html(), html);
        }
    }

    #[test]
    fn can_return_html_string_for_special_nodes() {
        {
            let node = Node::Image {
                src: "SOURCE".into(),
                alt: "ALT".into(),
            };
            let html = "<img src=\"SOURCE\" alt=\"ALT\">\n";
            assert_eq!(node.to_html(), html);
        }
    }

    #[test]
    fn can_return_when_empty_for_structure_nodes() {
        // Document
        let document_node = Node::Document {
            children: vec![Node::Paragraph { children: vec![] }],
        };
        let document_node_empty = Node::Document { children: vec![] };
        assert!(!document_node.is_empty());
        assert!(document_node_empty.is_empty());

        // Heading
        let heading_node = Node::Heading {
            level: 1,
            children: vec![Node::Paragraph { children: vec![] }],
        };
        let heading_node_empty = Node::Heading {
            level: 1,
            children: vec![],
        };
        assert!(!heading_node.is_empty());
        assert!(heading_node_empty.is_empty());

        // Paragraph
        let paragraph_node = Node::Paragraph {
            children: vec![Node::Text {
                content: "hello".into(),
            }],
        };
        let paragraph_node_empty = Node::Paragraph { children: vec![] };
        assert!(!paragraph_node.is_empty());
        assert!(paragraph_node_empty.is_empty());

        // List
        let list_node = Node::List {
            ordered: false,
            children: vec![Node::ListItem {
                children: vec![Node::Text {
                    content: "item".into(),
                }],
            }],
        };
        let list_node_empty = Node::List {
            ordered: false,
            children: vec![],
        };
        assert!(!list_node.is_empty());
        assert!(list_node_empty.is_empty());

        // ListItem
        let list_item_node = Node::ListItem {
            children: vec![Node::Text {
                content: "item".into(),
            }],
        };
        let list_item_node_empty = Node::ListItem { children: vec![] };
        assert!(!list_item_node.is_empty());
        assert!(list_item_node_empty.is_empty());

        // CodeBlock
        let code_block_node = Node::CodeBlock {
            children: vec![Node::Text {
                content: "code".into(),
            }],
        };
        let code_block_node_empty = Node::CodeBlock { children: vec![] };
        assert!(!code_block_node.is_empty());
        assert!(code_block_node_empty.is_empty());

        // BlockQuote
        let block_quote_node = Node::BlockQuote {
            children: vec![Node::Text {
                content: "quote".into(),
            }],
        };
        let block_quote_node_empty = Node::BlockQuote { children: vec![] };
        assert!(!block_quote_node.is_empty());
        assert!(block_quote_node_empty.is_empty());

        // Bold
        let bold_node = Node::Bold {
            children: vec![Node::Text {
                content: "bold".into(),
            }],
        };
        let bold_node_empty = Node::Bold { children: vec![] };
        assert!(!bold_node.is_empty());
        assert!(bold_node_empty.is_empty());

        // Italic
        let italic_node = Node::Italic {
            children: vec![Node::Text {
                content: "italic".into(),
            }],
        };
        let italic_node_empty = Node::Italic { children: vec![] };
        assert!(!italic_node.is_empty());
        assert!(italic_node_empty.is_empty());

        // Bold Italic
        let bold_italic_node = Node::BoldItalic {
            children: vec![Node::Text {
                content: "bold italic".into(),
            }],
        };
        let bold_italic_node_empty = Node::BoldItalic { children: vec![] };
        assert!(!bold_italic_node.is_empty());
        assert!(bold_italic_node_empty.is_empty());

        // Link
        let link_node = Node::Link {
            href: "x".into(),
            children: vec![Node::Text {
                content: "link".into(),
            }],
        };
        let link_node_empty = Node::Link {
            href: "".into(),
            children: vec![],
        };
        assert!(!link_node.is_empty());
        assert!(link_node_empty.is_empty());
    }

    #[test]
    fn can_return_when_empty_for_inline_nodes() {
        // non-empty
        let text_node = Node::Text {
            content: "text".into(),
        };
        let code_node = Node::Code {
            content: "code".into(),
        };

        assert!(!text_node.is_empty());
        assert!(!code_node.is_empty());

        // empty
        let text_node_empty = Node::Text { content: "".into() };
        let code_node_empty = Node::Code { content: "".into() };

        assert!(text_node_empty.is_empty());
        assert!(code_node_empty.is_empty());
    }

    #[test]
    fn can_return_when_empty_for_special_nodes() {
        let image_node = Node::Image {
            src: "x".into(),
            alt: "x".into(),
        };
        let image_node_empty = Node::Image {
            src: "".into(),
            alt: "".into(),
        };
        assert!(!image_node.is_empty());
        assert!(image_node_empty.is_empty());
    }

    #[test]
    fn children_returns_some_for_structure_nodes() {
        let child = Node::Text {
            content: "x".into(),
        };

        // Document
        let doc = Node::Document {
            children: vec![child.clone()],
        };
        let doc_children = doc.children().expect("Document should have children");
        assert_eq!(doc_children.len(), 1);

        // Heading
        let heading = Node::Heading {
            level: 1,
            children: vec![child.clone()],
        };
        let heading_children = heading.children().expect("Heading should have children");
        assert_eq!(heading_children.len(), 1);

        // Paragraph
        let paragraph = Node::Paragraph {
            children: vec![child.clone()],
        };
        let paragraph_children = paragraph
            .children()
            .expect("Paragraph should have children");
        assert_eq!(paragraph_children.len(), 1);

        // List
        let list = Node::List {
            ordered: false,
            children: vec![child.clone()],
        };
        let list_children = list.children().expect("List should have children");
        assert_eq!(list_children.len(), 1);

        // ListItem
        let li = Node::ListItem {
            children: vec![child.clone()],
        };
        let li_children = li.children().expect("ListItem should have children");
        assert_eq!(li_children.len(), 1);

        // CodeBlock
        let code_block = Node::CodeBlock {
            children: vec![child.clone()],
        };
        let code_block_children = code_block
            .children()
            .expect("CodeBlock should have children");
        assert_eq!(code_block_children.len(), 1);

        // BlockQuote
        let bq = Node::BlockQuote {
            children: vec![child.clone()],
        };
        let bq_children = bq.children().expect("BlockQuote should have children");
        assert_eq!(bq_children.len(), 1);

        // Bold
        let bold = Node::Bold {
            children: vec![child.clone()],
        };
        let bold_children = bold.children().expect("BlockQuote should have children");
        assert_eq!(bold_children.len(), 1);

        // Italic
        let italic = Node::Italic {
            children: vec![child.clone()],
        };
        let italic_children = italic.children().expect("BlockQuote should have children");
        assert_eq!(italic_children.len(), 1);

        // Bold Italic
        let bold_italic = Node::BoldItalic {
            children: vec![child.clone()],
        };
        let bold_italic_children = bold_italic
            .children()
            .expect("BlockQuote should have children");
        assert_eq!(bold_italic_children.len(), 1);
    }

    #[test]
    fn children_returns_none_for_inline_nodes() {
        let text = Node::Text {
            content: "x".into(),
        };
        let code = Node::Code {
            content: "x".into(),
        };

        assert!(text.children().is_none());
        assert!(code.children().is_none());
    }

    #[test]
    fn children_returns_none_for_special_nodes() {
        let image = Node::Image {
            src: "x".into(),
            alt: "x".into(),
        };
        assert!(image.children().is_none());
    }

    #[test]
    fn add_child_succeeds_for_structure_nodes() {
        let child = Node::Text {
            content: "x".into(),
        };

        // Document
        {
            let mut node = Node::Document { children: vec![] };
            node.add_child(child.clone());
            let len = node.children().map(|c| c.len()).unwrap_or(0);
            assert_eq!(len, 1, "Document should have 1 child");
        }

        // Heading
        {
            let mut node = Node::Heading {
                level: 1,
                children: vec![],
            };
            node.add_child(child.clone());
            let len = node.children().map(|c| c.len()).unwrap_or(0);
            assert_eq!(len, 1, "Heading should have 1 child");
        }

        // Paragraph
        {
            let mut node = Node::Paragraph { children: vec![] };
            node.add_child(child.clone());
            let len = node.children().map(|c| c.len()).unwrap_or(0);
            assert_eq!(len, 1, "Paragraph should have 1 child");
        }

        // List
        {
            let mut node = Node::List {
                ordered: false,
                children: vec![],
            };
            node.add_child(child.clone());
            let len = node.children().map(|c| c.len()).unwrap_or(0);
            assert_eq!(len, 1, "List should have 1 child");
        }

        // ListItem
        {
            let mut node = Node::ListItem { children: vec![] };
            node.add_child(child.clone());
            let len = node.children().map(|c| c.len()).unwrap_or(0);
            assert_eq!(len, 1, "ListItem should have 1 child");
        }

        // CodeBlock
        {
            let mut node = Node::CodeBlock { children: vec![] };
            node.add_child(child.clone());
            let len = node.children().map(|c| c.len()).unwrap_or(0);
            assert_eq!(len, 1, "CodeBlock should have 1 child");
        }

        // BlockQuote
        {
            let mut node = Node::BlockQuote { children: vec![] };
            node.add_child(child.clone());
            let len = node.children().map(|c| c.len()).unwrap_or(0);
            assert_eq!(len, 1, "BlockQuote should have 1 child");
        }

        // Bold
        {
            let mut node = Node::Bold { children: vec![] };
            node.add_child(child.clone());
            let len = node.children().map(|c| c.len()).unwrap_or(0);
            assert_eq!(len, 1, "Bold should have 1 child");
        }

        // Italic
        {
            let mut node = Node::Italic { children: vec![] };
            node.add_child(child.clone());
            let len = node.children().map(|c| c.len()).unwrap_or(0);
            assert_eq!(len, 1, "Italic should have 1 child");
        }

        // Bold Italic
        {
            let mut node = Node::BoldItalic { children: vec![] };
            node.add_child(child.clone());
            let len = node.children().map(|c| c.len()).unwrap_or(0);
            assert_eq!(len, 1, "BoldItalic should have 1 child");
        }

        // Link
        {
            let mut node = Node::Link {
                href: "x".into(),
                children: vec![],
            };
            node.add_child(child.clone());
            let len = node.children().map(|c| c.len()).unwrap_or(0);
            assert_eq!(len, 1, "Link should have 1 child");
        }
    }

    #[test]
    #[should_panic(expected = "Can't add child to this node type.")]
    fn add_child_panics_for_text_node() {
        let child = Node::Text {
            content: "x".into(),
        };

        let mut text = Node::Text {
            content: "x".into(),
        };

        text.add_child(child.clone());
    }

    #[test]
    #[should_panic(expected = "Can't add child to this node type.")]
    fn add_child_panics_for_code_node() {
        let child = Node::Text {
            content: "x".into(),
        };

        let mut code = Node::Code {
            content: "x".into(),
        };

        code.add_child(child.clone());
    }

    #[test]
    #[should_panic(expected = "Can't add child to this node type.")]
    fn add_child_panics_for_image_node() {
        let child = Node::Text {
            content: "x".into(),
        };
        let mut image = Node::Image {
            src: "x".into(),
            alt: "x".into(),
        };

        image.add_child(child.clone());
    }
}
