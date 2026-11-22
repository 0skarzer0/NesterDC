
function tvGetLinePositions(str, linePositions)
{
	var pos1 = 0;
	var posPrev = 0;
	var maxLineLength = 0;
	while (true)
	{
		posPrev = pos1;
		pos1 = str.indexOf("\n", pos1);
		if (pos1 < 0)
			break;
		linePositions[linePositions.length] = pos1;
		if ((pos1-posPrev) > maxLineLength)
			maxLineLength = pos1-posPrev;
		pos1++;
	}
	return maxLineLength;
}

function viewText(str, bgimg, bgBrightness, allowEdit)
{
	if (!bgimg)
		bgimg = null;
	if (!bgBrightness)
		bgBrightness = 255;
	if (!allowEdit)
		allowEdit = false;
	str += "\n";
	var fontname = "cour";
	var realFontSize = 14;
	var fontSize = 34;
	
	var myfont = new Font();
	myfont.load("/cd/fonts/"+fontname+"_"+realFontSize+".txf", realFontSize/74);
	myfont.setBilinear(true);
	
	var charWidth = myfont.getWidth("M", fontSize);
	
	var linePositions = new Array();
	var maxLineLength = tvGetLinePositions(str, linePositions);
	
	var curStartLine = 0;
	var curStartChar = 0;
	var repeatery = 0;
	var linesPerPage = 25; //25 is fullscreen
	var charsPerLine = 62;
	var maxScroll = (maxLineLength-charsPerLine)+1;
	
	var inEdit = false;
	var countInMode = 1000;
	var cursorLine = 0;
	var cursorChar = 0;
	
	var oskb = null;
	if (allowEdit)
		oskb = new OSKeyboard();
	
	//var test = new TestPoint(19, 310);
	
	var drawCounter = 0;
	while (true)
	{
		pad1.poll();
		
		if (inEdit)
		{
			if (countInMode < 30)
			{
				if ((countInMode%3)==0)
				{
					if (curStartLine < (linePositions.length-1) && linePositions.length > 10)
						curStartLine++;
					linesPerPage--;
				}
				countInMode++;
			}
			
			println("curStartLine="+curStartLine);
			curStartLine += Math.floor(pad1.YAXIS/100);
			if (maxLineLength > charsPerLine)
				curStartChar += Math.floor(pad1.XAXIS/100);
			if (curStartLine > (linePositions.length-linesPerPage))
				curStartLine--;
			if (curStartLine < 0)
				curStartLine = 0;
			if (curStartChar < 0)
				curStartChar = 0;
			if (curStartChar == maxScroll)
				curStartChar = maxScroll-1;
			
			var c = oskb.processInput();
			
			if (pad1.justPressed("B") || c == "DONE")
			{
				inEdit = false;
				countInMode = 0;
			}
			else if (c !== null)
			{
				if (c == "BS")
					str = str.substring(0, str.length-2)+"\n";
				else
					str = str.substring(0, str.length-1)+c+"\n";
				//println(str);
				linePositions = new Array();
				maxLineLength = tvGetLinePositions(str, linePositions);
				maxScroll = (maxLineLength-charsPerLine)+1;
			}
		}
		else
		{
			if (countInMode < 30)
			{
				if ((countInMode%3)==0)
				{
					if (curStartLine > 0 && linePositions.length > 10)
						curStartLine--;
					linesPerPage++;
				}
				countInMode++;
			}
			
			if (pad1.UP)
			{
				if ((repeatery%2)==0)
				{
					curStartLine--;
					if (curStartLine < 0)
						curStartLine++;
				}
				repeatery++;
			}
			else if (pad1.DOWN)
			{
				if ((repeatery%2)==0)
				{
					curStartLine++;
					if (curStartLine >= (linePositions.length-linesPerPage))
						curStartLine--;
				}
				repeatery++;
			}
			else
				repeatery = 0;

			if (pad1.LEFT)
			{
				curStartChar--;
				if (curStartChar < 0)
					curStartChar = 0;
			}
			else if (pad1.RIGHT)
			{
				curStartChar++;
				if (curStartChar == maxScroll)
					curStartChar = maxScroll-1;
			}

			if (pad1.justPressed("B"))
			{
				if (allowEdit)
					return str.substring(0, str.length-1);
				else
					return null;
			}
			if (allowEdit && pad1.justPressed("A"))
			{
				inEdit = true;
				countInMode = 0;
			}
		}
		
		Video.frameBegin();
		var curline = 0;
		for (var k = curStartLine; k < (curStartLine+linesPerPage); ++k)
		{
			if (k >= linePositions.length)
				break;
			var temp = null;
			if (k == 0)
				temp = str.substring(0, linePositions[0]);
			else if (k == linePositions.length-1)
			{
				temp = str.substring(linePositions[k-1]+1);
				if (inEdit && (drawCounter%30) < 15)
				{
					var testStr = (curStartChar > 0) ? temp.substr(curStartChar) : temp;
					var cursorX = myfont.getWidth(testStr, fontSize)+4;
					var cursorY = ((curline+1)*17)+4;
					//myfont.draw("|", 14+cursorX, ((curline+2)*17), 100, -1, fontSize, g_colors["red"]);
					Video.drawRect(cursorX, cursorY, cursorX+charWidth, cursorY+17, 150, packColor(128, 0, 255, 0));
				}
			}
			else
				temp = str.substring(linePositions[k-1]+1, linePositions[k]);
			if (curStartChar > 0)
				myfont.draw(temp.substr(curStartChar), 14, ((curline+2)*17), 100, 617, fontSize, g_colors["white"]);
			else
				myfont.draw(temp, 14, ((curline+2)*17), 100, 617, fontSize, g_colors["white"]);
			curline++;
		}
		if (inEdit && oskb)
			oskb.draw(107, 300);
		if (bgimg)
			bgimg.draw(0, 0, 1, packColor(255, bgBrightness, bgBrightness, bgBrightness), 10);
		Video.frameFinish();
		drawCounter++;
	}
}
