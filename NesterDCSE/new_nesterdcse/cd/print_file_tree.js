function PrintFileTree(strdir, tabcount)
{
	var oldcwd = FileSystem.getCWD();
	if (!tabcount)
		tabcount = 0;
	FileSystem.setCWD(strdir);
	var thingy = FileSystem.getFileList();
	for (var k = 0; k < thingy.length; ++k)
	{
		if (thingy[k].name == "pc")
			continue;
		for (var n = 0; n < tabcount; ++n)
			print("\t");
		print(thingy[k].name + "\n");
		if (thingy[k].isDir || strdir == "/")
			PrintFileTree(thingy[k].name, tabcount+1);
	}
	FileSystem.setCWD(oldcwd);
}

