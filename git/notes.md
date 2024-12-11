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

If you follow every step outlined in [Git internals](https://git-scm.com/book/en/v2/Git-Internals-Git-Objects), you'll end up with a tree that contains only SHA-1 information. This is not ideal because humans have some difficulty remembering sequences like (fdf4fc3344e67ab068f836878b6c4951e3b15f3d), and another major issue is that thres's no information about who created the tree or when it was created. To solve these problems "commits" were invented.

-------

To implement git, we must follow a specific sequence to achieve the minimal featuers that git provides. these features include:

1. Cat-file
2. Hash-file

# Cat-file
This feature allows you to read the file content of a specific compressed file. Since git works similar to a file manager, every SHA1 hash stored in the .git/objects folder can represent a file, a folder (AKA "tree"), or a commit.
The main ideia can be found in the code in this folder. It starts on the *cat_file* method.

# Hash-file
To store files without naming conflicts, git hashes the file content (while also compressing it) and saves it in the .git/objects folder. this logic is straightfoward and works as follows:

1. Get the file content and calculate its length
2. Create a header (composed of (blob|tree|commit) CONTENT_LENGTH+\0+CONTENT)
3. Calculate SHA1 of the previous step
4. Compress (using Zlib) everything from step 2
5. Save the content of step 4 in a file inside .git/objects/STEP_3_SHA1

# Ls-tree
When we create hash-files we need to save them inside a "folder", as we mentioned earlier, this folder is called a "tree". To read the content of this tree we use the _ls-tree_ command.

There are 4 possible outputs within a tree:

- Executable files (100755)
- Folders (040000)
- Regular files (100644)
- Symbolic links (120000)

Each code in these lines represents the type of file, similar to UNIX systems.

# Write-tree
In real git **write-tree** works over a staging area, as in "BuildingYourOwnX" they skip this part I'll skip it as well. The main goal of _write-tree_ is create a sequence of folders and files that belongs to a tree, this three is equivalent to a "folder" that we know in OS (similar to the explanation at the beginning). For your code we'll assume that everything inside _./custom_git_ excution folder represents the staging area

# Commit-tree
This is the _commit_ command without all the convinience

# Clone
CodeCrafters says that this is the most challange part of the software so far. Reading the [git documentation](https://git-scm.com/docs/http-protocol) it tell us some important information

## General Request Processing
All the sides (client and server) MUST support HTTP1.0 and HTTP1.1, the main communication will occur in HTTP.
If a file do not exists the server must return _404 Not Found_ or _410 Gone_, it is not default to return 200. The system should work as a OS, so, if the user requires a file that does not have permission to read it the server must return _403 Forbidden_

## Discovering References
The HTTP Clients must at first discover all the references available in that repo, to know this the client must reach the file present in **$GIT_URL/info/refs**. There's a difference between _dumb_ and _smart_ clients. Dumb clients has some particallarities that smart dont, for more details checkout [git documentation](https://git-scm.com/docs/http-protocol#_discovering_references). Its in Reference discovery phase that git compares client most recent _ref_ with server most recent ref, if those refs doesnt match it request (or pushes) new data

# Capabilities
In that phase, the main goal is discovery how the server and the client can communicate with each other. Here we use the same logic as when creating a "hash-file", I mean SHA1 _ref-name_\0CAPABILITIES. After discovery the server capability, client and server tries to determine the minimal packfile required

### Compile project
To compile all the code in ".src/" folder use this comand:

> gcc src/*.c -o custom_git -lz -lcrypto -lcurl