use std::io::Write;
use std::{fs, io, usize};
use std::path::Path;

pub enum Node {
    // Structure Nodes
    Document { children: Vec<Node> },
    Heading { level: u8, children: Vec<Node> },
    Paragraph { children: Vec<Node> },
    List { ordered: bool, children: Vec<Node> },
    ListItem { children: Vec<Node> },
    CodeBlock { children: Vec<Node> },
    BlockQuote { children: Vec<Node> },
    ImageNode { src: String, alt: String },

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
            Node::ImageNode { src, alt } => format!("<img src=\"{}\" alt=\"{}\">\n", src, alt),

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
            Node::ImageNode { src, alt } => src.is_empty() && alt.is_empty(),
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
            | Node::List { ordered: _, children, }
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
            Node::Link { href: _, content: _ } => None,
            Node::ImageNode { src: _, alt: _ } => None,
        }
    }

    /// Add a child to the back of the list of children. If the node is a type which does not allow
    /// children to be added, this function will panic.
    ///
    /// Example INVALID usage:
    ///
    /// ```rust
    /// // Attempting to add an inline node as a child of another inline node.
    /// let mut inline = Node::Text { content: String::from("Hello world") };
    /// let inline2 = Node::Bold { content: String::from(" bolded text") };
    /// inline.add_child(inline2); // Will panic! 'Can't add child to this node type.'
    /// ```
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
            Node::Link { href: _, content: _ } => panic!("Can't add child to this node type."),
            Node::ImageNode { src: _, alt: _ } => panic!("Can't add child to this node type."),
        };
    }
}

pub struct Parser {
    content: String,
    position: usize,
}

impl Parser {
    pub fn new(content: String) -> Self {
        // Normalize the input
        let normalized = content.replace("\r\n", "\n").replace("\r", "");
        Self { content: normalized, position: 0 }
    }

    pub fn parse_document(&self) -> Node {
        let text = Node::Text { content: self.content.clone() };
        let heading = Node::Heading { level: 1, children: vec![text] };
        Node::Document { children: vec![ heading ] }
    }

    fn peek(&self, offset: usize) -> u8 {

    }
}

pub struct Filesystem;

impl Filesystem {
    /// This uses a generic: `<P: AsRef<Path>>` to allow for easy use with any type that can be
    /// converted into a &Path. Allowing String types, &str types and anything that can be
    /// converted into a Path type, and then borrowed. There is no run time cost of doing this
    /// either, so it is very fast and very elegant.
    pub fn read_file<P: AsRef<Path>>(path: P) -> io::Result<String> {
        fs::read_to_string(path)
    }

    pub fn write_file<P: AsRef<Path>>(path: P, contents: &str) -> io::Result<()> {
        let mut file = fs::File::create(path)?;
        file.write_all(contents.as_bytes())
    }
}

pub fn main() -> Result<(), Box<dyn std::error::Error>> {
    let mut root = Node::Document {
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

    let p = Node::Paragraph {
        children: vec![Node::Text {
            content: String::from("This is a paragraph tag."),
        }],
    };

    root.add_child(p);

    let parser = Parser::new(String::from("Hello world\r\nhello\n"));
    let node = parser.parse_document();

    let content = Filesystem::read_file("./test/journal.md");
    match content {
        Ok(s) => Filesystem::write_file("./copied.md", &s)?,
        Err(err) => panic!("Failed to read file. {}", err)
    }

    println!("{}", node.to_html());

    // Return a value to meet the main function requirements
    Ok(())
}
