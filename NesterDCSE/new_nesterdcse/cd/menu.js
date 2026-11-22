
includeOnce("/cd/colors.js");

var BLANK_LINE = null;

function Menu()
{
	//functions
	this.draw = _Menu_draw;
	this.processInput = _Menu_processInput;
	this.setItems = _Menu_setItems;
	this.setColorRule = _Menu_setColorRule;
	
	//public
	this.itemColor = g_colors["white"];
	this.selectedColor = g_colors["yellow"];
	this.outlineColor = g_colors["black"];
	this.fontSize = 20;
	this.lineSpacing = 5;
	this.visibleRowCount = 10;
	this.enabled = true;
	this.iconWidth = -1;
	this.iconColor = null;
	
	//private
	this.curItemIndex = 0;
	this.firstVisibleItem = 0;
	this.curItem = null;
	this.items = new Array();
	this.trig_buildup -= 0;
	this.trig_buildup += 0;
	this.scrollCount = 0;
	this.drawCount = 0;
	this.scrollDir = 1;
	this.colorRules = new Array();
}

function _Menu_setColorRule(color, property, value)
{
	this.colorRules[this.colorRules.length] = {c:color, prop:property, val:value};
}

function _Menu_setItems(items)
{
	this.items = items;
	this.curItemIndex = 0;
	this.firstVisibleItem = 0;
	this.scrollCount = 0;
	this.trig_buildup -= 0;
	this.trig_buildup += 0;
	this.scrollCount = 0;
	this.drawCount = 0;
	this.scrollDir = 1;
}

function _Menu_processInput(pad)
{
	if (this.items.length == 0)
		return;
	if (!pad)
		pad = Input.controller[0];
	var visHalf = Math.floor(this.visibleRowCount/2);
	this.trig_buildup -= pad.LTRIG;
	this.trig_buildup += pad.RTRIG;
	if (pad.justPressed("DOWN") || this.trig_buildup > 254)
	{
		if (this.curItemIndex < (this.items.length - 1))
		{
			this.curItemIndex++;
			this.scrollCount = 0;
			this.drawCount = 0;
			if (this.curItemIndex > visHalf && this.curItemIndex < (this.items.length - (visHalf-1)))
				this.firstVisibleItem++;
		}
		if (this.trig_buildup > 254)
			this.trig_buildup -= 254;
	}
	if (pad.justPressed("UP") || this.trig_buildup < -254)
	{
		if (this.curItemIndex > 0)
		{
			this.curItemIndex--;
			this.scrollCount = 0;
			this.drawCount = 0;
			if (this.curItemIndex < (this.items.length - visHalf) && this.curItemIndex > (visHalf-1))
				this.firstVisibleItem--;
		}
		if (this.trig_buildup < -254)
			this.trig_buildup += 254;
	}
	this.curItem = this.items[this.curItemIndex];
}

function _Menu_draw(x, y, z, width, height)
{
	var iconX = x;
	if (this.iconWidth > 0)
	{
		x += this.iconWidth;
		width -= this.iconWidth;
	}
	
	var cury = y+this.fontSize;
	for (var k = this.firstVisibleItem; k < this.items.length && k < (this.firstVisibleItem + this.visibleRowCount); ++k)
	{
		if (this.items[k] != BLANK_LINE)
		{
			var tempColor;
			if (this.curItemIndex == k && this.enabled)
				tempColor = this.selectedColor;
			else
			{
				var n = 0;
				for (; n < this.colorRules.length; ++n)
				{
					var temprule = this.colorRules[n];
					if (this.items[k][temprule.prop] && this.items[k][temprule.prop] == temprule.val)
					{
						tempColor = temprule.c;
						break;
					}
				}
				if (n == this.colorRules.length)
					tempColor = this.itemColor;
			}

			var tempstr = this.items[k].name;
			if (this.curItemIndex == k)
			{
				if (this.scrollCount > 0)
					tempstr = tempstr.substr(this.scrollCount);
				var bFit = main_font.draw(tempstr, x, cury, z, width, this.fontSize, tempColor);
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
				main_font.draw(tempstr, x, cury, z, width, this.fontSize, tempColor);
			if (this.items[k].iconImg)
			{
				var tempIconColor = this.iconColor;
				if (!tempIconColor)
					tempIconColor = g_colors["white"];
				this.items[k].iconImg.draw(iconX, cury-(this.fontSize-5), 1, tempIconColor, z);
			}
		}
		cury += this.fontSize + this.lineSpacing;
	}
	this.drawCount++;
}
