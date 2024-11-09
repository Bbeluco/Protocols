Yes, I know, Git is not a protocol.

At this point, this repo is mainly used to store my learnings about any fundamental software that I use daily. So because of this, I believe that this repository is an excellent place to put all my knowledge of Git :).

# Git
I use git frequently, this is required for my job and also helps me keep some version control in my personal projects. I decided to rewrite my own git (keep in mind that Linus wrote this in 1 day). Since it is a core software in development and I belive I must have a deeper understanding in that stack.

## How it works?
It works using a very logical schema. Every time you commit something, it basically gets all the content in your folder, creates a compression version (using ZLib) and stores it in _./git/objects_ folder. The name of that compressed folder is a SHA-1 based on all the content of your commit. This guarantee that you'll have a little chance of name conflicts.

You can use _git cat-file -p {SHA-1 HASH}_ to check file type
_git hash-content -w {YOUR FILE}_ is another basic way to create a save into your .git/objects folder (it is a primitive commit).

In this [link](https://git-scm.com/book/en/v2/Git-Internals-Git-Objects) you can find an operation similar to checkout an older commit. You basically access your content of type **blob** (binary large objects. It is equivalent to _content of a file_) by _git cat-file -p_ command and redirects its output to your original file

## Git objects
As we speak earlier about blob, it is a good time to introduce **git objects**. In git, we have three types of objects

1. Blob
2. Tree
3. Commit

### Blob
Blob is literally the content of the file. It don't contain anything else than the content, so, if you want to know the name of the file for example you wouldn't find it in blob. **The blob has the only goal to store the content**

### Tree
Trees are basically a directory list, it could contain another tree or a bunch of blobs. In trees you'll find informations like filename and references to another places.

### Commit
A commit is a snapshot of the tree in that especific time. It also contains some information like: author, message, initial commit, time, etc.