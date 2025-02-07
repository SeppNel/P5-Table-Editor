# P5-Table-Editor
Tool to edit files from Persona 5 for translation or whatever you want.
This is my firt project coding, so expect lots of bugs and strange behavior. The code is a mess and there is probably a better way to do all of this, feel free to suggest changes so i can learn.

Also some comments are in spanish, I should translate them later...

## GUI Version:
You can run the application to change lines by dropping your .table files to the application. After you opened your file, you should be able to change every sentence one by one. *This actually takes too much time so I prefer you to use CLI version.* In the end you can send the file using save button on bottom-right.

## CLI version:

    TableEditor.exe export ".table file" ".txt file"
    TableEditor.exe import ".table file" ".txt file"

### Export:
This command will just export your table file as txt you mentioned. Path could be empty or not, doesn't matter.

### Import:
This command will import your txt file inside the table file and than save it. Your .txt and .table file must be present.