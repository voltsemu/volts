Set args = Wscript.Arguments

Set input = args.Item(0)
Set output = args.Item(1)

Set fso = CreateObject("Scripting.FileSystemObject")

If NOT fso.FolderExists(output) Then
    fso.CreateFolder(output)
End If

Set objShell = CreateObject("Shell.Application")

Set files = objShell.NameSpace(input).items

objShell.NameSpace(output).CopyHere(files)

Set objShell = Nothing
Set files = Nothing