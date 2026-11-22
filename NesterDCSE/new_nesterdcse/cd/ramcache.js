
function ramCache(dirName, ramPrefix)
{
	if (!ramPrefix)
		ramPrefix = "";
	dirName += "/";
	var fileList = FileSystem.getFileList(dirName);
	for (var k = 0; k < fileList.length; ++k)
	{
		var temp = fileList[k];
		if (!temp.isDir)
			FileSystem.copy(dirName+temp.name, "/ram/"+ramPrefix+temp.name);
	}
	return true;
}

