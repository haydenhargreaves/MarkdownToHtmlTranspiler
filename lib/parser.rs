use crate::node::Node;

#[derive(Debug)]
pub struct Parser {
    content: String
}

impl Parser {
    /// Create a new parser object with the content attached. This does not take ownership of the
    /// string provided and therefore dies with the string. The input string is normalized to
    /// support operation on all operating systems.
    pub fn new(content: &str) -> Self {
        let normalized = content.replace("\r\n", "\n").replace("\r", "");
        Self { content: normalized }
    }

    pub fn parse_document(&self) -> Node {
        let chars = self.content.chars().peekable();

        Node::Document { children: vec![
            Node::Paragraph { children: vec![
                Node::Text { content: chars.collect() },
            ]} 
        ]}
    }
}
