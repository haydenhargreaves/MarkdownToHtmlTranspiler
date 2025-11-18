/// This is an old code example, moving to using enums now

pub trait Node {
    fn to_html(&self) -> String;
    fn is_empty(&self) -> bool;

    fn add_child(&mut self, _: Box<dyn Node>) {
        panic!("Cannot add children to this node.")
    }

    fn children(&self) -> Option<&[Box<dyn Node>]> {
        None
    }
}

pub struct DocumentNode {
    pub children: Vec<Box<dyn Node>>,
}

pub struct HeadingNode {
    pub size: u8,
    pub children: Vec<Box<dyn Node>>,
}

pub struct ParagraphNode {
    pub children: Vec<Box<dyn Node>>,
}

impl Node for DocumentNode {
    fn to_html(&self) -> String {
        let inner_html = self
            .children
            .iter()
            .map(|child| child.to_html())
            .collect::<String>();
        format!(
            "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n<meta charset=\"UTF-8\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n<title>Document</title>\n</head>\n<body>{}</body>\n</html>",
            inner_html
        )
    }

    fn is_empty(&self) -> bool {
        self.children.is_empty()
    }

    fn add_child(&mut self, child: Box<dyn Node>) {
        self.children.push(child);
    }

    fn children(&self) -> Option<&[Box<dyn Node>]> {
        Some(&self.children)
    }
}

impl Node for HeadingNode {
    fn to_html(&self) -> String {
        let inner_html = self
            .children
            .iter()
            .map(|child| child.to_html())
            .collect::<String>();
        format!("<h{}>{}</h{}>", self.size, inner_html, self.size)
    }

    fn is_empty(&self) -> bool {
        self.children.is_empty()
    }

    fn add_child(&mut self, child: Box<dyn Node>) {
        self.children.push(child);
    }

    fn children(&self) -> Option<&[Box<dyn Node>]> {
        Some(&self.children)
    }
}

impl Node for ParagraphNode {
    fn to_html(&self) -> String {
        let inner_html = self
            .children
            .iter()
            .map(|child| child.to_html())
            .collect::<String>();
        format!("<p>{}</p>", inner_html)
    }

    fn is_empty(&self) -> bool {
        self.children.is_empty()
    }

    fn add_child(&mut self, child: Box<dyn Node>) {
        self.children.push(child);
    }

    fn children(&self) -> Option<&[Box<dyn Node>]> {
        Some(&self.children)
    }
}
