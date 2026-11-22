
function OSKeyboard(color)
{
	this.bShift = 0;
	this.currentRow = 0
	this.currentChar = 0;
	this.current4 = 0;
	this.offsets = new Array(0, 0, 0, 0);
	this.setSpacing(34);
	this.onEnter = false;
	this.onBS = false;
	
	this.imgKB = new Image();
	this.imgKB.load("/cd/keyboard/keyboard.png", IMG_OPAQUE, IMG_FILTER_NONE, IMG_ALPHA_NONE);
	
	this.imgKBshift = new Image();
	this.imgKBshift.load("/cd/keyboard/keyboard_shift.png", IMG_OPAQUE, IMG_FILTER_NONE, IMG_ALPHA_NONE);
	
	//this.test = new TestPoint(0, 137);
}

OSKeyboard.keyboardChars = new Array(
	new Array(
		"1234567890-=\\",
		"qwertyuiop[]",
		"asdfghjkl;'",
		"zxcvbnm,./"
	),
	new Array(
		"!@#$%^&*()_+|",
		"QWERTYUIOP{}",
		"ASDFGHJKL:\"",
		"ZXCVBNM<>?"
	)
);

OSKeyboard.prototype.processInput = function()
{
	if (this.onBS)
	{
		if (pad1.justPressed("UP") || pad1.justPressed("DOWN") ||
			pad1.justPressed("LEFT") || pad1.justPressed("RIGHT"))
		{
			if (pad1.justPressed("UP"))
			{
				this.currentRow = 4;
				this.current4 = 2;
			}
			else if (pad1.justPressed("DOWN"))
				this.onEnter = true;
			else if (pad1.justPressed("LEFT"))
			{
				this.currentRow = 0;
				this.currentChar = 12;
			}
			else if (pad1.justPressed("RIGHT"))
			{
				this.currentRow = 0;
				this.currentChar = 0;
			}
			this.onBS = false;
		}
	}
	else if (this.onEnter)
	{
		if (pad1.justPressed("UP") || pad1.justPressed("DOWN") ||
			pad1.justPressed("LEFT") || pad1.justPressed("RIGHT"))
		{
			if (pad1.justPressed("UP"))
				this.onBS = true;
			else if (pad1.justPressed("DOWN"))
			{
				this.currentRow = 4;
				this.current4 = 2;
			}
			else if (pad1.justPressed("LEFT"))
			{
				this.currentRow = 2;
				this.currentChar = 10;
			}
			else if (pad1.justPressed("RIGHT"))
			{
				this.currentRow = 2;
				this.currentChar = 0;
			}
			this.onEnter = false;
		}
	}
	else
	{
		var prevRow = this.currentRow;
		if (pad1.justPressed("UP"))
			this.currentRow--;
		if (pad1.justPressed("DOWN"))
			this.currentRow++;

		if (this.currentRow < 0)
			this.currentRow = 4;
		if (this.currentRow > 4)
			this.currentRow = 0;

		if (this.currentRow != 4)
		{
			if (prevRow == 4)
				this.currentChar = this.current4*4;
			else
			{
				if (this.currentChar < 0)
					this.currentChar = OSKeyboard.keyboardChars[0][this.currentRow].length-1;
				if (this.currentChar >= OSKeyboard.keyboardChars[0][this.currentRow].length)
					this.currentChar = OSKeyboard.keyboardChars[0][this.currentRow].length-1;
			}

			if (pad1.justPressed("LEFT"))
				this.currentChar--;
			if (pad1.justPressed("RIGHT"))
				this.currentChar++;
			
			if (this.currentRow == 0)
			{
				if (this.currentChar < 0 || this.currentChar >= OSKeyboard.keyboardChars[0][0].length)
					this.onBS = true;
			}
			else if (this.currentRow == 1 || this.currentRow == 2)
			{
				if (this.currentChar < 0 || this.currentChar >= OSKeyboard.keyboardChars[0][this.currentRow].length)
					this.onEnter = true;
			}
			else
			{
				if (this.currentChar < 0)
					this.currentChar = OSKeyboard.keyboardChars[0][this.currentRow].length-1;
				if (this.currentChar >= OSKeyboard.keyboardChars[0][this.currentRow].length)
					this.currentChar = 0;
			}
		}
		else
		{
			if (prevRow != 4)
			{
				var additional = (prevRow==0) ? 1 : 0;
				if (this.currentChar < (3+additional))
					this.current4 = 0;
				else if (this.currentChar < (7+additional))
					this.current4 = 1;
				else if (this.currentChar >= (7+additional))
					this.current4 = 2;
			}
			else
			{
				if (pad1.justPressed("LEFT"))
					this.current4--;
				if (pad1.justPressed("RIGHT"))
					this.current4++;

				if (this.current4 < 0)
					this.current4 = 2;
				if (this.current4 > 2)
					this.current4 = 0;
			}
		}
	}
	
	if (pad1.justPressed("A"))
	{
		if (this.onBS)
			return "BS";
		if (this.onEnter)
			return "\n";
		if (this.currentRow == 4)
		{
			if (this.current4 == 0)
				this.bShift = (this.bShift==1) ? 0 : 1;
			else
			{
				this.bShift = 0;
				if (this.current4 == 1)
					return " ";
				else if (this.current4 == 2)
					return "DONE";
			}
		}
		else
		{
			var retval = OSKeyboard.keyboardChars[this.bShift][this.currentRow].charAt(this.currentChar);
			this.bShift = 0;
			return retval;
		}
	}
	
	//this.test.processInput();
	return null;
}

OSKeyboard.prototype.setSpacing = function(sp)
{
	this.spacing = sp;
	var maxRowLen = OSKeyboard.keyboardChars[0][0].length*sp;
	for (var k = 1; k < 4; ++k)
		this.offsets[k] = (maxRowLen-(OSKeyboard.keyboardChars[0][k].length*sp))/2;
}

OSKeyboard.prototype.draw = function(x, y)
{
	if (this.bShift)
		this.imgKBshift.draw(x, y, 1, g_colors["white"], 100);
	else
		this.imgKB.draw(x, y, 1, g_colors["white"], 100);
	if (this.onEnter)
	{
		Video.drawRect(x+433, y+37, x+473, y+101, 150, packColor(128, 0, 255, 0));
		Video.drawRect(x+416, y+71, x+433, y+101, 150, packColor(128, 0, 255, 0));
	}
	else if (this.onBS)
	{
		Video.drawRect(x+445, y+1, x+477, y+33, 150, packColor(128, 0, 255, 0));
	}
	else
	{
		if (this.currentRow != 4)
		{
			var hi_x = x+(this.currentChar*this.spacing)+this.offsets[this.currentRow];
			var hi_y = y+(this.currentRow*this.spacing);
			Video.drawRect(hi_x, hi_y, hi_x+this.spacing,  hi_y+this.spacing, 150, packColor(128, 0, 255, 0));
		}
		else
		{
			switch (this.current4)
			{
				case 0:
				{
					Video.drawRect(x+16, y+138, x+137, y+168, 150, packColor(128, 0, 255, 0));
					break;
				}
				case 1:
				{
					Video.drawRect(x+162, y+138, x+284, y+168, 150, packColor(128, 0, 255, 0));
					break;
				}
				case 2:
				{
					Video.drawRect(x+304, y+138, x+408, y+168, 150, packColor(128, 0, 255, 0));
					break;
				}
			};
		}
	}
}

