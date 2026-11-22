
includeOnce("/cd/colors.js");

function trimTrailingSlash(str)
{
	if (str != "/" && str.charAt(str.length-1) == "/")
		str = str.substr(0, str.length-1);
	return str;
}

function FileBrowser(startDir)
{	
	//public
	this.curDir = null;
	this.itemColor = g_colors["white"];
	this.selectedColor = g_colors["yellow"];
	this.dirColor = g_colors["red"];
	this.outlineColor = g_colors["black"];
	this.lowestDir = "/";
	this.fontSize = 20;
	this.lineSpacing = 5;
	this.result = null;
	this.visibleRowCount = 10;
	
	//private
	this.curItem = 0;
	this.firstVisibleItem = 0;
	this.dirItems = new Array();
	this.startDir = null;
	this.trig_buildup -= 0;
	this.trig_buildup += 0;
	this.child = null;
	this.scrollCount = 0;
	this.drawCount = 0;
	this.scrollDir = 1;
	
	if (startDir)
		this.startBrowse(startDir);
}

FileBrowser.prototype.startBrowse = function(startDir)
{
	startDir = trimTrailingSlash(startDir);
	if (startDir != this.curDir)
	{
		this.curDir = startDir;
		this.dirItems = FileSystem.getFileList(this.curDir);
	}
	this.result = null;
	this.child = null;
}

FileBrowser.prototype.processInput = function()
{
	var visHalf = Math.floor(this.visibleRowCount/2);
	if (this.child)
	{
		if (this.child.processInput())
		{
			this.result = this.child.result;
			return true;
		}
		else
		{
			if (this.child.result == -2)
				this.child = null;
			return false;
		}
	}
	var pad = Input.controller[0];
	if (pad.justPressed("A"))
	{
		if (this.dirItems[this.curItem].isDir)
		{
			this.child = new FileBrowser(this.curDir+"/"+this.dirItems[this.curItem].name);
			this.child.itemColor = this.itemColor;
			this.child.selectedColor = this.selectedColor;
			this.child.dirColor = this.dirColor;
			this.child.outlineColor = this.outlineColor;
			this.child.lowestDir = this.lowestDir;
			this.child.fontSize = this.fontSize;
			this.child.lineSpacing = this.lineSpacing;
			this.child.visibleRowCount = this.visibleRowCount;
			return false;
		}
		else
		{
			this.result = this.curDir+"/"+this.dirItems[this.curItem].name;
			return true;
		}
	}
	else if (pad.justPressed("B"))
	{
		this.result = -2;
		return false;
	}
	else if (pad.justPressed("Y"))
	{
		this.result = null;
		return true;
	}
	
	this.trig_buildup -= pad.LTRIG;
	this.trig_buildup += pad.RTRIG;
	if (pad.justPressed("DOWN") || this.trig_buildup > 254)
	{
		if (this.curItem < (this.dirItems.length - 1))
		{
			this.curItem++;
			this.scrollCount = 0;
			this.drawCount = 0;
			if (this.curItem > visHalf && this.curItem < (this.dirItems.length - (visHalf-1)))
				this.firstVisibleItem++;
		}
		if (this.trig_buildup > 254)
			this.trig_buildup -= 254;
	}
	if (pad.justPressed("UP") || this.trig_buildup < -254)
	{
		if (this.curItem > 0)
		{
			this.curItem--;
			this.scrollCount = 0;
			this.drawCount = 0;
			if (this.curItem < (this.dirItems.length - visHalf) && this.curItem > (visHalf-1))
				this.firstVisibleItem--;
		}
		if (this.trig_buildup < -254)
			this.trig_buildup += 254;
	}
	return false;
}

FileBrowser.prototype.draw = function(x, y, z, width, height)
{
	if (this.child)
	{
		this.child.draw(x, y, z, width, height);
		return;
	}
	var cury = y+this.fontSize;
	for (var k = this.firstVisibleItem; k < this.dirItems.length && k < (this.firstVisibleItem + this.visibleRowCount); ++k)
	{
		var tempColor;
		if (this.curItem == k)
			tempColor = this.selectedColor;
		else if (this.dirItems[k].isDir)
			tempColor = this.dirColor;
		else
			tempColor = this.itemColor;
		
		var tempstr = this.dirItems[k].name;
		if (this.curItem == k)
		{
			if (this.scrollCount > 0)
				tempstr = tempstr.substr(this.scrollCount);
			var bFit = main_font.drawWithBorder(tempstr, x, cury, z, width, this.fontSize, tempColor, this.outlineColor);
			if (this.drawCount >= 60 && (this.drawCount % 30) == 0)
			{
				if (!bFit)
					this.scrollCount += this.scrollDir;
				else if (this.scrollCount > 0)
				{
					this.scrollDir = -1;
					this.scrollCount--;
				}
				if (this.scrollCount == 0)
					this.scrollDir = 1;
			}
		}
		else
			main_font.drawWithBorder(tempstr, x, cury, z, width, this.fontSize, tempColor, this.outlineColor);
		cury += this.fontSize + this.lineSpacing;
	}
	this.drawCount++;
}
