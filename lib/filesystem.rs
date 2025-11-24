use std::io::Write;
use std::{fs, io};
use std::path::Path;

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

#[cfg(test)]
mod filesystem_tests {
    use std::fs;
    use super::Filesystem;

    #[test]
    fn reads_file() {
        match Filesystem::read_file("./test/filesystem_test.md") {
            Ok(s) => assert_eq!(s, "DO NOT DELETE. Used in filesystem.rs tests.\n"),
            Err(err) => unreachable!("{}", err)
        }
    }

    #[test]
    fn writes_file() {
        let path = String::from("./test/filesystem_test_output.md");
        let content = String::from("TESTING OUTPUT");
        match Filesystem::write_file(&path, &content) {
            Ok(_) =>  {
                match fs::read_to_string(&path) {
                    Ok(s) => assert_eq!(s, content),
                    Err(err) => unreachable!("{}", err),
                }
            },
            Err(err) => unreachable!("{}", err)
        }
    }
}
