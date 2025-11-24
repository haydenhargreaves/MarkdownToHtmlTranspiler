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
