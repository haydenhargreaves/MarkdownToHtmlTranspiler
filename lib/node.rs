#[derive(Debug, Clone)]
pub enum Node {
    // Structure Nodes
    Document { children: Vec<Node> },
    Heading { level: u8, children: Vec<Node> },
    Paragraph { children: Vec<Node> },
    List { ordered: bool, children: Vec<Node> },
    ListItem { children: Vec<Node> },
    CodeBlock { children: Vec<Node> },
    BlockQuote { children: Vec<Node> },
    Image { src: String, alt: String },

    // Inline Nodes
    Text { content: String },
    Bold { content: String },
    Italic { content: String },
    BoldItalic { content: String },
    Code { content: String },
    Link { href: String, content: String },
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
            Node::Image { src, alt } => format!("<img src=\"{}\" alt=\"{}\">\n", src, alt),

            // Inline nodes
            Node::Text { content } => format!("{}", content),
            Node::Bold { content } => format!("<strong>{}</strong>", content),
            Node::Italic { content } => format!("<em>{}</em>", content),
            Node::BoldItalic { content } => format!("<strong><em>{}</em></strong>", content),
            Node::Code { content } => format!("<code>{}</code>", content),
            Node::Link { href, content } => format!("<a href=\"{}\">{}</a>", href, content),
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
            | Node::BlockQuote { children } => children.is_empty(),

            // Inline nodes
            Node::Text { content }
            | Node::Bold { content }
            | Node::Italic { content }
            | Node::BoldItalic { content }
            | Node::Code { content } => content.is_empty(),

            // Special rules
            Node::Link { href, content } => content.is_empty() && href.is_empty(),
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
            | Node::BlockQuote { children } => Some(&children),

            // Inline Nodes
            Node::Text { content: _ }
            | Node::Bold { content: _ }
            | Node::Italic { content: _ }
            | Node::BoldItalic { content: _ }
            | Node::Code { content: _ } => None,

            // Special Nodes
            Node::Link {
                href: _,
                content: _,
            } => None,
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
            | Node::BlockQuote { children } => children.push(child),

            // Inline Nodes
            Node::Text { content: _ }
            | Node::Bold { content: _ }
            | Node::Italic { content: _ }
            | Node::BoldItalic { content: _ }
            | Node::Code { content: _ } => panic!("Can't add child to this node type."),

            // Special Nodes
            Node::Link {
                href: _,
                content: _,
            } => panic!("Can't add child to this node type."),
            Node::Image { src: _, alt: _ } => panic!("Can't add child to this node type."),
        };
    }
}

#[cfg(test)]
mod node_tests {
    use super::Node;

    #[test]
    fn can_return_when_empty() {
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
                content: String::from("hello"),
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
                    content: String::from("item"),
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
                content: String::from("item"),
            }],
        };
        let list_item_node_empty = Node::ListItem { children: vec![] };
        assert!(!list_item_node.is_empty());
        assert!(list_item_node_empty.is_empty());

        // CodeBlock
        let code_block_node = Node::CodeBlock {
            children: vec![Node::Text {
                content: String::from("code"),
            }],
        };
        let code_block_node_empty = Node::CodeBlock { children: vec![] };
        assert!(!code_block_node.is_empty());
        assert!(code_block_node_empty.is_empty());

        // BlockQuote
        let block_quote_node = Node::BlockQuote {
            children: vec![Node::Text {
                content: String::from("quote"),
            }],
        };
        let block_quote_node_empty = Node::BlockQuote { children: vec![] };
        assert!(!block_quote_node.is_empty());
        assert!(block_quote_node_empty.is_empty());

        // ImageNode (structure-ish but with fields)
        let image_node = Node::Image {
            src: String::from("src"),
            alt: String::from("alt"),
        };
        let image_node_empty = Node::Image {
            src: String::from(""),
            alt: String::from(""),
        };
        assert!(!image_node.is_empty());
        assert!(image_node_empty.is_empty());

        // Inline nodes: non-empty
        let text_node = Node::Text {
            content: String::from("text"),
        };
        let bold_node = Node::Bold {
            content: String::from("bold"),
        };
        let italic_node = Node::Italic {
            content: String::from("italic"),
        };
        let bold_italic_node = Node::BoldItalic {
            content: String::from("both"),
        };
        let code_node = Node::Code {
            content: String::from("code"),
        };
        let link_node = Node::Link {
            href: String::from("https://example.com"),
            content: String::from("link"),
        };

        assert!(!text_node.is_empty());
        assert!(!bold_node.is_empty());
        assert!(!italic_node.is_empty());
        assert!(!bold_italic_node.is_empty());
        assert!(!code_node.is_empty());
        assert!(!link_node.is_empty());

        // Inline nodes: empty
        let text_node_empty = Node::Text {
            content: String::from(""),
        };
        let bold_node_empty = Node::Bold {
            content: String::from(""),
        };
        let italic_node_empty = Node::Italic {
            content: String::from(""),
        };
        let bold_italic_node_empty = Node::BoldItalic {
            content: String::from(""),
        };
        let code_node_empty = Node::Code {
            content: String::from(""),
        };
        let link_node_empty = Node::Link {
            href: String::from(""),
            content: String::from(""),
        };

        assert!(text_node_empty.is_empty());
        assert!(bold_node_empty.is_empty());
        assert!(italic_node_empty.is_empty());
        assert!(bold_italic_node_empty.is_empty());
        assert!(code_node_empty.is_empty());
        assert!(link_node_empty.is_empty());
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
    }

    #[test]
    fn children_returns_none_for_inline_nodes() {
        let text = Node::Text {
            content: "x".into(),
        };
        let bold = Node::Bold {
            content: "x".into(),
        };
        let italic = Node::Italic {
            content: "x".into(),
        };
        let bolditalic = Node::BoldItalic {
            content: "x".into(),
        };
        let code = Node::Code {
            content: "x".into(),
        };

        assert!(text.children().is_none());
        assert!(bold.children().is_none());
        assert!(italic.children().is_none());
        assert!(bolditalic.children().is_none());
        assert!(code.children().is_none());
    }

    #[test]
    fn children_returns_none_for_special_nodes() {
        let link = Node::Link {
            href: "x".into(),
            content: "x".into(),
        };
        let image = Node::Image {
            src: "x".into(),
            alt: "x".into(),
        };

        assert!(link.children().is_none());
        assert!(image.children().is_none());
    }

    #[test]
    fn add_child_succeeds_for_structure_nodes() {
        let child = Node::Text {
            content: "x".into(),
        };

        {
            // Document
            let mut document = Node::Document { children: vec![] };
            document.add_child(child);
            if let Some(children) = document.children() {
                assert_eq!(children.len(), 1);
            } else {
                panic!("Document should have children");
            }
        }
    }

    #[test]
    #[should_panic(expected = "Can't add child to this node type.")]
    fn add_child_panics_for_inline_nodes() {
        let child = Node::Text {
            content: "x".into(),
        };

        let mut text = Node::Text {
            content: "x".into(),
        };
        let mut bold = Node::Bold {
            content: "x".into(),
        };
        let mut italic = Node::Italic {
            content: "x".into(),
        };
        let mut bolditalic = Node::BoldItalic {
            content: "x".into(),
        };
        let mut code = Node::Code {
            content: "x".into(),
        };

        text.add_child(child.clone());
        bold.add_child(child.clone());
        italic.add_child(child.clone());
        bolditalic.add_child(child.clone());
        code.add_child(child.clone());
    }

    #[test]
    #[should_panic(expected = "Can't add child to this node type.")]
    fn add_child_panics_for_special_nodes() {
        let child = Node::Text {
            content: "x".into(),
        };

        let mut link = Node::Link {
            href: "x".into(),
            content: "x".into(),
        };
        let mut image = Node::Image {
            src: "x".into(),
            alt: "x".into(),
        };

        link.add_child(child.clone());
        image.add_child(child.clone());
    }
}
