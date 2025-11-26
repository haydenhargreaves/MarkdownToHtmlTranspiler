use transpiler::filesystem::Filesystem;
use transpiler::parser::Parser;

pub fn main() -> Result<(), Box<dyn std::error::Error>> {
    let file = Filesystem::read_file("./test.md");
    let content;
    match file {
        Ok(s) => content = s,
        Err(err) => panic!("Failed to read file. {}", err),
    }

    let parser = Parser::new(&content);
    let node = parser.parse_document();

    match Filesystem::write_file("./output.html", &node.to_html()) {
        Ok(_) => println!("Input document parsed and output written."),
        Err(err) => panic!("Failed to write to output. {}", err),
    }

    // Return a value to meet the main function requirements
    Ok(())
}
