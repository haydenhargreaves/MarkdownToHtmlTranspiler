pub enum Node {
    // Structure Nodes
    Document { children: Vec<Node> },
    Heading { level: u8, children: Vec<Node> },
    Paragraph { children: Vec<Node> },
    List { ordered: bool, children: Vec<Node> },
    ListItem { children: Vec<Node> },
    CodeBlock { children: Vec<Node> },
    BlockQuote { children: Vec<Node> },

    // Inline Nodes
    Text { content: String },
    Bold { content: String },
    Italic { content: String },
    BoldItalic { content: String },
    Code { content: String },
    Link { href: String, content: String },
}

impl Node {
    pub fn to_html(&self) -> String {
        match self {
            // Structure nodes
            Node::Document { children } => {
                let inner = children.iter().map(|x| x.to_html()).collect::<String>();
                format!(
                    "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n<meta charset=\"UTF-8\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n<title>Document</title>\n</head>\n<body>{}</body>\n</html>",
                    inner
                )
            }
            Node::Heading { level, children } => {
                let inner = children.iter().map(|x| x.to_html()).collect::<String>();
                format!("<h{level}>{}</h{level}>", inner, level = level)
            }
            Node::Paragraph { children } => {
                let inner = children.iter().map(|x| x.to_html()).collect::<String>();
                format!("<p>{}</p>", inner)
            }
            Node::List { ordered, children } => {
                let inner = children.iter().map(|x| x.to_html()).collect::<String>();
                let tag = if *ordered { "ol" } else { "ul" };
                format!("<{tag}>{}</{tag}>", inner, tag = tag)
            }
            Node::ListItem { children } => {
                let inner = children.iter().map(|x| x.to_html()).collect::<String>();
                format!("<li>{}</li>", inner)
            }
            Node::CodeBlock { children } => {
                let inner = children.iter().map(|x| x.to_html()).collect::<String>();
                format!("<code>{}</code>", inner)
            }
            Node::BlockQuote { children } => {
                let inner = children.iter().map(|x| x.to_html()).collect::<String>();
                format!("<blockquote>{}</blockquote>", inner)
            }

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
        }
    }

    /// Returns Some children if they exist, otherwise None will be returned. For nodes that do not
    /// have children, None will be returned.
    pub fn children(&self) -> Option<&[Node]> {
        match self {
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

            _ => None,
        }
    }

    /// Add a child to the back of the list of children. If the node is a type which does not allow
    /// children to be added, this function will panic.
    pub fn add_child(&mut self, child: Node) {
        match self {
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

            _ => panic!("Can't add child to this node type."),
        };
    }
}

pub fn main() {
    let root = Node::Document {
        children: vec![Node::Heading {
            level: 1,
            children: vec![
                Node::Text {
                    content: String::from("Heading level 1"),
                },
                Node::Bold {
                    content: String::from(" this part is bold"),
                },
                Node::Italic {
                    content: String::from(" this part is italic"),
                },
            ],
        }],
    };

    println!("{}", root.to_html())
}
