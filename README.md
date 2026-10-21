
# MP3 Tag Reader & Editor

## 👨‍💻 Author Details

- **Name:** C Reddy Vasantha Narasimha Reddy
- **Email:** [crvnreddy48@gmail.com](mailto:crvnreddy48@gmail.com)
- **LinkedIn:** [C Reddy Vasantha Narasimha Reddy](https://www.linkedin.com/in/reddy-vasantha-narasimha-reddy-c-690279352/)
- **Programming Language:** C
- **Batch ID:** 26015_128

---

## 📌 Project Description

MP3 Tag Reader & Editor is a C-based application developed to read, display, and modify metadata embedded in MP3 audio files. This metadata, known as ID3 tags, contains information such as song title, artist, album, release year, genre, track number, comments, and cover art.

The application enables users to view existing metadata and edit selected tag values. It uses binary file handling and file-processing techniques to access and update metadata while preserving the remaining tags and audio content where supported.

## ✨ Features

### 📖 View Mode
- Read and display MP3 metadata.
- Display song title, artist, album, year, and genre.
- Read supported ID3v1 and ID3v2 tags.
- Display additional supported text frames.
- Show picture information when supported.

### ✏️ Edit Mode
- Select a metadata field to modify.
- Update the selected tag value.
- Process metadata frames and their sizes.
- Preserve other tags and audio data where supported.

## 🏷️ ID3 Tag Formats

MP3 files may contain two types of ID3 tags:

- **ID3v1:** A fixed 128-byte metadata block located at the end of the file.
- **ID3v2:** A variable-size, frame-based metadata block located at the beginning of the file.

ID3v2 versions include 2.2, 2.3, and 2.4, which differ in frame identifiers and size encoding.

## 🛠️ Technologies Used

- **Language:** C
- **Compiler:** GCC
- **Editor:** Visual Studio Code
- **File Handling:** Standard C file I/O operations
- **Concepts:** Structures, functions, pointers, arrays, strings, bitwise operations, and binary file processing

## 📂 Project Structure

```text
MP3-Tag-Reader-Editor/
├── main.c
├── view.c
├── view.h
├── edit.c
├── edit.h
└── README.md
```

*Update this structure to match the actual files in your repository.*

## ⚙️ Compilation

Compile the program using GCC. For example:

```bash
gcc main.c view.c edit.c -o mp3reader
```

Ensure that all required source files are included in the compilation command.

## ▶️ Execution

On Linux:

```bash
./mp3reader
```

On Windows:

```bash
mp3reader.exe
```

Follow the on-screen menu to view or edit MP3 metadata.

## 💻 Sample Output

```text
MP3 Tag Reader & Editor

1. View MP3 Tags
2. Edit MP3 Tags
3. Exit

Enter your choice: 1
Enter MP3 file name: song.mp3

Title  : Example Song
Artist : Example Artist
Album  : Example Album
Year   : 2026
Genre  : Pop
```

*Note: This output is illustrative. Actual output depends on the program implementation and the selected MP3 file.*

## 🎯 Project Objectives

- To understand the structure of MP3 files and ID3 metadata.
- To implement metadata reading and editing operations.
- To gain practical experience in C file handling.
- To understand binary data processing and frame-size management.
- To develop modular and reusable C code.

## 🚀 Future Enhancements

- Support for additional ID3 versions and metadata frames.
- Improved validation for corrupted or invalid MP3 files.
- Enhanced error handling and user interaction.
- Support for additional metadata fields.

## 📚 Learning Outcomes

This project provides practical experience in C programming, binary file handling, string manipulation, modular programming, and metadata processing. It also helps develop an understanding of how information is stored and modified within multimedia files.

## 🤝 Connect With Me

- **Email:** [crvnreddy48@gmail.com](mailto:crvnreddy48@gmail.com)
- **LinkedIn:** [Visit My LinkedIn Profile](https://www.linkedin.com/in/reddy-vasantha-narasimha-reddy-c-690279352/)

---

⭐ If you find this project interesting, feel free to explore the repository.

**Developed by C Reddy Vasantha Narasimha Reddy**
